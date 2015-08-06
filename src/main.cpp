#include <iostream>

#include "NetHandler.h"
#include "LeafNetHandler.h"
#include "LeafProtocolHandler.h"

using namespace std;

int main(int argc, char *argv[])
{
        LeafNetHandler *nh = new LeafNetHandler();
        LeafProtocolHandler *cph = new LeafProtocolHandler(nh);
        cph->loop();
        return 0;
}