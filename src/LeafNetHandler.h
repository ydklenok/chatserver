#ifndef __LEAF_NET_HANDLER_H_H__
#define __LEAF_NET_HANDLER_H_H__

#include "NetHandler.h"
#include "LeafNetThreader.h"
#include "LeafProtocolHandler.h"

class LeafNetHandler : public NetHandler
{
public:
        LeafNetHandler(int port, int thread_num);
        ~LeafNetHandler();

        virtual NetThreader* newNetThreader(socket_t fd[2]);
private:

};

#endif