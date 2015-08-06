#include "NetHandler.h"

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
static void accept_error_cb(struct evconnlistener *listener, void *ctx);

static void conn_read_cb(BufferEvent *bev, void *ctx);
static void conn_event_cb(BufferEvent *bev, short events, void *ctx);

NetHandler::NetHandler()
{
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        base_ = event_base_new();
        if (!base_)
        {
                cout << "Couldn't open event base." << endl;
                exit(-1);
        }
}

NetHandler::~NetHandler()
{
        event_base_loopexit(base_, NULL);
}

void NetHandler::loop()
{
        if(ph_ == NULL)
        {
                cout << "ProtocolHandler can not NULL." << endl;
                exit(1);
        }
        event_base_dispatch(base_);
}

int NetHandler::createConnection(string host, int port)
{
        BufferEvent *bev;
        char port_str[64] = {0};
        struct addrinfo hints, *res, *ressave;

        if(port <= 0 || port > 65535)
        {
                cout << "Invalid port in connection." << endl;
                return -1;
        }

        bev = bufferevent_socket_new(base_, -1, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, conn_read_cb, NULL, conn_event_cb, NULL);
        bufferevent_enable(bev, EV_READ);

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        sprintf(port_str, "%d", port);
        if( getaddrinfo(host.c_str(), port_str, &hints, &res) != 0)
        {
                cout << "getaddrinfo error in connecting." << endl;
                return -1;
        }
        ressave = res;
        do
        {
                if(bufferevent_socket_connect(bev, res->ai_addr, res->ai_addrlen) == 0)
                {
                        break;
                }
        }while( (res = res->ai_next) != NULL );

        if(res == NULL)
        {
                cout << "get addrinfo hints error." << endl;
                return -1;
        }
        freeaddrinfo(ressave);

        return 0;
}

int NetHandler::createListen(int port)
{
        struct evconnlistener *listener;
        struct sockaddr_in sin;

        if(port <= 0 || port > 65535)
        {
                cout << "Invalid port in listen." << endl;
                return -1;
        }

        /* Clear the sockaddr before using it, in case there are extra
        * platform-specific fields that can mess us up. */
        memset(&sin, 0, sizeof(sin));
        /* This is an INET address */
        sin.sin_family = AF_INET;
        /* Listen on 0.0.0.0 */
        sin.sin_addr.s_addr = htonl(0);
        /* Listen on the given port. */
        sin.sin_port = htons(port);

        listener = evconnlistener_new_bind(base_,
                accept_conn_cb, 
                this, 
                LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
                -1,
                (struct sockaddr*)&sin,
                sizeof(sin));

        if (!listener) 
        {
                perror("Couldn't create listener.");
                return -1;
        }
        evconnlistener_set_error_cb(listener, accept_error_cb);

        return 0;
}

bool NetHandler::write(BufferEvent *bev, const void *data, size_t size)
{
        if(!bev)
        {
                return false;
        }

        int ret = bufferevent_write(bev, data, size);
        if(ret == 0)
        {
                return true;
        }
        else
        {
                cout << "write error at bev=[" << bev << "]" << endl;
                ph_->peerLogout(bev);
                return false;
        }
}

void NetHandler::onAccept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen)
{
        /* We got a new connection! Set up a bufferevent for it. */
        struct event_base *base = evconnlistener_get_base(listener);
        BufferEvent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

        bufferevent_setcb(bev, conn_read_cb, NULL, conn_event_cb, this);
        bufferevent_enable(bev, EV_READ);
}
void NetHandler::onErrorAccept(struct evconnlistener *listener)
{
        struct event_base *base = evconnlistener_get_base(listener);
        int err = EVUTIL_SOCKET_ERROR();
        cout << "Got an error " << err << " (" << evutil_socket_error_to_string(err) << ")" << "on the listener. Shutting down.\n" << endl;
        event_base_loopexit(base, NULL);
        exit(-1);
}

