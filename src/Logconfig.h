#ifndef __LOG_CONFIG_H_H__
#define __LOG_CONFIG_H_H__

#include <string>

#ifdef _WIN32
#include <direct.h>
#else
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <log4cxx/log4cxx.h>
#include <log4cxx/propertyconfigurator.h>

using namespace std;

class Logconfig
{
public:
        Logconfig();
        ~Logconfig();

        void init(string logpro);

private:
        log4cxx::LoggerPtr logger_;
};

#endif