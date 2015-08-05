#include "LeafProtocolHandler.h"
#include "handle/ChatHandle.h"

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

LeafProtocolHandler::LeafProtocolHandler(LeafNetHandler *nh)
        :ProtocolHandler(nh)
{
        
}

void LeafProtocolHandler::init()
{
        ChatHandle::createInstance(this);
}