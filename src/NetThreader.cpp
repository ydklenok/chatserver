#include "NetThreader.h"

static void read_parent_cb(BufferEvent *bev, void *ctx);
static void event_parent_cb(BufferEvent *bev, short events, void *ctx);
static void net_threader_read_cb(BufferEvent *bev, void *ctx);
static void net_threader_event_cb(BufferEvent *bev, short events, void *ctx);
void *g_net_threader_worker_livevent(void *ctx);

NetThreader::NetThreader(socket_t fd[2])
{
        int ret;
        logger_ = log4cxx::Logger::getLogger("NetThreader");

        ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fd);
        if(ret == -1)
        {
                LOG4CXX_FATAL(logger_, "socketpair error.");
                exit(-1);
        }

        read_fd_ = fd[1];
        write_fd_ = fd[0];

        base_ = event_base_new();
        if(!base_)
        {
                LOG4CXX_FATAL(logger_, "event_base_new() error.");
                exit(-1);
        }

        bev_ = bufferevent_socket_new(base_, read_fd_, BEV_OPT_CLOSE_ON_FREE);
        if(!bev_)
        {
                LOG4CXX_FATAL(logger_, "bufferevent_socket_new() error.");
                exit(-1);
        }

        bufferevent_setcb(bev_, read_parent_cb, NULL, event_parent_cb, this);
        bufferevent_enable(bev_, EV_READ);
};

NetThreader::~NetThreader()
{
        bufferevent_free(bev_);
        event_base_free(base_);
}

void NetThreader::loop()
{
        LOG4CXX_INFO(logger_, "threader [" << thread_id << "] start");
        event_base_dispatch(base_);
}

bool NetThreader::write(BufferEvent *bev, const void *data, size_t size)
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
                LOG4CXX_ERROR(logger_, "write error at bev=[" << bev << "]");
                ph_->peerLogout(bev);
                return false;
        }
}

void NetThreader::onEventParent(BufferEvent *bev, short events)
{
        if (events & BEV_EVENT_ERROR)
        {
                LOG4CXX_ERROR(logger_, "Error from bufferevent: " << strerror(errno));
        }

        if(events & BEV_EVENT_EOF)
        {
                LOG4CXX_DEBUG(logger_, "The peer has performed an orderly shutdown.");
        }

        if(events & BEV_EVENT_CONNECTED)
        {
                LOG4CXX_INFO(logger_, "connected.");
        }

        if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
        {
                bufferevent_free(bev);
        }
}

void NetThreader::onEvent(BufferEvent *bev, short events)
{
        if (events & BEV_EVENT_ERROR)
        {
                LOG4CXX_ERROR(logger_, "Error from bufferevent: " << strerror(errno));
        }

        if(events & BEV_EVENT_EOF)
        {
                LOG4CXX_DEBUG(logger_, "The peer has performed an orderly shutdown.");
        }

        if(events & BEV_EVENT_CONNECTED)
        {
                LOG4CXX_INFO(logger_, "connected.");
        }

        if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
        {
                ph_->peerLogout(bev);
        }
}

void NetThreader::onReadParent(BufferEvent *bev)
{
        struct evbuffer *input = bufferevent_get_input(bev);
        size_t buffer_len = evbuffer_get_length(input);

        while(buffer_len > 0)
        {
                char buf[1] = {0};
                evbuffer_remove(input, &buf, 1);
                if(!strncmp(buf, "c", 1) && fdList_.size() > 0)
                {
                        int fd = fdList_.front();
                        fdList_.pop_front();
                        BufferEvent *bev = bufferevent_socket_new(base_, fd, BEV_OPT_CLOSE_ON_FREE);
                        if(!bev)
                        {
                                LOG4CXX_ERROR(logger_, "bufferevent_socket_new() error2.");
                                exit(-1);
                        }

                        bufferevent_setcb(bev, net_threader_read_cb, NULL, net_threader_event_cb, this);
                        bufferevent_enable(bev, EV_READ);
                }
                buffer_len--;
        }
}

void NetThreader::onRead(BufferEvent *bev)
{
        struct evbuffer *input = bufferevent_get_input(bev);

        string record;
        int ret = -1;
        while((ret = parserRecord(input, record)) == 1)
        {
                if(ret == -1)
                {
                        LOG4CXX_ERROR(logger_, "parser record error.");
                        if(ph_)
                        {
                                ph_->peerLogout(bev);
                        }
                        else
                        {
                                LOG4CXX_ERROR(logger_, "ProtocolHandler is null when logout.");
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
                                bufferRecordMap_.insert(make_pair(bev, record));
                        }
                        else
                        {
                                LOG4CXX_ERROR(logger_, "ProtocolHandler is null when map record.");
                        }
                }
        }
}

int NetThreader::parserRecord(struct evbuffer *buf, string &record)
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
                LOG4CXX_ERROR(logger_, "record length > " << LEAF_READ_SIZE_MAX);
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

void NetThreader::addFd(int fd)
{
        fdList_.push_back(fd);
}

void NetThreader::freeBufferEvent(BufferEvent *bev)
{
        int fd = bufferevent_getfd(bev);
        LOG4CXX_DEBUG(logger_, "The peer disconnect fd " << fd);
        bufferevent_free(bev);
}

static void read_parent_cb(BufferEvent *bev, void *ctx)
{
        NetThreader *nt = (NetThreader*)ctx;
        if(nt)
        {
                nt->onReadParent(bev);
        }
}

static void event_parent_cb(BufferEvent *bev, short events, void *ctx)
{
        NetThreader *nt = (NetThreader*)ctx;
        if(nt)
        {
                nt->onEvent(bev, events);
        }
}

static void net_threader_read_cb(BufferEvent *bev, void *ctx)
{
        /* This callback is invoked when there is data to read on bev. */
        NetThreader *nt = (NetThreader*)ctx;
        if(nt)
        {
                nt->onRead(bev);
        }
}

static void net_threader_event_cb(BufferEvent *bev, short events, void *ctx)
{
        NetThreader *nt = (NetThreader*)ctx;
        if(nt)
        {
                nt->onEvent(bev, events);
        }
}

void *g_net_threader_worker_livevent(void *ctx)
{
        NetThreader *nt = (NetThreader*)ctx;
        if(nt)
        {
                nt->loop();
        }

        return NULL;
}