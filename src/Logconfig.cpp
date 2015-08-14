#include "Logconfig.h"

Logconfig::Logconfig()
{

}

Logconfig::~Logconfig()
{

}

void Logconfig::init(string logProPath)
{
#ifdef _WIN32
        mkdir("logs");
#else
        mkdir("logs", 0777);
#endif

        log4cxx::PropertyConfigurator::configureAndWatch(logProPath);

        logger_ = log4cxx::Logger::getLogger("Logconfig");

        LOG4CXX_INFO(logger_, "------------------- leaf process start -------------------");
}