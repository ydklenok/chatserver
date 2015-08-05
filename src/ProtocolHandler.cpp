#include "ProtocolHandler.h"

ProtocolHandler::ProtocolHandler(NetHandler *nh)
        :nh_(nh)
{
        
}

ProtocolHandler::~ProtocolHandler()
{
        hash_map<string, set<string>*>::iterator it = gid_pids_map_.begin();
        for(; it != gid_pids_map_.end(); it++)
        {
                delete it->second;
        }
        gid_pids_map_.clear();
}

void ProtocolHandler::loop()
{
        this->init();
        nh_->protocol(this);
        nh_->init();
        nh_->loop();
}

void ProtocolHandler::peerLogin(string &gid, string &pid, int fd)
{
        if(gid == "" || pid == "" || fd <= 0)
        {
                cout << "Invalid param gid=" << gid << ",pid="<< pid << ",fd=" << fd  << endl;
                return ;
        }

        /* add to pids map. */
        pid_fd_map_[pid] = fd;

        /* add to gids map. */
        set<string> *pids;
        hash_map<string, set<string>*>::iterator it = gid_pids_map_.find(gid);
        if(it == gid_pids_map_.end())
        {
                pids = new set<string>;
        }
        else
        {
                pids = it->second;
        }
        pids->insert(pid);
}

void ProtocolHandler::peerLogout(string &pid)
{
        if(pid == "")
        {
                cout << "pid is null." << endl;
                return ;
        }

        /* delete 'peer id' in the pid-fd map*/
        hash_map<string, int>::iterator it = pid_fd_map_.find(pid);
        if(it != pid_fd_map_.end())
        {
                pid_fd_map_.erase(it);
        }

        removePidFromGroup(pid);
}

void ProtocolHandler::peerLogout(int fd)
{
        hash_map<string, int>::iterator it = pid_fd_map_.begin();
        for(; it != pid_fd_map_.end(); it++)
        {
                int cfd = it->second;
                if(cfd == fd)
                {
                        string pid = it->first;
                        removePidFromGroup(pid);
                        pid_fd_map_.erase(it);
                        break;
                }
        }
}

void ProtocolHandler::removePidFromGroup(string &pid)
{
        /* delete peer id in the gid-pids map*/
        hash_map<string, set<string>*>::iterator itgid = gid_pids_map_.begin();
        for(; itgid != gid_pids_map_.end(); itgid++)
        {
                set<string> *uids = itgid->second;
                uids->erase(pid);
                if(uids->size() == 0)
                {
                        delete uids;
                }
        }
}

bool ProtocolHandler::sendToPeer(string &pid, const void *data, size_t size)
{
        hash_map<string, int>::iterator it = pid_fd_map_.find(pid);
        if(it != pid_fd_map_.end())
        {
                int fd = it->second;
                return nh_->write(fd, data, size);
        }
        else
        {
                cout << "can not find fd by pid[" << pid << "]" << endl;
                return false;
        }
}

bool ProtocolHandler::sendToGroup(string &gid, const void *data, size_t size)
{
        hash_map<string, set<string>*>::iterator it = gid_pids_map_.find(gid);
        if(it != gid_pids_map_.end())
        {
                set<string> *pids = it->second;
                set<string>::iterator pidit = pids->begin();
                for(; pidit != pids->end(); pidit++)
                {
                        string pid = *pidit;
                        bool suc = sendToPeer(pid, data, size);
                        if(!suc)
                        {
                                cout << "send data to " << pid << "error." << endl;
                        }
                }
        }

        return true;
}

void ProtocolHandler::registe(Command cmd, ProcessRoutine process)
{
        processRoutine_[cmd] = process;
}

void ProtocolHandler::dispatch(string &record)
{
        if (record.substr(0, 22) == "<policy-file-request/>")
        {
                processPolicy();
        }
        else if (record.substr(0, 22) == "<policy-stat-request/>")
        {
                processState();
        }
        else
        {
                unsigned short cmd = 0;
                if(record.size() < sizeof(unsigned short))
                {
                        cout << "record size less than cmd-length." << endl;
                        return ;
                }
                cmd = ntohs(*((unsigned short*)record.c_str()));
                string content = record.substr(sizeof(unsigned short));

                hash_map<unsigned short, ProcessRoutine>::iterator it = processRoutine_.find(cmd);
                if(it != processRoutine_.end())
                {
                        ProcessRoutine process = it->second;
                        process(content);
                }
                else
                {
                        cout << "Can not find process routine at " << cmd << endl;
                }
        }
}

void ProtocolHandler::processPolicy()
{

}

void ProtocolHandler::processState()
{

}