void NetHandler::onEvent(BufferEvent *bev, short events)
{
        if (events & BEV_EVENT_ERROR)
        {
                perror("Error from bufferevent");
        }

        if(events & BEV_EVENT_EOF)
        {
                cout << "event eof." << endl;
        }

        if(events & BEV_EVENT_CONNECTED)
        {
                
        }

        if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
        {
                ph_->peerLogout(bev);
        }
}

void NetHandler::onRead(BufferEvent *bev)
{
        struct evbuffer *input = bufferevent_get_input(bev);

        string record;
        int ret = -1;
        while((ret = parserRecord(input, record)) == 1)
        {
                if(ret == -1)
                {
                        cout << "parser record error." << endl;
                        if(ph_)
                        {
                                ph_->peerLogout(bev);
                        }
                        else
                        {
                                cout << "ProtocolHandler is null." << endl;
                        }
                }
                else if(ret == 0)
                {
                        break;
                }
                else
                {
                        if(ph_)
                        {
                                ph_->dispatch(bev, record);
                        }
                        else
                        {
                                cout << "ProtocolHandler is null." << endl;
                        }
                }
        }
}

int NetHandler::parserRecord(struct evbuffer *buf, string &record)
{
        /* The records contain a 4-byte size field in network order, followed by that
            number of bytes.  We will return 1 and set the 'out' fields if we
            have a whole record, return 0 if the record isn't here yet, and
            -1 on error. */
        size_t buffer_len = evbuffer_get_length(buf);

        char header[RECORD_HEADER_LEN];

        if(buffer_len < RECORD_HEADER_LEN)
                return 0; /* The size field hasn't arrived. */

        /* We use evbuffer_copyout here so that the size field will stay on the buffer for now*/
        evbuffer_copyout(buf, header, RECORD_HEADER_LEN);

        /* check <policy-file-request/>*/
        if(header[0] == '<' && header[1] == 'p'
                && header[2] == 'o' && header[3] == 'l')
        {
                if(buffer_len < 22)
                        return 0;
                if(strncmp(header, "<policy-file-request/>", 22) == 0)
                {
                        record = string(header, 22);
                        return 1;
                }
        }

        ev_uint32_t record_len = *((ev_uint32_t*)header);

        /* Convert len_buf into host order */
        record_len = ntohl(record_len);

        if(record_len > LEAF_READ_SIZE_MAX)
        {
                cout << "record length > " << LEAF_READ_SIZE_MAX << endl;
                return -1; /* record length too large. */
        }

        if(buffer_len < record_len + RECORD_HEADER_LEN)
                return 0; /* The record hasn't arrived. */

        /* Okay, _now_ we can remove the record. */
        evbuffer_drain(buf, RECORD_HEADER_LEN);

        char rbuf[LEAF_READ_SIZE_MAX] = {0};
        evbuffer_remove(buf, &rbuf, record_len);
        record = string(rbuf, record_len);

        return 1;
}

void NetHandler::freeBufferEvent(BufferEvent *bev)
{
        bufferevent_free(bev);
}

static void conn_read_cb(BufferEvent *bev, void *ctx)
{
        /* This callback is invoked when there is data to read on bev. */
        NetHandler *nh = (NetHandler*)ctx;
        if(nh)
        {
                nh->onRead(bev);
        }
}

static void conn_event_cb(BufferEvent *bev, short events, void *ctx)
{
        NetHandler *nh = (NetHandler*)ctx;
        if(nh)
        {
                nh->onEvent(bev, events);
        }
}

static void accept_conn_cb(struct evconnlistener *listener,
        evutil_socket_t fd, struct sockaddr *address, int socklen,
        void *ctx)
{
        NetHandler *nh = (NetHandler *)ctx;
        if(nh)
        {
                nh->onAccept(listener, fd, address, socklen);
        }
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
        NetHandler *nh = (NetHandler *)ctx;
        if(nh)
        {
                nh->onErrorAccept(listener);
        }
}