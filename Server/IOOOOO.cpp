
#include"Server.h"
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
	map<SOCKET, sockaddr_in>s2addr;//地址
	fd_set readfd;//用于检查可读取数据
	int ret = 0;

	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	unsigned long u1 = 1;
	ioctlsocket(server, FIONBIO, (unsigned long*)&u1);//设置为非阻塞

	bind(server, (sockaddr*)&serveraddr, sizeof(serveraddr));//绑定
	listen(server, 2);
	bool Astart = false;
	bool Bstart = false;
	while (1)
	{
		sockaddr_in addr;
		int len = sizeof(addr);
		int client = accept(server, (sockaddr*)&addr, &len);
		if (client != INVALID_SOCKET)
		{
			clientset.push_back(client);
			s2addr[client] = addr;
			send(clientset.back(), "connected", sizeof("connected"), 0);
			cout << inet_ntoa(addr.sin_addr) << "已加入连接" << "   当前连接数: " << clientset.size() << endl;
			if (clientset.size() == 2)
			{
				send(clientset[0], "matched", sizeof("matched"), 0);
				send(clientset[1], "matched", sizeof("matched"), 0);
			}
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
		//处理数据
		if (ret > 0)
		{
			vector<SOCKET> deleteclient;//要退出的套接字，待删除的套接字vector
			
			for (int i = 0; i < clientset.size(); i++)
			{
				if (FD_ISSET((int)(clientset[i]), &readfd))
				{
					char data[1024] = { 0 };
				    int ret=recv(clientset[i], data, 1023, 0);
					string recvdata = data;
					if (recvdata == "quit" || recvdata == "win" || recvdata == "lose")
					{
						if (recvdata == "win" || recvdata == "lose")
						{
							if (clientset.size() == 2)
							{
								if (FD_ISSET((int)(clientset[0]), &readfd))
								{
									int ret1 = send(clientset[1], recvdata.c_str(), recvdata.size(), 0);
									Sleep(5000);
								
								}
								if (FD_ISSET((int)(clientset[1]), &readfd))
								{

									int ret2 = send(clientset[0], recvdata.c_str(), recvdata.size(), 0);
									Sleep(5000);
									
								}
							}
						}
						deleteclient.push_back(clientset[i]);
						
					}
					else
					{
	
						std::cout << "来自" << inet_ntoa(s2addr[clientset[i]].sin_addr) << " : " << data << endl;
					}
					if (clientset.size() == 2)
					{
						if (FD_ISSET((int)(clientset[0]), &readfd))
						{
							int ret1 = send(clientset[1], recvdata.c_str(), recvdata.size(), 0);
							
							if (recvdata == "ready")
							{
								Astart = true;
							}
						}
						if (FD_ISSET((int)(clientset[1]), &readfd))
						{
						
							int ret2=send(clientset[0], recvdata.c_str(), recvdata.size(), 0);
				
							if (recvdata == "ready")
							{
								Bstart = true;
							}
						}
					}
				}
			}
			if (Astart && Bstart)
			{  
				send(clientset[0], "start", sizeof("start"), 0);
				send(clientset[1], "start", sizeof("start"), 0);
				Sleep(3000);
				srand((unsigned)time(NULL));
				char a = char(rand() % 4 + 49);
				string level = "";
				level += a;
				send(clientset[0], level.c_str(),level.size(), 0);
				send(clientset[1], level.c_str(), level.size(), 0);
				Astart = false;
				Bstart = false;//改掉避免持续发送
			}
			if (!deleteclient.empty())
			{
				for (int i = 0; i < deleteclient.size(); i++)
				{
					if(i==0)
					    std::cout << "玩家A" << "已退出连接，剩余连接数: " << clientset.size() - 1 << endl;
					else
						std::cout << "玩家B" << "已退出连接，剩余连接数: " << clientset.size() - 1 << endl;
					closesocket(deleteclient[i]);
					vector<SOCKET>::iterator it = find(clientset.begin(), clientset.end(), deleteclient[i]);
					clientset.erase(it);
				}
			}
		}
	}
}