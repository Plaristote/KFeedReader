#ifndef FEEDATTACHMENT_H
#define FEEDATTACHMENT_H

#include <qobject.h>

class FeedAttachment : public QObject
{
public:
    enum AttachmentTypes { EnclosureAttachment = 0, MediaAttachment };

    FeedAttachment(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    virtual void saveToJson(QJsonObject &) const = 0;
    virtual void loadFromJson(const QJsonObject &) = 0;
};

#endif
