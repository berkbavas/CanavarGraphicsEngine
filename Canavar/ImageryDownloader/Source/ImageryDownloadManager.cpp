#include "ImageryDownloadManager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkRequest>

ImageryDownloadManager::ImageryDownloadManager(QObject* parent)
    : QObject(parent)
{}

ImageryDownloadManager::~ImageryDownloadManager()
{
    mRunning = false;
    for (auto* reply : mActiveReplies.keys())
        reply->abort();
}

void ImageryDownloadManager::setJobs(QVector<ImgTileJob> jobs)
{
    mJobs = std::move(jobs);
    mNextIndex = 0;
}

void ImageryDownloadManager::start(int concurrency)
{
    if (mRunning)
        return;

    mConcurrency = concurrency;
    mRunning = true;
    mNextIndex = 0;

    for (int i = 0; i < mConcurrency; ++i)
        startNext();
}

void ImageryDownloadManager::abort()
{
    if (!mRunning)
        return;

    mRunning = false;
    for (auto* reply : mActiveReplies.keys())
        reply->abort();
}

int ImageryDownloadManager::doneCount() const
{
    int n = 0;
    for (const auto& j : mJobs)
        if (j.status == TileStatus::Done || j.status == TileStatus::Skipped ||
            j.status == TileStatus::Failed)
            ++n;
    return n;
}

void ImageryDownloadManager::startNext()
{
    if (!mRunning)
        return;
    if (mActiveCount >= mConcurrency)
        return;

    while (mNextIndex < mJobs.size())
    {
        if (mJobs[mNextIndex].status == TileStatus::Pending)
        {
            startJob(mNextIndex++);
            return;
        }
        ++mNextIndex;
    }

    if (mActiveCount == 0)
    {
        mRunning = false;
        emit allFinished();
    }
}

void ImageryDownloadManager::startJob(int index)
{
    auto& job = mJobs[index];
    job.status = TileStatus::Active;
    job.errorMsg.clear();

    QNetworkRequest request(QUrl(job.url));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    // Polite User-Agent with contact info as recommended by tile server operators.
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "CanavarGraphicsEngine/1.0 ImageryDownloader "
                      "(github.com/CanavarGraphicsEngine; not-for-production)");

    auto* reply = mNam.get(request);
    mActiveReplies[reply] = index;
    ++mActiveCount;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const int idx = mActiveReplies.take(reply);
        auto& job = mJobs[idx];
        --mActiveCount;

        const int httpStatus =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (reply->error() == QNetworkReply::OperationCanceledError)
        {
            job.status = TileStatus::Pending;
        }
        else if (httpStatus == 404)
        {
            job.status = TileStatus::Skipped;
            job.errorMsg = "Not found";
        }
        else if (reply->error() != QNetworkReply::NoError)
        {
            job.status = TileStatus::Failed;
            job.errorMsg = reply->errorString();
        }
        else
        {
            const QByteArray data = reply->readAll();
            QFileInfo fi(job.filePath);
            fi.dir().mkpath(".");
            QFile f(job.filePath);
            if (f.open(QIODevice::WriteOnly) && f.write(data) == data.size())
            {
                job.status = TileStatus::Done;
            }
            else
            {
                job.status = TileStatus::Failed;
                job.errorMsg = f.errorString();
            }
        }

        reply->deleteLater();

        const int done = doneCount();
        emit progressChanged(done, (int)mJobs.size());

        if (mRunning)
            startNext();
        else if (mActiveReplies.isEmpty())
            emit allFinished();
    });

    emit progressChanged(doneCount(), (int)mJobs.size());
}
