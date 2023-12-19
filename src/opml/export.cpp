#include "../feed.h"
#include "../feedfolder.h"
#include <QDomDocument>

static void exportOpmlOutline(const Feed *feed, QDomDocument &document, QDomElement &parent)
{
    QDomElement outline = document.createElement(QStringLiteral("outline"));

    outline.setAttribute(QStringLiteral("type"), QStringLiteral("rss"));
    outline.setAttribute(QStringLiteral("text"), feed->name());
    outline.setAttribute(QStringLiteral("title"), feed->name());
    outline.setAttribute(QStringLiteral("description"), feed->description());
    outline.setAttribute(QStringLiteral("copyright"), feed->copyright());
    outline.setAttribute(QStringLiteral("xmlUrl"), feed->xmlUrl().toString());
    outline.setAttribute(QStringLiteral("htmlUrl"), feed->link().toString());
    outline.setAttribute(QStringLiteral("maxArticleAge"), QString::number(feed->maxArticleAge()));
    outline.setAttribute(QStringLiteral("maxArticleNumber"), QString::number(feed->maxArticleNumber()));
    outline.setAttribute(QStringLiteral("faviconUrl"), feed->faviconUrl().toString());
    outline.setAttribute(QStringLiteral("version"), QStringLiteral("RSS"));
    if (feed->useCustomTtl()) {
        outline.setAttribute(QStringLiteral("useCustomFetchInterval"), QStringLiteral("true"));
        outline.setAttribute(QStringLiteral("fetchInterval"), QString::number(feed->customTtl()));
    } else {
        outline.setAttribute(QStringLiteral("useCustomFetchInterval"), QStringLiteral("false"));
        outline.setAttribute(QStringLiteral("fetchInterval"), QString::number(feed->ttl()));
    }
    parent.appendChild(outline);
}

static void exportOpmlOutline(const FeedFolder *folder, QDomDocument &document, QDomElement &parent)
{
    QDomElement outline;

    if (folder->parentItem()) {
        outline = document.createElement(QStringLiteral("outline"));
        outline.setAttribute(QStringLiteral("text"), folder->name());
        outline.setAttribute(QStringLiteral("isOpen"), folder->expanded() ? QStringLiteral("true") : QStringLiteral("false"));
        outline.setAttribute(QStringLiteral("description"), folder->description());
        parent.appendChild(outline);
    } else {
        outline = parent;
    }
    for (int i = 0; i < folder->childCount(); ++i) {
        const MenuItem *child = folder->childAt(i);

        switch (child->itemType()) {
        case MenuItem::FolderMenuItem:
            exportOpmlOutline(reinterpret_cast<const FeedFolder *>(child), document, outline);
            break;
        case MenuItem::FeedMenuItem:
            exportOpmlOutline(reinterpret_cast<const Feed *>(child), document, outline);
            break;
        default:
            break;
        }
    }
}

void exportAsOpmlDocument(const FeedFolder *folder, QDomDocument &document)
{
    QDomElement opml = document.createElement(QStringLiteral("opml"));
    QDomElement head = document.createElement(QStringLiteral("head"));
    QDomElement body = document.createElement(QStringLiteral("body"));

    opml.setAttribute(QStringLiteral("version"), QStringLiteral("1.0"));
    opml.appendChild(head);
    opml.appendChild(body);
    exportOpmlOutline(folder, document, body);
    document.appendChild(opml);
}
