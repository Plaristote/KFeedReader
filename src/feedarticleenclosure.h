#ifndef FEEDARTICLEENCLOSURE_H
#define FEEDARTICLEENCLOSURE_H

#include "feedattachment.h"
#include <QUrl>
#include <qobject.h>

class QDomElement;

class FeedArticleEnclosure : public FeedAttachment
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(qint64 size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
public:
    FeedArticleEnclosure(QObject *parent = nullptr);

    void saveToJson(QJsonObject &) const override;
    void loadFromJson(const QJsonObject &) override;

    void loadFromXml(const QDomElement &);

    Q_INVOKABLE QUrl qmlView() const;

    const QString &title() const
    {
        return m_title;
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
    void setTitle(const QString &);
    void setType(const QString &);
    void setSize(qint64);
    void setUrl(const QUrl &);

Q_SIGNALS:
    void titleChanged(const QString &title);
    void typeChanged(const QString &type);
    void sizeChanged(qint64 size);
    void urlChanged(const QUrl &url);

private:
    QString m_title;
    QString m_type;
    qint64 m_size = 0;
    QUrl m_url;
};

#endif
