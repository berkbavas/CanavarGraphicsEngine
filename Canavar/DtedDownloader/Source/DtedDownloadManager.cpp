#include "DtedDownloadManager.h"

#include <QDir>
#include <QFileInfo>
#include <QNetworkRequest>

DtedDownloadManager::DtedDownloadManager(QObject* parent)
    : QObject(parent)
{}

DtedDownloadManager::~DtedDownloadManager()
{
    // Abort without emitting signals – we're being destroyed.
    mRunning = false;
    for (auto* reply : mActiveReplies.keys())
        reply->abort();
}

void DtedDownloadManager::setJobs(QVector<DtedTileJob> jobs)
{
    mJobs = std::move(jobs);
    mNextIndex = 0;
}

void DtedDownloadManager::start(int concurrency)
{
    if (mRunning)
        return;

    mConcurrency = concurrency;
    mRunning = true;
    mNextIndex = 0;

    for (int i = 0; i < mConcurrency; ++i)
        startNext();
}

void DtedDownloadManager::abort()
{
    if (!mRunning)
        return;

    mRunning = false;
    // Each aborted reply will emit finished() → onReplyFinished() resets it to Pending.
    for (auto* reply : mActiveReplies.keys())
        reply->abort();
}

void DtedDownloadManager::startNext()
{
    if (!mRunning)
        return;
    if (mActiveCount >= mConcurrency)
        return;

    while (mNextIndex < mJobs.size())
    {
        if (mJobs[mNextIndex].status == JobStatus::Pending)
        {
            startJob(mNextIndex++);
            return;
        }
        ++mNextIndex;
    }

    // No more pending jobs – wait for active ones to finish.
    if (mActiveCount == 0)
    {
        mRunning = false;
        emit allFinished();
    }
}

void DtedDownloadManager::startJob(int index)
{
    auto& job = mJobs[index];
    job.status = JobStatus::Active;
    job.bytesReceived = 0;
    job.bytesTotal = 0;
    job.errorMsg.clear();

    // Ensure the output directory exists.
    QFileInfo fi(job.filePath);
    fi.dir().mkpath(".");

    auto* file = new QFile(job.filePath);
    if (!file->open(QIODevice::WriteOnly))
    {
        job.status = JobStatus::Failed;
        job.errorMsg = "Cannot create file: " + file->errorString();
        delete file;
        emit progressChanged();
        startNext();
        return;
    }

    QNetworkRequest request(QUrl(job.url));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "CanavarGraphicsEngine/1.0 DtedDownloader");

    auto* reply = mNam.get(request);
    mActiveReplies[reply] = index;
    mActiveFiles[reply] = file;
    ++mActiveCount;

    // Stream response body directly to disk.
    connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
        file->write(reply->readAll());
    });

    connect(reply, &QNetworkReply::downloadProgress, this, [this, reply](qint64 received, qint64 total) {
        if (int idx = mActiveReplies.value(reply, -1); idx >= 0)
        {
            mJobs[idx].bytesReceived = received;
            mJobs[idx].bytesTotal = total;
            emit progressChanged();
        }
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });

    emit progressChanged();
}

void DtedDownloadManager::onReplyFinished(QNetworkReply* reply)
{
    int idx = mActiveReplies.take(reply);
    auto* file = mActiveFiles.take(reply);
    --mActiveCount;

    auto& job = mJobs[idx];
    file->close();

    const int httpStatus =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() == QNetworkReply::OperationCanceledError)
    {
        // Aborted – reset so this tile can be retried.
        file->remove();
        job.status = JobStatus::Pending;
        job.bytesReceived = 0;
        job.bytesTotal = 0;
    }
    else if (httpStatus == 404)
    {
        // Tile doesn't exist (ocean / void area) – not an error.
        file->remove();
        job.status = JobStatus::Skipped;
        job.errorMsg = "Not found (ocean/void)";
    }
    else if (reply->error() != QNetworkReply::NoError)
    {
        file->remove();
        job.status = JobStatus::Failed;
        job.errorMsg = reply->errorString();
    }
    else
    {
        job.status = JobStatus::Done;
    }

    delete file;
    reply->deleteLater();

    emit progressChanged();

    if (mRunning)
        startNext();
    else if (mActiveReplies.isEmpty())
        emit allFinished();
}
