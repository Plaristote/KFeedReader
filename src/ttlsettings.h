#ifndef TTLSETTINGS_H
#define TTLSETTINGS_H

#include <QObject>
#include <QVector>
#include <optional>

class TtlSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int customTtl READ customTtl WRITE setCustomTtl NOTIFY customTtlChanged)
    Q_PROPERTY(bool useCustomTtl READ useCustomTtl WRITE setUseCustomTtl NOTIFY customTtlChanged)
    Q_PROPERTY(int autoUpdateEnabled READ autoUpdateEnabled NOTIFY customTtlChanged)
public:
    TtlSettings(QObject *parent = nullptr);

    Q_INVOKABLE int ttlInUnits(qint64 value, int type) const;

    int customTtl() const;
    bool useCustomTtl() const;
    virtual int autoUpdateEnabled() const;
    Q_INVOKABLE virtual bool isSkippedHour(unsigned short index) const;
    Q_INVOKABLE virtual bool isSkippedDay(unsigned short index) const;

    virtual void loadFromJson(QJsonObject &);
    virtual void saveToJson(QJsonObject &) const;

public Q_SLOTS:
    void setCustomTtl(int);
    void setUseCustomTtl(bool);
    void setSkipHour(unsigned short index, bool skipped = true);
    void setSkipDay(unsigned short index, bool skipped = true);
    virtual void enableAutoUpdate(bool);

Q_SIGNALS:
    void customTtlChanged();
    void skipDaysChanged();
    void skipHoursChanged();

private:
    std::optional<int> m_customTtl;

protected:
    bool m_autoUpdateEnabled = true;
    QVector<unsigned short> m_customSkipDays;
    QVector<unsigned short> m_customSkipHours;
};

#endif
