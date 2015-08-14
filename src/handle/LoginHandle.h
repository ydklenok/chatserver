#ifndef __LOGIN_HANDLE_H_H__
#define __LOGIN_HANDLE_H_H__

#include "BaseHandle.h"
#include <log4cxx/logger.h>

class LoginHandle : public BaseHandle
{
public:
        LoginHandle();
        ~LoginHandle(){};

        RegistEvent(LoginHandle, C2S_Login);

protected:
        void handle(BufferEvent *bev, string &content);

private:
        log4cxx::LoggerPtr logger_;
};

#endif
