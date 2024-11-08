#pragma once

#include <QObject>
#include <QUrl>
#include <QList>
#include <QQmlListProperty>

class QJsonObject;

class JavaScriptPlugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString source READ source CONSTANT)
public:
    explicit JavaScriptPlugin(QObject *parent = nullptr);

    static JavaScriptPlugin* factory(QObject *parent, const QJsonObject&);

    const QString& name() const
    {
        return m_name;
    }

    const QString& source() const
    {
        return m_source;
    }

    Q_INVOKABLE bool matchesDomain(const QUrl&) const;

private:
    QString m_name;
    QString m_source;
    QStringList m_domainNames;
};

class JavaScriptPlugins : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<JavaScriptPlugin> plugins READ qmlPlugins CONSTANT)
public:
    explicit JavaScriptPlugins(QObject *parent = nullptr);

    QQmlListProperty<JavaScriptPlugin> qmlPlugins()
    {
      return QQmlListProperty<JavaScriptPlugin>(this, &m_plugins);
    }

private:
    QList<JavaScriptPlugin*> m_plugins;
};
