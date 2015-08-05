#include <iostream>

#include "NetHandler.h"
#include "LeafNetHandler.h"
#include "LeafProtocolHandler.h"

using namespace std;

#define LEAF_READ_SIZE_MAX (2048)
int main(int argc, char *argv[])
{
        LeafNetHandler *nh = new LeafNetHandler(LEAF_READ_SIZE_MAX);
        LeafProtocolHandler *cph = new LeafProtocolHandler(nh);
        cph->loop();
}