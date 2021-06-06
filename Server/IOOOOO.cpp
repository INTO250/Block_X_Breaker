
#include"IOOOOO.h"

IOmode::IOmode(int part,string ipaddr)
{

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	serveraddr.sin_family = AF_INET;//对这个类进行初始化
	serveraddr.sin_port = htons(part);
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr.c_str());
}
IOmode::~IOmode()
{
	WSACleanup();
}
void IOmode::blockmode()//阻塞模式,对多个客户端没啥用
{
	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in clientaddr;
	unsigned long u1 = 1;//0是阻塞，其余为非阻塞
	int nret = ioctlsocket(server, FIONBIO, (unsigned long*) &u1);
	bind(server, (sockaddr*)&serveraddr, sizeof(serveraddr));//绑定server

	char buffer[1024];

	listen(server, 5);
	int len = sizeof(sockaddr);
	SOCKET client = accept(server, (sockaddr*)&clientaddr, &len);

}
void IOmode::selectmode()//选择模型
{
	vector<SOCKET>clientset;//客户端集合
	SOCKET clientsetA;
	SOCKET clientsetB;//打砖块是两人对战，偷个懒写两个服务器
	map<SOCKET, sockaddr_in>s2addr;//地址
	fd_set readfd;//用于检查可读取数据
	int ret = 0;

	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	unsigned long u1 = 1;
	ioctlsocket(server, FIONBIO, (unsigned long*)&u1);//设置为非阻塞

	bind(server, (sockaddr*)&serveraddr, sizeof(serveraddr));//绑定
	listen(server, 5);
	
	while (1)
	{
		sockaddr_in addr;
		int len = sizeof(addr);
		int client = accept(server, (sockaddr*)&addr, &len);
		if (client != INVALID_SOCKET)
		{
			clientset.push_back(client);
			s2addr[client] = addr;
			cout << inet_ntoa(addr.sin_addr) << "已加入连接" << "   当前连接数: " << clientset.size() << endl;
		}
		FD_ZERO(&readfd);//初始化
		for (int i = 0; i < clientset.size(); i++)
		{
			FD_SET((int)(clientset[i]), &readfd);//检查
		}
		//查看是否有数据发送
		ret = 0;
		if (!clientset.empty())
		{
			timeval tv = { 0,0 };
			ret = select(clientset[clientset.size() - 1] + 1, &readfd, NULL, NULL, &tv);
		}
		vector<SOCKET>deleteclient;//要退出的客户端
		if (clientset.size() == 1)
		{
			if (FD_ISSET((int)(clientset[0]), &readfd))
			{
				char data[1024] = { 0 };
				recv(clientset[0], data, 1023, 0);
				string recvdata = data;
				if (recvdata == "quit")
				{
					cout << "玩家断开连接" << endl;
					deleteclient.push_back(clientset[0]);
				}
			}
		}
		//处理数据
		if (ret > 0&& clientset.size()==2)
		{
			clientsetA=clientset[0];
			clientsetB=clientset[1];
			
			if (FD_ISSET((int)(clientsetA), &readfd))
			{
				char dataA[1024] = { 0 };
				recv(clientsetA, dataA, 1023, 0);
				string recvdata = dataA;
				cout << "来自" << "玩家A"<< " : " << dataA << endl;
				send(clientsetB, dataA, sizeof(recvdata), 0);
				if (recvdata == "quit")
				{
					deleteclient.push_back(clientset[0]);
					cout << "玩家A断开连接" << endl;
				}
			}
			if (FD_ISSET((int)(clientsetB), &readfd))
			{
				char dataB[1024] = { 0 };
				recv(clientsetB, dataB, 1023, 0);
				string recvdata = dataB;
				cout << "来自" << "玩家B" << " : " << dataB << endl;
				send(clientsetA, dataB, sizeof(recvdata), 0);
				if (recvdata == "quit")
				{
					deleteclient.push_back(clientset[0]);
					cout << "玩家B断开连接" << endl;
				}
			}
			//关闭要退出的客户
			if (!deleteclient.empty())
			{
				for (int i = 0; i < deleteclient.size(); i++)
				{
					shutdown(deleteclient[i],2);
					closesocket(deleteclient[i]);
					vector<SOCKET>::iterator it = find(clientset.begin(), clientset.end(), deleteclient[i]);
					clientset.erase(it);
				}
			}
		}
	}
}