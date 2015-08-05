#include <iostream>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <cstring>

using namespace std;

string G_addr;
string G_port;
int G_total_user;
char *G_exepath;
char *G_msg_num;

template<class T> 
string toString(T data) {
	stringstream ssm;
	ssm <<data;
	return ssm.str();
}

static void createClient(char *exepath, int uid)
{
	string uidstr = toString(uid);
        cout << "uid-" << uid << endl;
	
	pid_t pid;
	pid = fork();
	if(pid < 0)
	{
		cout << "fork error." << endl;
	}
	else if(pid == 0)
	{
		int e = execl(exepath, 
			"client",
			G_addr.c_str(),
			G_port.c_str(),
			uidstr.c_str(),
			(char*)0);
		if(e < 0)
		{
			cout << "execlp error[" << errno << "]:" << strerror(errno) << endl;
		}
	}
}

int main(int argc, char **argv)
{
	if(argc != 5)
	{
		cout << "usage:address, port, tol_usr_num, client_path" << endl;
		exit(-1);
	}
	G_addr = argv[1];
	G_port = argv[2];
	G_total_user = atoi(argv[3]);
	G_exepath = argv[4];

	for(int i = 0; i < G_total_user; i++)
	{
		createClient(G_exepath, i);
		sleep(1);	
	}
	return 0;
}
