#include "feedarticleenclosure.h"
#include <QDomElement>
#include <QJsonObject>

FeedArticleEnclosure::FeedArticleEnclosure(QObject *parent)
    : FeedAttachment(parent)
{
}

void FeedArticleEnclosure::saveToJson(QJsonObject &out) const
{
    out.insert(QStringLiteral("_type"), static_cast<int>(EnclosureAttachment));
    if (!m_title.isEmpty())
        out.insert(QStringLiteral("title"), title());
    out.insert(QStringLiteral("type"), type());
    out.insert(QStringLiteral("size"), size());
    out.insert(QStringLiteral("url"), url().toString());
}

void FeedArticleEnclosure::loadFromJson(const QJsonObject &in)
{
    setTitle(in.value(QStringLiteral("title")).toString());
    setType(in.value(QStringLiteral("type")).toString());
    setSize(in.value(QStringLiteral("size")).toInt());
    setUrl(QUrl(in.value(QStringLiteral("url")).toString()));
}

void FeedArticleEnclosure::loadFromXml(const QDomElement &element)
{
    setSize(element.attribute(QStringLiteral("length")).toInt());
    setType(element.attribute(QStringLiteral("type")));
    setUrl(QUrl(element.attribute(QStringLiteral("url"))));
}

void FeedArticleEnclosure::setTitle(const QString &value)
{
    if (value != m_title) {
        m_title = value;
        Q_EMIT titleChanged(value);
    }
}

void FeedArticleEnclosure::setType(const QString &value)
{
    if (value != m_type) {
        m_type = value;
        Q_EMIT typeChanged(value);
    }
}

void FeedArticleEnclosure::setSize(qint64 value)
{
    if (value != m_size) {
        m_size = value;
        Q_EMIT sizeChanged(value);
    }
}

void FeedArticleEnclosure::setUrl(const QUrl &value)
{
    if (value != m_url) {
        m_url = value;
        Q_EMIT urlChanged(value);
    }
}
