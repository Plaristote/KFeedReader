#include "ttlsettings.h"
#include "feedupdate.h"
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

TtlSettings::TtlSettings(QObject *parent)
    : QAbstractItemModel(parent)
{
}

int TtlSettings::ttlInUnits(qint64 value, int type) const
{
    return FeedUpdater::ttlInUnits(value, static_cast<FeedUpdater::TtlType>(type));
}

int TtlSettings::customTtl() const
{
    return m_customTtl.has_value() ? *m_customTtl : 60;
}

bool TtlSettings::useCustomTtl() const
{
    return m_customTtl.has_value() && *m_customTtl != 0;
}

int TtlSettings::autoUpdateEnabled() const
{
    return m_autoUpdateEnabled ? 2 : 0;
}

void TtlSettings::enableAutoUpdate(bool trigger)
{
    m_autoUpdateEnabled = trigger;
}

bool TtlSettings::isSkippedHour(unsigned short value) const
{
    return m_customTtl.has_value() ? m_customSkipHours.indexOf(value) >= 0 : false;
}

bool TtlSettings::isSkippedDay(unsigned short value) const
{
    return m_customTtl.has_value() ? m_customSkipDays.indexOf(value) >= 0 : false;
}

void TtlSettings::setCustomTtl(int value)
{
    if (!m_customTtl.has_value() || *m_customTtl != value) {
        m_customTtl = value;
        Q_EMIT customTtlChanged();
    }
}

void TtlSettings::setUseCustomTtl(bool value)
{
    bool has_value = m_customTtl.has_value();

    if (has_value && !value) {
        m_customTtl.reset();
    } else if (!has_value && value) {
        m_customTtl = 60;
    } else {
        return;
    }
    Q_EMIT customTtlChanged();
}

void TtlSettings::setSkipHour(unsigned short value, bool skipped)
{
    int index = m_customSkipHours.indexOf(value);

    if (index < 0 && skipped)
        m_customSkipHours.append(value);
    else if (index >= 0 && !skipped)
        m_customSkipHours.removeAt(index);
    else
        return;
    Q_EMIT skipHoursChanged();
}

void TtlSettings::setSkipDay(unsigned short value, bool skipped)
{
    int index = m_customSkipDays.indexOf(value);

    if (index < 0 && skipped)
        m_customSkipDays.append(value);
    else if (index >= 0 && !skipped)
        m_customSkipDays.removeAt(index);
    else
        return;
    Q_EMIT skipDaysChanged();
}

void TtlSettings::loadFromJson(QJsonObject &root)
{
    QJsonValue jsonCustomTtl = root.value(QStringLiteral("customTtl"));
    QJsonValue jsonAutoUpdate = root.value(QStringLiteral("autoUpdateEnabled"));

    if (!jsonCustomTtl.isNull()) {
        QJsonArray jsonSkipHours = root.value(QStringLiteral("customSkipHours")).toArray();
        QJsonArray jsonSkipDays = root.value(QStringLiteral("customSkipDays")).toArray();
        auto transformer = [](const QJsonValue &value) -> unsigned short {
            return value.toInt();
        };

        setCustomTtl(jsonCustomTtl.toInt());
        std::transform(jsonSkipDays.begin(), jsonSkipDays.end(), std::back_inserter(m_customSkipDays), transformer);
        std::transform(jsonSkipHours.begin(), jsonSkipHours.end(), std::back_inserter(m_customSkipHours), transformer);
    }
    qDebug() << this << " with auto update: " << jsonAutoUpdate.isNull() << jsonAutoUpdate.toBool();
    enableAutoUpdate(jsonAutoUpdate.isNull() || jsonAutoUpdate.toBool());
}

void TtlSettings::saveToJson(QJsonObject &root) const
{
    if (useCustomTtl()) {
        QJsonArray skipDays, skipHours;

        std::copy(m_customSkipDays.begin(), m_customSkipDays.end(), std::back_inserter(skipDays));
        std::copy(m_customSkipHours.begin(), m_customSkipHours.end(), std::back_inserter(skipHours));
        root.insert(QStringLiteral("customTtl"), *m_customTtl);
        root.insert(QStringLiteral("customSkipDays"), skipDays);
        root.insert(QStringLiteral("customSkipHours"), skipHours);
    }
    root.insert(QStringLiteral("autoUpdateEnabled"), m_autoUpdateEnabled);
}
