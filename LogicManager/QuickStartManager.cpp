#include "QuickStartManager.h"
#include "LMsgS2S.h"
#include "Work.h"


QuickStartManager::QuickStartManager()
{
	m_stop = false;
}

QuickStartManager::~QuickStartManager()
{

}

bool QuickStartManager::Init()
{
	return true;
}

bool QuickStartManager::Final()
{
	return true;
}

void QuickStartManager::Start()
{
	if (!m_pthread)
	{
		m_pthread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&QuickStartManager::Run, this)));
	}
}

void QuickStartManager::Stop()
{
	m_stop = true;
}

void QuickStartManager::Join()
{
	if (m_pthread)
	{
		m_pthread->join();
	}
}

void QuickStartManager::Sleep(int interval)
{
	boost::this_thread::sleep(boost::posix_time::millisec(interval));
}


bool QuickStartManager::GetStop()
{
	return m_stop;
}


void QuickStartManager::Run()
{
	while (!GetStop())
	{
		LLOG_ERROR("QuickStartManager::Run");
		CheckCreateRoom();
	}
}

void QuickStartManager::PushUser(int user_id)
{
	boost::lock_guard<boost::mutex> guard(m_lock);
	//·ÀÖ¹Ðé¼Ù»½ÐÑ
	while (m_users_list.size() > LIST_USER_MAX_CNT)
	{
		m_notFull.wait(m_lock);
	}
	
	m_users_list.push_front(user_id);
	if (m_users_list.size() >= ROOM_USER_CNT)
	{
		m_canCreateRoom.notify_all();
	}
}

bool QuickStartManager::RemoveUser(int user_id)
{
	if (user_id <= 0)
		return false;

	boost::lock_guard<boost::mutex> guard(m_lock);

	for (auto iter = m_users_list.begin(); iter != m_users_list.end(); iter++)
	{
		if ((*iter) == user_id)
		{
			iter = m_users_list.erase(iter);

			m_notFull.notify_all();
			return true;
		}
	}
	return false;
}

void QuickStartManager::CheckCreateRoom()
{
	LLOG_ERROR("CheckCreateRoom");
	boost::lock_guard<boost::mutex> guard(m_lock);

	while (m_users_list.size() < ROOM_USER_CNT)
	{
		m_canCreateRoom.wait(m_lock);
	}
	LLOG_ERROR("Succeed CreateRoom");

	LMsgLM2LMQuckCreateRoom* send =(LMsgLM2LMQuckCreateRoom*)LMsgFactory::Instance().CreateMsg(MSG_LM_2_LM_QUICK_CREATE_ROOM);
	for (int i = 0; i < ROOM_USER_CNT; ++i)
	{
		int user_id = m_users_list.back();
		
		send->m_users.push_back(user_id);

		m_users_list.pop_back();
	}
	gWork.Push(send);

	m_notFull.notify_all();
}
