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
struct BindRegRequest
{
	Lstring m_url;
	Lint	m_invitee;
};

class HttpRedPacketThread : public LSingleton<HttpRedPacketThread>
{
	typedef CircularArrayQueue<BindRegRequest> UrlQueue;

public:
	int Init(int start);
	void start();
	void stop();
	void push(BindRegRequest& url);
	void checkBindingRelation(BindRegRequest& url);

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
