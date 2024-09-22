#pragma once
#include "cloudprovider.h"

class Feed;
class FeedFolder;

class SyncService : public QObject
{
    Q_OBJECT
public:
    SyncService(CloudProvider &parent);

    void synchronize(FeedFolder &);
    void synchronize(Feed &);

private:
    CloudProvider &m_cloudProvider;
};
