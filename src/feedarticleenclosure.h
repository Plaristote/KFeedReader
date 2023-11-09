#ifndef FEEDARTICLEENCLOSURE_H
#define FEEDARTICLEENCLOSURE_H

#include <QUrl>
#include <qobject.h>

class QDomElement;

class FeedArticleEnclosure : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(qint64 size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
public:
    FeedArticleEnclosure(QObject *parent = nullptr);

    void loadFromXml(const QDomElement &);

    Q_INVOKABLE QUrl qmlView() const
    {
        return QUrl(QStringLiteral("qrc:/ArticleEnclosure.qml"));
    }

    const QString &type() const
    {
        return m_type;
    }
    qint64 size() const
    {
        return m_size;
    }
    const QUrl &url() const
    {
        return m_url;
    }

public Q_SLOTS:
    void setType(const QString &);
    void setSize(qint64);
    void setUrl(const QUrl &);

Q_SIGNALS:
    void typeChanged(const QString &type);
    void sizeChanged(qint64 size);
    void urlChanged(const QUrl &url);

private:
    QString m_type;
    qint64 m_size;
    QUrl m_url;
};

#endif
