#ifndef FEEDUPDATE_H
#define FEEDUPDATE_H

#include <QObject>
#include <QTimer>

class Feed;

class FeedUpdater : public QObject
{
    Q_OBJECT
public:
    enum TtlType { TtlInMinutes = 0, TtlInHours };
    Q_ENUM(TtlType)

    FeedUpdater(Feed &feed);

    Q_INVOKABLE static int ttlInUnits(int ttl, TtlType type);

public Q_SLOTS:
    void resetUpdateTimer();
    void restartUpdateTimer();

private Q_SLOTS:
    void timerTicked();

private:
    Feed &feed;
    QTimer m_updateTimer;
};

#endif
