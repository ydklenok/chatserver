#ifndef __CLIENT_PROTOCOL_HANDLER_H_H__
#define __CLIENT_PROTOCOL_HANDLER_H_H__

#include <iostream>
#include <string>

#include "ConstDefine.h"
#include "ProtocolHandler.h"
#include "LeafNetHandler.h"

using namespace std;

class LeafProtocolHandler : public ProtocolHandler
{
public:
        LeafProtocolHandler(LeafNetHandler *nh);
        ~LeafProtocolHandler(){};

        void init();
        void dispatch(string &record);
private:
};

#endif
