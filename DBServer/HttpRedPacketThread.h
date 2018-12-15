#ifndef __HTTP_RED_PACKET_THREAD__
#define __HTTP_RED_PACKET_THREAD__

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <sstream>
#include <iostream>
#include "LBase.h"
#include "LSingleton.h"
#include "CircularArrayQueue.h"
#include "HttpRequest.h"

using namespace std;

enum RedPacketType
{
	inviteeFirst  =  1, //被邀请者第一次领取
	inviterFirst  =  2, //邀请者第一次领取
	inviteeSecond =  3, //被邀请者第二次领取
	inviterSecond =  4, //邀请者第二次领取
};

struct RedPacketRequest
{
	Lint	m_userId;
	Lint	m_invitee; //每个被邀请者确定4个红包
	Lint    m_type;
	Lstring m_url;
	Lint    m_index;   //红包档位
	Lint	m_money;   //红包金额
};

class HttpRedPacketThread : public LSingleton<HttpRedPacketThread>
{
	typedef CircularArrayQueue<RedPacketRequest> UrlQueue;

public:
	int Init(int start);
	void start();
	void stop();
	void push(RedPacketRequest& url);
	void requestRedPacket(RedPacketRequest& url);

	// 单例LSingleton一定要强制实现这两个函数感觉比较...
	bool Init()
	{
		return true;
	}

	bool Final()
	{
		return true;
	}

private:
	void run(void);
	boost::thread* _thread;
	boost::shared_ptr<UrlQueue>  _queue_ptr;
	int _start;
	boost::mutex m_mutexUrlQueue;
	boost::mutex m_mutexCatchFile;
};

#define gHRP HttpRedPacketThread::Instance()

#endif
