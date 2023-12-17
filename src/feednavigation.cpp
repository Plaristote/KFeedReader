#include "feedarticle.h"
#include <QList>

FeedArticle *findPreviousArticle(const QList<FeedArticle *> &articles, const FeedArticle *article)
{
    for (auto it = articles.begin(); it != articles.end(); ++it) {
        if (*it == article) {
            ++it;
            if (it != articles.end())
                return *it;
            break;
        }
    }
    return nullptr;
}

FeedArticle *findNextArticle(const QList<FeedArticle *> &articles, const FeedArticle *article)
{
    auto previous = articles.begin();
    auto it = previous;

    if (previous != articles.end()) {
        ++it;
        while (it != articles.end()) {
            if (*it == article)
                return *previous;
            previous = it;
            ++it;
        }
    }
    return nullptr;
}
