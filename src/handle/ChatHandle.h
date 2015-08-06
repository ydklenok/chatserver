#ifndef __CHAT_HANDLE_H_H__
#define __CHAT_HANDLE_H_H__

#include "BaseHandle.h"

class ChatHandle : public BaseHandle
{
public:
        ChatHandle() {};
        ~ChatHandle(){};

        RegistEvent(ChatHandle, C2S_Chat);

protected:
        void handle(BufferEvent *bev, string &content);
private:

};

#endif
