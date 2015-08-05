#include "LeafNetHandler.h"

LeafNetHandler::LeafNetHandler(size_t rsize)
        :NetHandler(rsize)
{
        
}

LeafNetHandler::~LeafNetHandler()
{

}

void LeafNetHandler::init()
{
        int port = 9876;
        createListen(port);
}