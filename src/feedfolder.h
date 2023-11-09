#ifndef FEEDFOLDER_H
#define FEEDFOLDER_H

#include "menuitem.h"
#include <QQmlListProperty>
#include <QUrl>

class FeedFolder : public MenuItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl faviconUrl READ faviconUrl CONSTANT)
    Q_PROPERTY(QQmlListProperty<QObject> items READ items NOTIFY itemsChanged)
public:
    FeedFolder(QObject *parent = nullptr)
        : MenuItem(parent)
    {
    }

    QString view() const override
    {
        return QStringLiteral("qrc:/FeedFolder.qml");
    }
    QUrl faviconUrl() const
    {
        return QUrl(QStringLiteral("qrc:/icons/folder.png"));
    }
    qint64 unreadCount() const override;
    bool fetching() const override;
    double progress() const override;
    ItemType itemType() const override
    {
        return MenuItem::FolderMenuItem;
    }

    void loadFromJson(QJsonObject &) override;
    void saveToJson(QJsonObject &) override;
    static FeedFolder *createFromJson(QJsonObject &, QObject *parent = nullptr);

public Q_SLOTS:
    void addItem(QObject *);
    void removeItem(QObject *);
    void fetch() override;
    void remove() override;

Q_SIGNALS:
    void itemsChanged();

private:
    QQmlListProperty<QObject> items()
    {
        return QQmlListProperty<QObject>(this, &m_items);
    }

    QList<QObject *> m_items;
};

#endif // FEEDFOLDER_H
