#ifndef __BASE_HANDLE_H_H__
#define __BASE_HANDLE_H_H__

#include <iostream>

#include "../ProtocolHandler.h"
#include "../Command.h"

#define RegistEvent(className, cmd) \
static void createInstance(ProtocolHandler* ph) \
{ \
        className* instance_ = getInstance(); \
        instance_->ph_ = ph; \
        ph->registe(cmd,((ProcessRoutine)className::handle_));\
}\
static className* getInstance()\
{\
        static className ins;\
        return &ins;\
}\
static void handle_(string &content)\
{\
        className::getInstance()->handle(content);\
}

using namespace std;

class BaseHandle
{
public:
        BaseHandle(){};
        virtual ~BaseHandle() {};

protected:
        ProtocolHandler *ph_;
        static BaseHandle* instance_;
private:
        int cmd_;
};

#endif
