#include "ChatHandle.h"
#include "../proto/C2SChat.pb.h"

void ChatHandle::handle(BufferEvent* bev, string &content)
{
        C2SChat c2schat;
        bool suc = false;
        suc = c2schat.ParseFromString(content);
        if(suc)
        {
                cout << c2schat.DebugString() << endl;
        }
}