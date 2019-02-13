#pragma once


#include "LBase.h"
#include "LRunnable.h"
#include "LSingleton.h"
#include <list>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/lock_guard.hpp> 
#include <boost/thread/condition_variable.hpp>



class QuickStartManager : public LSingleton<QuickStartManager> 
{

public:
	QuickStartManager();
	~QuickStartManager();

public:
	virtual	bool	Init();
	virtual	bool	Final();

	void Start();
	void Stop();
	void Join();
	void Sleep(int interval);

	bool GetStop();

	//void Clear();

	/*
	*	brief 这个是线程运行的函数，根据具体逻辑去实现
	*/
	virtual void Run();

	void  PushUser(int user_id);
	bool  RemoveUser(int user_id);
	void  CheckCreateRoom();

private:
	
	std::list<int>		m_users_list;
	boost::condition_variable_any		m_canCreateRoom;
	boost::condition_variable_any		m_notFull;
	boost::mutex						m_lock;
	boost::mutex						m_remove_lock;
	boost::shared_ptr<boost::thread>	m_pthread;
	bool								m_stop;
};

#define gQuickStart QuickStartManager::Instance()