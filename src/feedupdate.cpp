#include "feedupdate.h"
#include "feed.h"
#include <QSettings>

FeedUpdater::FeedUpdater(Feed &feed)
    : feed(feed)
{
    connect(&feed, &Feed::ttlChanged, this, &FeedUpdater::resetUpdateTimer);
    connect(&feed, &Feed::customTtlChanged, this, &FeedUpdater::resetUpdateTimer);
    connect(&feed, &Feed::lastUpdateChanged, this, &FeedUpdater::resetUpdateTimer);
    connect(&feed, &Feed::scheduledUpdateChanged, this, &FeedUpdater::restartUpdateTimer);
    connect(&m_updateTimer, &QTimer::timeout, this, &FeedUpdater::timerTicked);
}

void FeedUpdater::timerTicked()
{
    if (feed.autoUpdateEnabled())
        feed.fetch();
}

int FeedUpdater::ttlInUnits(int ttl, TtlType type)
{
    switch (type) {
    case TtlInMinutes:
        break;
    case TtlInHours:
        return ttl * 60;
    }
    return ttl;
}

void FeedUpdater::resetUpdateTimer()
{
    QSettings settings;
    qint64 ttl = feed.m_ttl > 0 ? feed.m_ttl : settings.value(QStringLiteral("defaultTTL"), 60).toInt();
    QDateTime nextUpdate;

    if (feed.m_useCustomTtl && feed.m_customTtl > 0)
        ttl = feed.m_customTtl;
    if (feed.m_lastUpdate.isNull())
        feed.m_lastUpdate = QDateTime::currentDateTime();
    nextUpdate = QDateTime(feed.m_lastUpdate).addSecs(ttl * 60);
    for (char i = 0; i < 7 && feed.isSkippedDay(nextUpdate.date().dayOfWeek()); ++i)
        nextUpdate = nextUpdate.addDays(1);
    for (char i = 0; i < 24 && feed.isSkippedHour(nextUpdate.time().hour()); ++i)
        nextUpdate = nextUpdate.addSecs(3600);
    feed.setScheduledUpdate(nextUpdate);
}

void FeedUpdater::restartUpdateTimer()
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 interval;

    if (now < feed.m_scheduledUpdate)
        interval = now.msecsTo(feed.m_scheduledUpdate);
    else
        interval = 500;
    m_updateTimer.stop();
    m_updateTimer.setInterval(interval);
    m_updateTimer.start();
}
