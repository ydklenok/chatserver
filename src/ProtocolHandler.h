#ifndef __PROTOCOL_HANDLER_H_H__
#define __PROTOCOL_HANDLER_H_H__

#include <string>
#include <set>
#include <log4cxx/logger.h>
#include <event2/bufferevent.h>

#include "ConstDefine.h"
#include "Command.h"
#include "StructUtil.h"
#include "NetThreader.h"
#include <algorithm>

using namespace std;

class NetThreader;

class ProtocolHandler
{
public:
        ProtocolHandler(NetThreader *nt);
        virtual ~ProtocolHandler();

        void loop();
        void registe(Command cmd, ProcessRoutine process);
        void dispatch(BufferEvent *bev, string &record);

        void peerLogin(const string gid, const string pid, BufferEvent *bev);
        void peerLogout(string &pid);
        void peerLogout(BufferEvent *bev);
        bool sendToPeer(string &pid, const void *data, size_t size);
        bool sendToPeer(BufferEvent *bev, const void *data, size_t size);
        bool sendToGroup(string &gid, const void *data, size_t size);

protected:
        virtual void init() = 0;
        hash_map<unsigned short, ProcessRoutine>       processRoutine_;

        /* peer id, BufferEvent*/
        hash_map<string, BufferEvent*>          pid_bev_map_;
        /* group id, peer ids*/
        hash_map<string, string>                gid_pids_map_;
        /* group id, BufferEvent set.*/
        hash_map<string, set<BufferEvent*>*>    gid_bevs_map_;

        NetThreader      *nt_;

private:
        void processPolicy();
        void processState();
        void removeBufferEventFromGidMap(BufferEvent *bev);

        log4cxx::LoggerPtr logger_;
};

#endif
