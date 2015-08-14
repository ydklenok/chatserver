#ifndef __NET_HANDLER_H_H__
#define __NET_HANDLER_H_H__

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
#include <log4cxx/logger.h>

#include "ProtocolHandler.h"
#include "StructUtil.h"
#include "NetThreader.h"

using namespace std;
class ProtocolHandler;
class NetThreader;

class NetHandler
{
public:
        NetHandler(int port, int thread_num = 8);
        virtual ~NetHandler();

        void    start();
        void    onAccept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen);
        void    onErrorAccept(struct evconnlistener *listener);
        virtual NetThreader* newNetThreader(socket_t fd[2]) = 0;

private:
        int     listenAndLoop();
        char*   getPeerIP(int fd);
        char*   getPeerIP(struct sockaddr *address);

        struct event_base       *base_;
        int                     thread_num_;
        vector<NetThreader*>    threads_;
        int                     last_thread_;
        int                     port_;
        vector<pthread_t>       pids_;
        log4cxx::LoggerPtr      logger_;
};

#endif
