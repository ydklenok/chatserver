#include <string>

#include "Logconfig.h"
#include "LeafNetHandler.h"

using namespace std;

int main(int argc, char *argv[])
{
        Logconfig *lc = new Logconfig;
        lc->init("conf/leaf.pro");

        int port = 9876;
        int thread_num = 8;
        LeafNetHandler *nh = new LeafNetHandler(port, thread_num);
        nh->start();

        return 0;
}