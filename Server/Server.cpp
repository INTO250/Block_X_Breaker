#include"Server.h"
#include"IOOOOO.h"
int main()
{
	IOmode server(12345, "192.168.1.105");//ÿ�춼�û�����
	server.selectmode();
	return 0;
}