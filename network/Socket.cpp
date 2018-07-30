#include "Socket.h"

Socket::Socket()
{
	m_bRun = false;
}

Socket::~Socket()
{
	closesocket(m_socket);
	WSACleanup();
}

void Socket::setAddr(const char* ip, UINT port)
{
	m_strIP = ip;
	m_iPort = port;
}

bool Socket::createSocket()
{
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		log("createsockt error error = %d", INVALID_SOCKET);
		return false;
	}
	char chOpt = 1;
	int iErr = setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(chOpt));
	log("scoket::setsockopt iErr = %d", iErr);
	if (iErr == SOCKET_ERROR)
	{
		log("scoket::setsockopt error = %d", WSAGetLastError());
	}

	return true;
}

bool Socket::connectSocket()
{
	SOCKADDR_IN serverAddr;
	
	if (!createSocket())
	{
		log("Sockt::connectSocket createSocket error");
		return false;
	}
	
	if (m_socket == INVALID_SOCKET)
	{
		log("Sockt::connectSocket createSocket invalid line = %d",__LINE__);
		return false;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	struct in_addr addr = { 0 };
	//inet_pton(AF_INET, "192.168.1.107", (void*)&addr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(m_strIP.c_str());;
	serverAddr.sin_port = htons(m_iPort);

	bool conRet = false;
	timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	if (connect(m_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int retErr = WSAGetLastError();
		if (retErr != EINPROGRESS)
		{
			retErr = false;
			log("socket::connect fail");
		}
		else
		{
			fd_set writeSet;
			FD_ZERO(&writeSet);
			FD_SET(m_socket,&writeSet);
			int socketRet = select(m_socket + 1,NULL,&writeSet,NULL, &timeout);
			if (socketRet > 0)
			{
				conRet = true;
			}
			else
			{
				if (socketRet == 0)
				{
					log("socket::connect time out");
					conRet = false;
				}
			}
		}
	}
	else
	{
		conRet = true;
	}

	return conRet;
}

void Socket::process()
{
	if (connectSocket())
	{
		while(m_bRun)
		{
			if (recvbuf(m_pHead, sizeof(m_pHead)))
			{
				//取的数据长度
				TCP_Head* head = (TCP_Head*)m_pHead;
				int curlen = head->TCPInfo.wPacketSize;
				//接受不完整
				if (curlen < sizeof(TCP_Head))
				{
					break;
				}

				char curBuf[20000] = { 0 };
				if (recvbuf(curBuf, curlen - sizeof(TCP_Head)))
				{
					memset(m_pBuf, 0, sizeof(m_pBuf));
					memcpy(m_pBuf, head,sizeof(head));
					memcpy(m_pBuf + sizeof(TCP_Head), curBuf, curlen - sizeof(TCP_Head));
				}
				
			}
		}
	}
}

void Socket::start()
{
	if (!m_bRun)
	{
		m_bRun = true;
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			log("WSAStartup()error! = %d", WSAGetLastError);
		}
#endif
		CreateThread(NULL,0,workFunc,this,0,NULL);
	}
}

int Socket::sendbuf(char* buf, int len)
{
	if (!buf)
		return -1;
	if (INVALID_SOCKET == m_socket)
	{
		log("Socket::send, ERROR: sockt invalid line=%d", __LINE__);
		return -1;
	}
	int ibytes = 0;
	int iCount = 0;
	while (iCount < len)
	{
		ibytes = send(m_socket, buf + ibytes, len - ibytes, 0);
		if (ibytes <len)
		{
			if (ibytes == -1 || ibytes == 0)
			{
				log("Socket::send, ERROR: Send error");
				return -1;
			}
			iCount += ibytes;

		}
	}
	
	return iCount;

}
bool Socket::recvbuf(char* buf, int size)
{
	int recvLen = 0;
	while (recvLen < size)
	{
		int ibufLen = recv(m_socket, buf + recvLen, size - recvLen, 0);
		if (ibufLen != 0 || ibufLen != -1)
		{
			recvLen += ibufLen;
		}
		else
		{
			log("Socket::recv error,error = %d", ibufLen);
			return false;
		}
	}
	return true;
}

DWORD WINAPI Socket::workFunc(LPVOID userdata)
{
	static_cast<Socket*>(userdata)->process();
	return 0;
}