#include "LoginHandle.h"
#include "../proto/C2SLogin.pb.h"

LoginHandle::LoginHandle()
{
        logger_ = log4cxx::Logger::getLogger("LoginHandle");
}

void LoginHandle::handle(BufferEvent* bev, string &content)
{
        C2SLogin c2slogin;
        bool suc = false;
        suc = c2slogin.ParseFromString(content);
        if(suc)
        {
                LOG4CXX_INFO(logger_, c2slogin.DebugString());
                ph_->peerLogin(c2slogin.gid(), c2slogin.uid(), bev);
        }
}