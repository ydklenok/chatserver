#ifndef __PROTOCOL_HANDLER_H_H__
#define __PROTOCOL_HANDLER_H_H__

#include <string>
#include <set>
#include "ConstDefine.h"
#include "Command.h"
#include "StructUtil.h"
#include "NetHandler.h"
#include <algorithm>

using namespace std;
class NetHandler;

class ProtocolHandler
{
public:
        ProtocolHandler(NetHandler *nh);
        virtual ~ProtocolHandler();

        void loop();
        void registe(Command cmd, ProcessRoutine process);
        void dispatch(string &record);

        void peerLogin(string &gid, string &uid, int fd);
        void peerLogout(string &pid);
        void peerLogout(int fd);
        bool sendToPeer(string &pid, const void *data, size_t size);
        bool sendToGroup(string &gid, const void *data, size_t size);

protected:
        void removePidFromGroup(string &pid);
        virtual void init() = 0;
        void processPolicy();
        void processState();
        hash_map<unsigned short, ProcessRoutine>       processRoutine_;

        /* peer id, file descriptor*/
        hash_map<string, int>                   pid_fd_map_;
        /* group id, peer ids*/
        hash_map<string, set<string>*>          gid_pids_map_;
                
        NetHandler      *nh_;

private:
};

#endif
