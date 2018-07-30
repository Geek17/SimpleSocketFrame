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

//�����ں�
struct TCP_Info
{
	BYTE							cbDataKind;							//��������
	BYTE							cbCheckCode;						//Ч���ֶ�
	WORD							wPacketSize;						//���ݴ�С
};

//��������
struct TCP_Command
{
	WORD							wMainCmdID;							//��������
	WORD							wSubCmdID;							//��������
};
//�����ͷ
struct TCP_Head
{
	TCP_Info						TCPInfo;							//�����ṹ
	TCP_Command						CommandInfo;						//������Ϣ
};

//���ȶ���
#define SOCKET_TCP_BUFFER			16384								//���绺��
#define SOCKET_TCP_PACKET			(SOCKET_TCP_BUFFER-sizeof(TCP_Head))//���绺��

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