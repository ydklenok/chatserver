#include "ChatHandle.h"
#include "../proto/C2SChat.pb.h"

ChatHandle::ChatHandle()
{
        logger_ = log4cxx::Logger::getLogger("ChatHandle");
}

void ChatHandle::handle(BufferEvent* bev, string &content)
{
        C2SChat c2schat;
        bool suc = false;
        suc = c2schat.ParseFromString(content);
        if(suc)
        {
                LOG4CXX_INFO(logger_, c2schat.DebugString());
        }
}