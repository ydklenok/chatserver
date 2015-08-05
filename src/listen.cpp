#include <map>
#include <iostream>
#include <set>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <signal.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "proto/Chat.pb.h"
using namespace std;

#define RECORD_HEADER_LEN sizeof(int)


size_t g_read_buf_max = 4096;

class ConnUsr
{
public:
        ConnUsr();
        ~ConnUsr();
private:
        struct bufferevent *bev_;
        char *rbuf;
};

int gid = 1001;
typedef set<struct bufferevent*>        BufSet;
typedef map<int, BufSet*>               GroupMap;

GroupMap                                groupMap;

static void assembleBuf(struct bufferevent *bev, string &content)
{
        if(content.empty())
                return ;

        int header = content.length();
        int hostHeader = ntohl(header);

        int ret = bufferevent_write(bev, (const void*)&hostHeader, RECORD_HEADER_LEN);
        if(ret != 0)
                return ;
        ret = bufferevent_write(bev, content.c_str(), header);
        if(ret != 0)
                return ;
}

static void broadcaseToGroup(string &record)
{
        map<int, BufSet*>::iterator it = groupMap.find(gid);
        if(it != groupMap.end())
        {
                BufSet *bufSet = it->second;
                set<struct bufferevent*>::iterator itor = bufSet->begin();
                for(; itor != bufSet->end(); itor++)
                {
                        struct bufferevent *bev = *itor;
                        struct evbuffer *out = bufferevent_get_output(bev);
                        assembleBuf(bev, record);
                }
        }
}

static int malloc_record(struct evbuffer *buf, string &record)
{
        /* The records contain a 4-byte size field in network order, followed by that
            number of bytes.  We will return 1 and set the 'out' fields if we
            have a whole record, return 0 if the record isn't here yet, and
            -1 on error. */
        size_t buffer_len = evbuffer_get_length(buf);
        ev_uint32_t record_len;

        if(buffer_len < RECORD_HEADER_LEN)
                return 0; /* The size field hasn't arrived. */
        
        /* We use evbuffer_copyout here so that the size field will stay on the buffer for now*/
        evbuffer_copyout(buf, &record_len, RECORD_HEADER_LEN);
        /* Convert len_buf into host order */
        record_len = ntohl(record_len);

        if(record_len > g_read_buf_max)
                return -1; /* record length too large. */

        if(buffer_len < record_len + RECORD_HEADER_LEN)
                return 0; /* The record hasn't arrived. */

        /* Okay, _now_ we can remove the record. */
        evbuffer_drain(buf, RECORD_HEADER_LEN);
        evbuffer_remove(buf, g_read_buf, record_len);

        record = string(g_read_buf, buffer_len);

        return 1;
}

static void echo_read_cb(struct bufferevent *bev, void *ctx)
{
        /* This callback is invoked when there is data to read on bev. */
        struct evbuffer *input = bufferevent_get_input(bev);

        string record;
        int ret = malloc_record(input, record);
        if(ret == 1)
        {
                Chat chat;
                bool suc = chat.ParseFromString(record);
                cout << "recv ctx[" << chat.ctx() << "] from [" << chat.uid() << "]." << endl;
                broadcaseToGroup(record);
        }
        else if(ret == 0)
        {
                printf("waiting to receive data.\n");
        }
        else
        {
                perror("get record error:");
                bufferevent_free(bev);
        }
}

static void add_user_to_group(struct bufferevent *bev)
{
        BufSet *bufSet;
        map<int, BufSet*>::iterator it = groupMap.find(gid);
        if(it == groupMap.end())
        {
                bufSet = new BufSet;
                groupMap.insert(make_pair(gid, bufSet));
        }
        else
        {
                bufSet = it->second;
        }
        bufSet->insert(bev);
        printf("add a user fd = %d\n", bufferevent_getfd(bev));
}

static void del_user_from_group(struct bufferevent *bev)
{
        map<int, BufSet*>::iterator it = groupMap.begin();
        for(; it != groupMap.end(); it++)
        {
                BufSet *bufSet = it->second;
                bufSet->erase(bev);
                printf("del a user fd = %d\n", bufferevent_getfd(bev));
        }
}

static void echo_event_cb(struct bufferevent *bev, short events, void *ctx)
{
        if (events & BEV_EVENT_ERROR)
        {
                perror("Error from bufferevent");
        }

        if(events & BEV_EVENT_EOF)
        {
                printf("event eof.\n");
        }

        if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
        {
                del_user_from_group(bev);
                bufferevent_free(bev);
        }
}

static void accept_conn_cb(struct evconnlistener *listener,
        evutil_socket_t fd, struct sockaddr *address, int socklen,
        void *ctx)
{
        /* We got a new connection! Set up a bufferevent for it. */
        struct event_base *base = evconnlistener_get_base(listener);
        struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

        bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);
        bufferevent_enable(bev, EV_READ|EV_WRITE);

        add_user_to_group(bev);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
        struct event_base *base = evconnlistener_get_base(listener);
        int err = EVUTIL_SOCKET_ERROR();
        fprintf(stderr, "Got an error %d (%s) on the listener. Shutting down.\n", err, evutil_socket_error_to_string(err));

        event_base_loopexit(base, NULL);
}

#ifndef _WIN32
void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
        struct event_base *base = (event_base*)user_data;
        struct timeval delay = { 2, 0 };

        printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

        event_base_loopexit(base, &delay);
}
void signal_init(event_base *base)
{
        struct event *signal_event;
        signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);
}
#else
void signal_init(event_base *base)
{
        printf("signal_init is null at windows.\n");
}
#endif


int main(int argc, char *argv[])
{
        struct event_base *base;
        struct evconnlistener *listener;
        struct sockaddr_in sin;
        int port = 9876;

#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

        if (argc > 1) 
        {
                port = atoi(argv[1]);
        }
        if (port<=0 || port>65535) 
        {
                puts("Invalid port");
                return 1;
        }

        base = event_base_new();
        if (!base) 
        {
                puts("Couldn't open event base");
                return 1;
        }

        /* Clear the sockaddr before using it, in case there are extra
        *          * platform-specific fields that can mess us up. */
        memset(&sin, 0, sizeof(sin));
        /* This is an INET address */
        sin.sin_family = AF_INET;
        /* Listen on 0.0.0.0 */
        sin.sin_addr.s_addr = htonl(0);
        /* Listen on the given port. */
        sin.sin_port = htons(port);

        listener = evconnlistener_new_bind(base, 
                accept_conn_cb, 
                NULL, 
                LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, 
                -1, 
                (struct sockaddr*)&sin, 
                sizeof(sin));

        if (!listener) 
        {
                perror("Couldn't create listener");
                return 1;
        }
        evconnlistener_set_error_cb(listener, accept_error_cb);

        signal_init(base);

        event_base_dispatch(base);
        return 0;
}
