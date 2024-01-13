#include "../feed.h"
#include "../feedfolder.h"
#include <QDebug>
#include <QFile>

static bool feedsUrlMatch(Feed *feed, const QUrl &url)
{
    QUrl feedUrl = feed->xmlUrl();

    feedUrl.setScheme(url.scheme());
    return feedUrl == url;
}

static Feed *findFeedByUrl(FeedFolder *folder, const QUrl &url)
{
    Feed *result = nullptr;

    for (int i = 0; i < folder->childCount() && !result; ++i) {
        MenuItem *item = folder->childAt(i);

        switch (item->itemType()) {
        case MenuItem::FolderMenuItem:
            result = findFeedByUrl(reinterpret_cast<FeedFolder *>(item), url);
            break;
        case MenuItem::FeedMenuItem:
            if (feedsUrlMatch(reinterpret_cast<Feed *>(item), url)) {
                result = reinterpret_cast<Feed *>(item);
            }
            break;
        default:
            break;
        }
    }
    return result;
}

static void importNextSubscription(FeedFolder *folder, QStringList lines)
{
    static const QString urlPrefix(QStringLiteral("https://www.youtube.com/feeds/videos.xml?channel_id="));
    QStringList columns = lines.takeFirst().split(QStringLiteral(","));

    if (columns.size() > 1) {
        QString channelId = columns.first();
        QString channelName = columns.last();
        QUrl feedUrl(urlPrefix + channelId);

        if (!findFeedByUrl(folder, feedUrl)) {
            Feed *feed = new Feed(folder);

            feed->setName(columns.last());
            feed->setXmlUrl(feedUrl);
            feed->setScheduledUpdate(QDateTime::currentDateTime().addSecs(1));
            folder->addItem(feed);
        }
    }
    if (lines.size()) {
        QTimer *timer = new QTimer(folder);

        QObject::connect(timer, &QTimer::timeout, folder, [=]() {
            importNextSubscription(folder, lines);
            timer->deleteLater();
        });
        timer->start(750);
    }
}

static void importYouTubeSubscribe(FeedFolder *folder, QString csv)
{
    QStringList lines = csv.split(QStringLiteral("\n"));

    if (lines.size() > 0) {
        lines.erase(lines.begin());
        importNextSubscription(folder, lines);
    }
}

void FeedFolder::synchronizeWithYouTube(const QUrl &takeoutUrl)
{
    QFile file(takeoutUrl.path());

    if (file.open(QIODevice::ReadOnly)) {
        importYouTubeSubscribe(this, QString::fromUtf8(file.readAll()));
    }
}
