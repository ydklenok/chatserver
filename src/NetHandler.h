#ifndef __NET_HANDLER_H_H__
#define __NET_HANDLER_H_H__

#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <signal.h>
#endif

#include <cstring>

#include <event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

#include "ProtocolHandler.h"
#include "StructUtil.h"

using namespace std;
class ProtocolHandler;

class NetHandler
{
public:
        NetHandler(size_t rsize);
        ~NetHandler();

        void    protocol(ProtocolHandler *ph){ph_ = ph;};
        void    loop();
        bool    write(int fd, const void *data, size_t size);

        void    onEvent(struct bufferevent *bev, short events);
        void    onRead(struct bufferevent *bev);
        void    onAccept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen);
        void    onErrorAccept(struct evconnlistener *listener);
        void    removeConnection(int fd);

        virtual void init() = 0;

protected:
        int     createConnection(string host, int port);
        int     createListen(int port);
        int     parserRecord(struct evbuffer *buf, string &record);
        void    removeConnection(struct bufferevent *bev);

private:
        struct event_base       *base_;
        ProtocolHandler         *ph_;
        char                    *rbuf_;

        const static size_t     RECORD_HEADER_LEN = 4;
        size_t                  rsize_;

        hash_map<int, struct bufferevent*>      bevMap_;
};

#endif
