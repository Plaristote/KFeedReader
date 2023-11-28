#include "feedarticleenclosure.h"
#include <QDomElement>

FeedArticleEnclosure::FeedArticleEnclosure(QObject *parent)
    : QObject(parent)
{
}

void FeedArticleEnclosure::loadFromXml(const QDomElement &element)
{
    setSize(element.attribute(QStringLiteral("length")).toInt());
    setType(element.attribute(QStringLiteral("type")));
    setUrl(QUrl(element.attribute(QStringLiteral("url"))));
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
