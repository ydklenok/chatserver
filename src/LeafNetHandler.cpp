#include "LeafNetHandler.h"

LeafNetHandler::LeafNetHandler(int port, int thread_num)
        :NetHandler(port, thread_num)
{
        
}

LeafNetHandler::~LeafNetHandler()
{
        
}

NetThreader* LeafNetHandler::newNetThreader(socket_t fd[2])
{
        LeafNetThreader *nt;
        LeafProtocolHandler *ph;

        nt = new LeafNetThreader(fd);
        ph = new LeafProtocolHandler(nt);
        nt->protocol(ph);

        return nt;
}