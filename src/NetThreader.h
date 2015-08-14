#ifndef __NET_THREADER_H_H__
#define __NET_THREADER_H_H__

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <signal.h>
#endif

#include <cstring>
#include <log4cxx/logger.h>

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <pthread.h>

#include "ProtocolHandler.h"
#include "StructUtil.h"

void *g_net_threader_worker_livevent(void *ctx);

class NetHandler;
class ProtocolHandler;

using namespace std;

class NetThreader{
public:
        NetThreader(socket_t fd[2]);
        ~NetThreader();

        bool    write(int fd, const void *data, size_t size);
        bool    write(BufferEvent *bev, const void *data, size_t size);
        void    onEvent(BufferEvent *bev, short events);
        void    onRead(BufferEvent *bev);
        void    onReadParent(BufferEvent *bev);
        void    onEventParent(BufferEvent *bev, short events);
        void    freeBufferEvent(BufferEvent *bev);
        void    loop();
        void    protocol(ProtocolHandler *ph){ph_ = ph;};
        void    addFd(int fd);
        int     writeFd(){return write_fd_;};

        int     thread_id;

private:
        int     parserRecord(struct evbuffer *buf, string &record);

        ProtocolHandler                 *ph_;
        hash_map<BufferEvent*, string>  bufferRecordMap_;
        const static size_t             RECORD_HEADER_LEN = 4;

public:
        pthread_t                       tid;

private:
        struct event_base               *base_;
        BufferEvent                     *bev_;
        int                             read_fd_;
        int                             write_fd_;
        list<int>                       fdList_;
        log4cxx::LoggerPtr              logger_;
};

#endif