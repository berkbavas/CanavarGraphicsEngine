#pragma once

#include <QFile>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QVector>

enum class JobStatus
{
    Pending,
    Active,
    Done,
    Failed,
    Skipped,
};

struct DtedTileJob
{
    int lat{ 0 };
    int lon{ 0 };
    QString url;
    QString filePath;
    JobStatus status{ JobStatus::Pending };
    qint64 bytesReceived{ 0 };
    qint64 bytesTotal{ 0 };
    QString errorMsg;
};

class DtedDownloadManager : public QObject
{
    Q_OBJECT

  public:
    explicit DtedDownloadManager(QObject* parent = nullptr);
    ~DtedDownloadManager() override;

    void setJobs(QVector<DtedTileJob> jobs);
    void start(int concurrency = 4);
    void abort();

    const QVector<DtedTileJob>& jobs() const { return mJobs; }
    bool isRunning() const { return mRunning; }

  signals:
    void progressChanged();
    void allFinished();

  private:
    void startNext();
    void startJob(int index);
    void onReplyFinished(QNetworkReply* reply);

    QNetworkAccessManager mNam;
    QVector<DtedTileJob> mJobs;
    QHash<QNetworkReply*, int> mActiveReplies;
    QHash<QNetworkReply*, QFile*> mActiveFiles;
    int mConcurrency{ 4 };
    int mNextIndex{ 0 };
    int mActiveCount{ 0 };
    bool mRunning{ false };
};
