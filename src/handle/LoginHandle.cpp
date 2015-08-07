#include "LoginHandle.h"
#include "../proto/C2SLogin.pb.h"

void LoginHandle::handle(BufferEvent* bev, string &content)
{
        C2SLogin c2slogin;
        bool suc = false;
        suc = c2slogin.ParseFromString(content);
        if(suc)
        {
                cout << c2slogin.DebugString() << endl;
                ph_->peerLogin(c2slogin.gid(), c2slogin.uid(), bev);
        }
}