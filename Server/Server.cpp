#include"Server.h"
#include"IOOOOO.h"
int main()
{
	IOmode server(12345, "192.168.137.1");//每天都得换。。
	server.selectmode();
	return 0;
}