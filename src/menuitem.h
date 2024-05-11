#ifndef MENUITEM_H
#define MENUITEM_H

#include "ttlsettings.h"
#include <QQmlListProperty>
#include <QUrl>
#include <qobject.h>

class MenuItem : public TtlSettings
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(qint64 unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString view READ view NOTIFY viewChanged)
    Q_PROPERTY(ItemType type READ itemType CONSTANT)
    Q_PROPERTY(QQmlListProperty<QObject> crumbs READ qmlCrumbs NOTIFY crumbsChanged)
public:
    enum Role { DisplayNameRole = Qt::UserRole + 1, DescriptionRole, IconUrlRole, MenuItemRole };
    enum ItemType { NoItemType = 0, FolderMenuItem, FeedMenuItem };
    Q_ENUM(ItemType)

    MenuItem(QObject *parent = nullptr);
    MenuItem(MenuItem &parent);

    static MenuItem *fromIndex(const QModelIndex &item);
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &item) const override;
    int rowCount(const QModelIndex &item) const override;
    int columnCount(const QModelIndex &) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool isAncestorOf(const MenuItem *) const;

    virtual QUrl faviconUrl() const;
    const QString &name() const;
    const QString &description() const;
    virtual qint64 unreadCount() const;
    virtual bool fetching() const;
    virtual double progress() const;
    virtual void loadFromJson(QJsonObject &) override;
    virtual void saveToJson(QJsonObject &) const override;
    virtual ItemType itemType() const;
    virtual QString view() const;
    virtual int indexOf(const QObject *) const;
    virtual int childCount() const;
    virtual MenuItem *childAt(int) const;
    MenuItem *parentItem() const;
    int row() const;

    virtual int autoUpdateEnabled() const override;
    virtual void enableAutoUpdate(bool) override;

    QQmlListProperty<QObject> qmlCrumbs()
    {
        return QQmlListProperty<QObject>(this, reinterpret_cast<QList<QObject *> *>(&m_crumbs));
    }

public Q_SLOTS:
    void setName(const QString &name);
    void setDescription(const QString &description);
    void setParentItem(MenuItem *);
    void reparent(MenuItem *target, MenuItem *subject);
    void appendNextToSibling(MenuItem *item, MenuItem *sibling);
    void appendBeforeSibling(MenuItem *item, MenuItem *sibling);
    virtual void fetch(){};
    virtual void remove();
    virtual void triggerBeforeSave(){};
    virtual void markAsRead(){};
    void updateCrumbs();

Q_SIGNALS:
    void nameChanged(const QString &name);
    void descriptionChanged(const QString &description);
    void viewChanged();
    void unreadCountChanged();
    void fetchingChanged();
    void progressChanged();
    void parentChanged();
    void beforeSave();
    void removed(QObject *);
    void crumbsChanged();

private:
    QString m_name;
    QString m_description;
    MenuItem *m_parentItem = nullptr;
    QList<MenuItem *> m_crumbs;
};

#endif // MENUITEM_H
