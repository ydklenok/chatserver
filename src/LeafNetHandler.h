#ifndef __LEAF_NET_HANDLER_H_H__
#define __LEAF_NET_HANDLER_H_H__

#include "NetHandler.h"

class LeafNetHandler : public NetHandler
{
public:
        LeafNetHandler(size_t rsize);
        ~LeafNetHandler();

        virtual void init();
private:

};

#endif