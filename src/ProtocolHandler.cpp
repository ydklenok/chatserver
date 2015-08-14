#include "ProtocolHandler.h"

ProtocolHandler::ProtocolHandler(NetThreader *nt)
        :nt_(nt)
{
        logger_ = log4cxx::Logger::getLogger("ProtocolHandler");
}

ProtocolHandler::~ProtocolHandler()
{
        hash_map<string, set<BufferEvent*>*>::iterator it = gid_bevs_map_.begin();
        for(; it != gid_bevs_map_.end(); it++)
        {
                set<BufferEvent*> *bevs = it->second;
                set<BufferEvent*>::iterator bevit = bevs->begin();
                for(; bevit != bevs->end(); bevit++)
                {
                        nt_->freeBufferEvent(*bevit);
                }
                bevs->clear();
                delete it->second;
        }
        gid_bevs_map_.clear();
        gid_pids_map_.clear();
        pid_bev_map_.clear();
}

void ProtocolHandler::peerLogin(const string gid, const string pid, BufferEvent *bev)
{
        if(gid == "" || pid == "" || !bev)
        {
                LOG4CXX_ERROR(logger_, "Invalid param gid=" << gid << ",pid="<< pid << ",bev=null");
                return ;
        }

        /* add to pids map. */
        pid_bev_map_[pid] = bev;

        /* add to gids map. */
        set<BufferEvent*> *bevs;
        hash_map<string, set<BufferEvent*>*>::iterator it = gid_bevs_map_.find(gid);
        if(it == gid_bevs_map_.end())
        {
                bevs = new set<BufferEvent*>;
                gid_bevs_map_[gid] = bevs;
        }
        else
        {
                bevs = it->second;
        }
        bevs->insert(bev);
}

void ProtocolHandler::peerLogout(string &pid)
{
        if(pid == "")
        {
                LOG4CXX_ERROR(logger_, "pid is null.");
                return ;
        }

        /* delete 'peer id' in the pid-bev map and gid-bevs map*/
        BufferEvent *bev;
        hash_map<string, BufferEvent*>::iterator it = pid_bev_map_.find(pid);
        if(it != pid_bev_map_.end())
        {
                bev = it->second;
                pid_bev_map_.erase(it);
        }

        removeBufferEventFromGidMap(bev);

        nt_->freeBufferEvent(bev);
}

void ProtocolHandler::peerLogout(BufferEvent *bev)
{
        if(!bev)
        {
                LOG4CXX_ERROR(logger_, "bev is null.");
                return ;
        }

        hash_map<string, BufferEvent*>::iterator it = pid_bev_map_.begin();
        for(; it != pid_bev_map_.end(); it++)
        {
                BufferEvent *bev0 = it->second;
                if(bev == bev0)
                {
                        pid_bev_map_.erase(it);
                        break;
                }
        }

        removeBufferEventFromGidMap(bev);

        nt_->freeBufferEvent(bev);
}

void ProtocolHandler::removeBufferEventFromGidMap(BufferEvent *bev)
{
        bool brk = false;
        hash_map<string, set<BufferEvent*>*>::iterator bfit = gid_bevs_map_.begin();
        for(; bfit != gid_bevs_map_.end(); bfit++)
        {
                string gid = bfit->first;
                set<BufferEvent*> *bf = bfit->second;
                set<BufferEvent*>::iterator setit = bf->begin();
                for (; setit != bf->end(); setit++)
                {
                        if(*setit == bev)
                        {
                                bf->erase(setit);
                                if(bf->size() == 0)
                                {
                                        gid_bevs_map_.erase(gid);
                                        delete bf;
                                }
                                brk = true;
                        }
                        if(brk) break;
                }
                if(brk) break;
        }
}

bool ProtocolHandler::sendToPeer(string &pid, const void *data, size_t size)
{
        hash_map<string, BufferEvent*>::iterator it = pid_bev_map_.find(pid);
        if(it != pid_bev_map_.end())
        {
                BufferEvent *bev = it->second;
                return true;//nh_->write(bev, data, size);
        }
        else
        {
                LOG4CXX_ERROR(logger_, "can not find fd by pid[" << pid << "]");
                return false;
        }
}

bool ProtocolHandler::sendToPeer(BufferEvent *bev, const void *data, size_t size)
{
        return true;//nh_->write(bev, data, size);
}

bool ProtocolHandler::sendToGroup(string &gid, const void *data, size_t size)
{
        hash_map<string, set<BufferEvent*>*>::iterator it = gid_bevs_map_.find(gid);
        if(it != gid_bevs_map_.end())
        {
                set<BufferEvent*> *bevs = it->second;
                set<BufferEvent*>::iterator bevit = bevs->begin();
                for(; bevit != bevs->end(); bevit++)
                {
                        BufferEvent* bev = *bevit;
                        bool suc = true;//nh_->write(bev, data, size);
                        if(!suc)
                        {
                                peerLogout(bev);
                                LOG4CXX_ERROR(logger_, "send data error at gid=" << gid);
                        }
                }
        }

        return true;
}

void ProtocolHandler::registe(Command cmd, ProcessRoutine process)
{
        processRoutine_[cmd] = process;
}

void ProtocolHandler::dispatch(BufferEvent *bev, string &record)
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
                        LOG4CXX_ERROR(logger_, "record size less than cmd-length.");
                        return ;
                }
                cmd = ntohs(*((unsigned short*)record.c_str()));
                string content = record.substr(sizeof(unsigned short));

                hash_map<unsigned short, ProcessRoutine>::iterator it = processRoutine_.find(cmd);
                if(it != processRoutine_.end())
                {
                        ProcessRoutine process = it->second;
                        process(bev, content);
                }
                else
                {
                        LOG4CXX_ERROR(logger_, "Can not find process routine at Command[" << cmd << "].");
                }
        }
}

void ProtocolHandler::processPolicy()
{

}

void ProtocolHandler::processState()
{

}