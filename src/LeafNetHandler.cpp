#include "LeafNetHandler.h"

LeafNetHandler::LeafNetHandler()
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