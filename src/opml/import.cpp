#include "../feed.h"
#include "../feedfolder.h"
#include <QDomDocument>

static void importOpmlOutlineIn(FeedFolder *folder, QDomElement root)
{
    const QString itemTag = QStringLiteral("outline");
    const QString xmlUrlAttribute = QStringLiteral("xmlUrl");
    const QString linkAttribute = QStringLiteral("link");
    const QString nameAttribute = QStringLiteral("text");
    const QString descriptionAttribute = QStringLiteral("comment");
    const QString idAttribute = QStringLiteral("id");
    const QString isOpen = QStringLiteral("isOpen");

    for (QDomElement outline = root.firstChildElement(itemTag); !outline.isNull(); outline = outline.nextSiblingElement(itemTag)) {
        MenuItem *item = nullptr;

        if (outline.hasAttribute(xmlUrlAttribute)) {
            Feed *feed = new Feed(folder);

            item = feed;
            if (outline.hasAttribute(idAttribute) && !outline.attribute(idAttribute).isEmpty())
                feed->setUuid(outline.attribute(idAttribute));
            feed->setXmlUrl(QUrl(outline.attribute(xmlUrlAttribute)));
            feed->setLink(QUrl(outline.attribute(linkAttribute)));
        } else {
            FeedFolder *subfolder = new FeedFolder(folder);

            subfolder->setExpanded(outline.attribute(isOpen) == QStringLiteral("true"));
            item = subfolder;
            importOpmlOutlineIn(subfolder, outline);
        }
        item->setName(outline.attribute(nameAttribute));
        item->setDescription(outline.attribute(descriptionAttribute));
        folder->addItem(item);
    }
}

void importOpmlDocumentIn(FeedFolder *folder, QDomDocument &document)
{
    QDomElement root = document.firstChildElement(QStringLiteral("opml")).firstChildElement(QStringLiteral("body"));

    importOpmlOutlineIn(folder, root);
}
