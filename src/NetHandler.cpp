#include "NetHandler.h"

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
static void accept_error_cb(struct evconnlistener *listener, void *ctx);

NetHandler::NetHandler(int port, int thread_num)
        :port_(port), thread_num_(thread_num)
{
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        logger_ = log4cxx::Logger::getLogger("NetHandler");

        last_thread_ = 0;
        base_ = event_base_new();
        if (!base_)
        {
                LOG4CXX_FATAL(logger_, "Couldn't open event base.");
                exit(-1);
        }
}

NetHandler::~NetHandler()
{
        int i;
        for(i = 0; i < thread_num_; i++)
        {
                pthread_join(threads_[i]->tid, NULL);
        }

        for(i = 0; i < thread_num_; i++)
        {
                delete threads_[i];
        }

        event_base_loopexit(base_, NULL);
}

void NetHandler::start()
{
        int ret = -1;
        int i = 0;
        socket_t fd[2];

        for(i = 0; i < thread_num_; i++)
        {
                NetThreader *nh = newNetThreader(fd);
                if(!nh)
                {
                        LOG4CXX_FATAL(logger_, "create net threader error.");
                        exit(-1);
                }
                threads_.push_back(nh);
        }

        for (i = 0; i < thread_num_; i++) 
        {
                threads_[i]->thread_id = i;
                ret = pthread_create(&(threads_[i]->tid), NULL, g_net_threader_worker_livevent, threads_[i]);
                if(ret != 0)
                {
                        LOG4CXX_FATAL(logger_, "Can't create thread: " << strerror(ret));
                        exit(1);
                }
        }

        listenAndLoop();
}

int NetHandler::listenAndLoop()
{
        struct evconnlistener *listener;
        struct sockaddr_in sin;

        if(port_ <= 0 || port_ > 65535)
        {
                LOG4CXX_ERROR(logger_, "Invalid port in listen.");
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
        sin.sin_port = htons(port_);

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

        LOG4CXX_INFO(logger_, "listen at port [" << port_ << "] for " << thread_num_ << " threaders.");
        event_base_dispatch(base_);

        return 0;
}

void NetHandler::onAccept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen)
{
        /* We got a new connection! Set up a bufferevent for it. */
        char buf[1];
        int tid = (last_thread_ + 1) % thread_num_;
        NetThreader *thread = threads_[tid];

        LOG4CXX_DEBUG(logger_, "The peer connect from " << getPeerIP(address));

        last_thread_ = tid;
        thread->addFd(fd);
        buf[0] = 'c';
        int wfd = thread->writeFd();
        if(send(wfd, buf, 1, 0) != 1)
        {
                LOG4CXX_ERROR(logger_, "Writing to thread notify pipe error.");
        }
}
void NetHandler::onErrorAccept(struct evconnlistener *listener)
{
        struct event_base *base = evconnlistener_get_base(listener);
        int err = EVUTIL_SOCKET_ERROR();
        LOG4CXX_ERROR(logger_, "Got an error " << err << " (" << evutil_socket_error_to_string(err) << ")" << "on the listener. Shutting down.");
        event_base_loopexit(base, NULL);
        exit(-1);
}

char* NetHandler::getPeerIP(struct sockaddr *address)
{
        return inet_ntoa(((struct sockaddr_in*)address)->sin_addr);
}

char *NetHandler::getPeerIP(int fd)
{
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);

        if(getpeername(fd, (struct sockaddr*)&addr, &len) == 0)
        {
                return inet_ntoa(addr.sin_addr);
        }
        return NULL;
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