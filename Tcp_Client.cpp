// TCP_Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#define	 WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

//连接到WinSock2 对应的lib文件：Ws2_32.lib,Mswsock.lib,Advapi32.lib
#pragma comment (lib,"Ws2_32.lib")
#pragma comment (lib,"Mswsock.lib")
#pragma comment (lib,"AdvApi32.lib")
//定义缓冲区的长度和端口号
#define	DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define ip_addr "127.0.0.1"

int _cdecl main(int argc,char **argv)
{
	WSADATA wsaData;
	SOCKET ConnectionSocket=INVALID_SOCKET;
	struct addrinfo *result=NULL,*ptr=NULL,hints;
	char * sendbuf="this is a  test ";

	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen=DEFAULT_BUFLEN;

	
	//初始化套接字
	iResult =WSAStartup(MAKEWORD(2,2),&wsaData);
	if(iResult!=0)
	{
		printf("WSAStartup failed with error: %d \n", iResult);
		return -1;
	}

	ZeroMemory(&hints,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype =SOCK_STREAM;
	hints.ai_protocol=IPPROTO_TCP;
	
	//解析服务器地址和端口号
	iResult = getaddrinfo(ip_addr,DEFAULT_PORT,&hints,&result);
	if (iResult!=0)
	{
		printf("getaddrinfo failed with error: %d \n",iResult);
		WSACleanup();
		return 1;
	}

	//尝试连接服务器地址，直到成功
	for (ptr=result;ptr!=NULL;ptr=ptr->ai_next)
	{
		//创建套接字
		ConnectionSocket=socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);
		if (ConnectionSocket==INVALID_SOCKET)
		{
			printf("socket failed with error: %ld \n",WSAGetLastError());
			WSACleanup();
			return 1;
		}
		
		//向服务器请求连接
		iResult=connect(ConnectionSocket,ptr->ai_addr,(int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectionSocket);
			ConnectionSocket=INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);
	if (ConnectionSocket==INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	//发送缓冲区中的测试数据
	iResult=send(ConnectionSocket,sendbuf,(int)strlen(sendbuf),0);
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error :%d \n",WSAGetLastError());
		closesocket(ConnectionSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent:%ld \n",iResult);

	//数据发送结束，调用shutdown()函数声明不再发送数据，此时客户端仍然可以接受数据
	iResult=shutdown(ConnectionSocket,SD_SEND);
	if (iResult==SOCKET_ERROR)
	{
		printf("shutdown failed with error:%d \n",WSAGetLastError());
		closesocket(ConnectionSocket);
		WSACleanup();
		return 1;
	}

	//持续接受数据，直到服务器关闭连接
	do{
		iResult = recv(ConnectionSocket,recvbuf,recvbuflen,0);
		if (iResult>0)
		{
			printf("Bytes received: %d \n",iResult);
		}else if (iResult==0)
		{
			printf("Connection closed \n");
		}else
			printf("recv failed with error : %d \n",WSAGetLastError());
	}while (iResult>0);
	

	//关闭套接字
	closesocket(ConnectionSocket);
	//释放资源
	WSACleanup();
	return 0;
	
}
