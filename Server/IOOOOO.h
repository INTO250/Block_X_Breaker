#pragma once
#include"Server.h"
class IOmode
{

private:
	sockaddr_in serveraddr;
public:
	IOmode(int part, string ipaddr);

	~IOmode();

	void blockmode();

	void selectmode();
};