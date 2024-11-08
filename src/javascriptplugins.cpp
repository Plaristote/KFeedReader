#include "javascriptplugins.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>

static QString pluginsFolder()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/javascript/");
}

static QString settingsPath()
{
    return pluginsFolder() + QStringLiteral("plugins.json");
}

JavaScriptPlugins::JavaScriptPlugins(QObject* parent) : QObject(parent)
{
    QFile file(settingsPath());

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        QJsonArray root = document.array();

        for (QJsonValue value : document.array()) {
            if (value.isObject()) {
                m_plugins << JavaScriptPlugin::factory(this, value.toObject());
            } else {
                qDebug() << "JavaScriptPlugins: plugins.json: malformed value";
            }
        }
    }
}

JavaScriptPlugin::JavaScriptPlugin(QObject* parent) : QObject(parent)
{
}

JavaScriptPlugin * JavaScriptPlugin::factory(QObject *parent, const QJsonObject &data)
{
    JavaScriptPlugin* plugin = new JavaScriptPlugin(parent);
    QJsonValue domains = data[QStringLiteral("domains")];
    QJsonValue source = data[QStringLiteral("src")];

    if (source.isString()) {
        QString filepath = pluginsFolder() + source.toString();
        QFile file(filepath);

        plugin->m_name = file.fileName();
        if (file.open(QIODevice::ReadOnly))
            plugin->m_source.assign(file.readAll());
        else
            qDebug() << "JavaScriptPlugins: could not read source file" << filepath;
    }
    if (domains.isArray())
        plugin->m_domainNames = domains.toVariant().toStringList();
    return plugin;
}

bool JavaScriptPlugin::matchesDomain(const QUrl& url) const
{
  return m_domainNames.size() == 0 || m_domainNames.indexOf(url.host()) > 0;
}
