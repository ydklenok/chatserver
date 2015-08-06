#ifndef __LEAF_NET_HANDLER_H_H__
#define __LEAF_NET_HANDLER_H_H__

#include "NetHandler.h"

class LeafNetHandler : public NetHandler
{
public:
        LeafNetHandler();
        ~LeafNetHandler();

        virtual void init();
private:

};

#endif