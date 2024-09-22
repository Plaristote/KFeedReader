#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QUrl>

class CloudProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url MEMBER m_url NOTIFY urlChanged)
    Q_PROPERTY(QByteArray authToken MEMBER m_authToken NOTIFY authTokenChanged)
public:
    CloudProvider(QObject *parent = nullptr);

    bool isAuthentifiable() const;
    void decorateRequest(QNetworkRequest &request) const;

    const QUrl &url() const
    {
        return m_url;
    }

    QNetworkAccessManager &network()
    {
        return m_network;
    }

public Q_SLOTS:
    void setSettings(const QUrl &url, const QByteArray &token);

Q_SIGNALS:
    void urlChanged();
    void authTokenChanged();
    void enabledChanged();

private:
    QUrl m_url;
    QByteArray m_authToken;
    QNetworkAccessManager m_network;
};
