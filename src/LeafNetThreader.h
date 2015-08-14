#ifndef __LEAF_NET_THREADER_H_H__
#define __LEAF_NET_THREADER_H_H__

#include "ProtocolHandler.h"
#include "NetThreader.h"

class LeafNetThreader : public NetThreader
{
public:
        LeafNetThreader(socket_t fd[2]);
        ~LeafNetThreader();
};

#endif