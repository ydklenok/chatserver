#include "LeafProtocolHandler.h"
#include "handle/ChatHandle.h"
#include "handle/LoginHandle.h"

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
        LoginHandle::createInstance(this);
        ChatHandle::createInstance(this);
}