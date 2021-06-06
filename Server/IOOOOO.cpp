
#include"IOOOOO.h"

IOmode::IOmode(int part,string ipaddr)
{

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	serveraddr.sin_family = AF_INET;//���������г�ʼ��
	serveraddr.sin_port = htons(part);
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr.c_str());
}
IOmode::~IOmode()
{
	WSACleanup();
}
void IOmode::blockmode()//����ģʽ,�Զ���ͻ���ûɶ��
{
	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in clientaddr;
	unsigned long u1 = 1;//0������������Ϊ������
	int nret = ioctlsocket(server, FIONBIO, (unsigned long*) &u1);
	bind(server, (sockaddr*)&serveraddr, sizeof(serveraddr));//��server

	char buffer[1024];

	listen(server, 5);
	int len = sizeof(sockaddr);
	SOCKET client = accept(server, (sockaddr*)&clientaddr, &len);

}
void IOmode::selectmode()//ѡ��ģ��
{
	vector<SOCKET>clientset;//�ͻ��˼���
	SOCKET clientsetA;
	SOCKET clientsetB;//��ש�������˶�ս��͵����д����������
	map<SOCKET, sockaddr_in>s2addr;//��ַ
	fd_set readfd;//���ڼ��ɶ�ȡ����
	int ret = 0;

	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	unsigned long u1 = 1;
	ioctlsocket(server, FIONBIO, (unsigned long*)&u1);//����Ϊ������

	bind(server, (sockaddr*)&serveraddr, sizeof(serveraddr));//��
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
			cout << inet_ntoa(addr.sin_addr) << "�Ѽ�������" << "   ��ǰ������: " << clientset.size() << endl;
		}
		FD_ZERO(&readfd);//��ʼ��
		for (int i = 0; i < clientset.size(); i++)
		{
			FD_SET((int)(clientset[i]), &readfd);//���
		}
		//�鿴�Ƿ������ݷ���
		ret = 0;
		if (!clientset.empty())
		{
			timeval tv = { 0,0 };
			ret = select(clientset[clientset.size() - 1] + 1, &readfd, NULL, NULL, &tv);
		}
		vector<SOCKET>deleteclient;//Ҫ�˳��Ŀͻ���
		if (clientset.size() == 1)
		{
			if (FD_ISSET((int)(clientset[0]), &readfd))
			{
				char data[1024] = { 0 };
				recv(clientset[0], data, 1023, 0);
				string recvdata = data;
				if (recvdata == "quit")
				{
					cout << "��ҶϿ�����" << endl;
					deleteclient.push_back(clientset[0]);
				}
			}
		}
		//��������
		if (ret > 0&& clientset.size()==2)
		{
			clientsetA=clientset[0];
			clientsetB=clientset[1];
			
			if (FD_ISSET((int)(clientsetA), &readfd))
			{
				char dataA[1024] = { 0 };
				recv(clientsetA, dataA, 1023, 0);
				string recvdata = dataA;
				cout << "����" << "���A"<< " : " << dataA << endl;
				send(clientsetB, dataA, sizeof(recvdata), 0);
				if (recvdata == "quit")
				{
					deleteclient.push_back(clientset[0]);
					cout << "���A�Ͽ�����" << endl;
				}
			}
			if (FD_ISSET((int)(clientsetB), &readfd))
			{
				char dataB[1024] = { 0 };
				recv(clientsetB, dataB, 1023, 0);
				string recvdata = dataB;
				cout << "����" << "���B" << " : " << dataB << endl;
				send(clientsetA, dataB, sizeof(recvdata), 0);
				if (recvdata == "quit")
				{
					deleteclient.push_back(clientset[0]);
					cout << "���B�Ͽ�����" << endl;
				}
			}
			//�ر�Ҫ�˳��Ŀͻ�
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