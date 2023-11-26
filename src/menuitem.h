#ifndef MENUITEM_H
#define MENUITEM_H

#include <QUrl>
#include <qobject.h>

/**
 * @todo write docs
 */
class MenuItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(qint64 unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(bool fetching READ fetching NOTIFY fetchingChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString view READ view CONSTANT)
    Q_PROPERTY(ItemType type READ itemType CONSTANT)

public:
    enum ItemType { NoItemType = 0, FolderMenuItem, FeedMenuItem };
    Q_ENUM(ItemType)

    MenuItem(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    MenuItem(MenuItem &parent)
        : QObject(&parent)
        , m_parentItem(&parent)
    {
    }

    virtual QUrl faviconUrl() const
    {
        return QUrl();
    }

    const QString &name() const
    {
        return m_name;
    }
    const QString &description() const
    {
        return m_description;
    }
    virtual qint64 unreadCount() const
    {
        return 0;
    }
    virtual bool fetching() const
    {
        return false;
    }
    virtual double progress() const
    {
        return 0;
    }
    virtual void loadFromJson(QJsonObject &);
    virtual void saveToJson(QJsonObject &);
    virtual ItemType itemType() const
    {
        return NoItemType;
    }
    virtual QString view() const
    {
        return QString();
    }

    virtual int indexOf(const QObject *) const
    {
        return -1;
    }

    virtual int childCount() const
    {
        return 0;
    }

    virtual MenuItem *childAt(int) const
    {
        return nullptr;
    }

    MenuItem *parentItem() const;

    void setParentItem(MenuItem *);

    int row() const;

public Q_SLOTS:
    void setName(const QString &name);
    void setDescription(const QString &description);
    virtual void fetch(){};
    virtual void remove();

Q_SIGNALS:
    void nameChanged(const QString &name);
    void descriptionChanged(const QString &description);
    void unreadCountChanged();
    void fetchingChanged();
    void progressChanged();
    void parentChanged();
    void removed(QObject *);

private:
    QString m_name;
    QString m_description;
    MenuItem *m_parentItem = nullptr;
};

#endif // MENUITEM_H
