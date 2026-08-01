#pragma once

#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QVector>

enum class TileStatus
{
    Pending,
    Active,
    Done,
    Failed,
    Skipped,
};

struct ImgTileJob
{
    int z{ 0 };
    int x{ 0 };
    int y{ 0 };
    QString url;
    QString filePath;
    TileStatus status{ TileStatus::Pending };
    QString errorMsg;
};

class ImageryDownloadManager : public QObject
{
    Q_OBJECT

  public:
    explicit ImageryDownloadManager(QObject* parent = nullptr);
    ~ImageryDownloadManager() override;

    void setJobs(QVector<ImgTileJob> jobs);
    void start(int concurrency = 8);
    void abort();

    const QVector<ImgTileJob>& jobs() const { return mJobs; }
    bool isRunning() const { return mRunning; }
    int doneCount() const;

  signals:
    void progressChanged(int done, int total);
    void allFinished();

  private:
    void startNext();
    void startJob(int index);

    QNetworkAccessManager mNam;
    QVector<ImgTileJob> mJobs;
    QHash<QNetworkReply*, int> mActiveReplies;
    int mConcurrency{ 8 };
    int mNextIndex{ 0 };
    int mActiveCount{ 0 };
    bool mRunning{ false };
};
