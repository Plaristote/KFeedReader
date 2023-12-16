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
    FeedFolder(QObject *parent = nullptr);
    FeedFolder(FeedFolder &parent);
    ~FeedFolder();

    QString view() const override;
    QUrl faviconUrl() const override;
    qint64 unreadCount() const override;
    bool fetching() const override;
    double progress() const override;
    ItemType itemType() const override;
    void loadFromJson(QJsonObject &) override;
    void saveToJson(QJsonObject &) const override;
    static FeedFolder *createFromJson(QJsonObject &, FeedFolder *parent = nullptr);
    int indexOf(const QObject *) const override;
    int childCount() const override;
    MenuItem *childAt(int) const override;

public Q_SLOTS:
    void addItem(QObject *);
    void addItemAfter(QObject *, QObject *);
    void addItemBefore(QObject *, QObject *);
    void removeItem(QObject *);
    void fetch() override;
    void remove() override;

Q_SIGNALS:
    void itemsChanged();

private:
    void connectItem(MenuItem *);
    void disconnectItem(MenuItem *);
    void insertItemAt(QList<QObject *>::iterator, MenuItem *);

    QQmlListProperty<QObject> items()
    {
        return QQmlListProperty<QObject>(this, &m_items);
    }

    QList<QObject *> m_items;
};

#endif // FEEDFOLDER_H
