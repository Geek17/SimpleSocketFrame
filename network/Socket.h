#include "cocos2d.h"
USING_NS_CC;

#ifdef _WINDOWS_
#include <Windows.h>
#include <thread>
#else
#include <pthread.h>
#endif

using namespace std;

typedef unsigned char BYTE;
typedef unsigned int  UINT;

//网络内核
struct TCP_Info
{
	BYTE							cbDataKind;							//数据类型
	BYTE							cbCheckCode;						//效验字段
	WORD							wPacketSize;						//数据大小
};

//网络命令
struct TCP_Command
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
};
//网络包头
struct TCP_Head
{
	TCP_Info						TCPInfo;							//基础结构
	TCP_Command						CommandInfo;						//命令信息
};

//长度定义
#define SOCKET_TCP_BUFFER			16384								//网络缓冲
#define SOCKET_TCP_PACKET			(SOCKET_TCP_BUFFER-sizeof(TCP_Head))//网络缓冲

class Socket : public Layer
{
public:
	Socket::Socket();
	Socket::~Socket();
public:
	bool createSocket();
	bool connectSocket();
	void setAddr(const char* ip, UINT port);
	void start();
	int sendbuf(char* buf, int len);
	bool recvbuf(char* buf,int size);
	void process();
public:
	static DWORD WINAPI workFunc(LPVOID userdata);
private:
	bool   m_bRun;
	string m_strIP;
	UINT   m_iPort;
	SOCKET m_socket;
	thread m_thread;
	char   m_pHead[sizeof(TCP_Head)];
	char   m_pBuf[SOCKET_TCP_BUFFER * 10];
};