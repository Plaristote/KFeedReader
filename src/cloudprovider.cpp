#include "cloudprovider.h"
#include <QSettings>

CloudProvider::CloudProvider(QObject *parent)
    : QObject(parent)
{
    QSettings settings(QStringLiteral("cloudProvider"));

    m_url = settings.value("url").toUrl();
    m_authToken = settings.value("token").toByteArray();
    connect(this, &CloudProvider::urlChanged, this, &CloudProvider::enabledChanged);
    connect(this, &CloudProvider::authTokenChanged, this, &CloudProvider::enabledChanged);
}

void CloudProvider::setSettings(const QUrl &url, const QByteArray &token)
{
    QSettings settings(QStringLiteral("CloudProvider"));

    m_url = url;
    m_authToken = token;
    settings.setValue("url", m_url);
    settings.setValue("token", m_authToken);
}

bool CloudProvider::isAuthentifiable() const
{
    return m_authToken.length() && !m_url.isEmpty();
}

void CloudProvider::decorateRequest(QNetworkRequest &request) const
{
    request.setRawHeader(QStringLiteral("Accept").toUtf8(), QStringLiteral("application/json").toUtf8());
    request.setRawHeader(QStringLiteral("X-AppToken").toUtf8(), m_authToken);
}
