#include "HttpRedPacketThread.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "UserManager.h"



static unsigned int g_push_index = 0;
static unsigned int g_pop_index = 0;

int HttpRedPacketThread::Init(int start)
{
	_start = start;
	if (_start == 0)
	{
		LLOG_ERROR("HttpRedPacketThread not start, config start = %d.", _start);
		return 0;
	}

	_queue_ptr = boost::shared_ptr<UrlQueue>(new UrlQueue());
	assert(_queue_ptr != NULL);
	_queue_ptr->init(100000 + 1);
	return 1;
}

void HttpRedPacketThread::start()
{
	if (_start == 0)
	{
		return;
	}

	_thread = new boost::thread(boost::bind(&HttpRedPacketThread::run, this));
	assert(_thread != NULL);
}

void HttpRedPacketThread::stop()
{
	if (_start == 0)
	{
		return;
	}

	if (_queue_ptr != NULL)
	{
		_queue_ptr->push(BindRegRequest());
	}

	/** join函数，作用是等待直到线程执行结束；可不加，但不能保证退出Destroy函数前线程被终结 */
	if (_thread != NULL)
	{
		_thread->join();
	}
	LLOG_ERROR("remote log thread exit, push_index=%d, pop_index=%d\n", g_push_index, g_pop_index);
}

void HttpRedPacketThread::push(BindRegRequest& request)
{
	boost::mutex::scoped_lock l(m_mutexUrlQueue);
	LLOG_DEBUG("redpacket_test_write!");
	if (_start == 0)
	{
		return;
	}
	
	if (request.m_url.empty())
	{
		LLOG_ERROR("[WARN] write log is empty!\n");
		return;
	}
	
	++g_push_index;
	if (_queue_ptr == NULL)
	{
		LLOG_ERROR("[ERROR] function:%s, _queue_ptr is null!", __FUNCTION__);
		return;
	}
	LLOG_DEBUG("[ERROR] function:%s,_queue_ptr size :%d!", __FUNCTION__, (int)_queue_ptr->size());
	int ret = _queue_ptr->push(request);
}

void HttpRedPacketThread::run()
{
	LLOG_DEBUG("HttpRedPacketThread runing");
	while (true)
	{
		if (_start == 0)
		{
			return;
		}

		if (_queue_ptr == NULL)
		{
			LLOG_ERROR("[ERROR] function:%s, _queue_ptr is null!", __FUNCTION__);
			return;
		}

		if (_queue_ptr->empty())
		{
#ifdef _LINUX
			::usleep(10);
#else
			Sleep(1);
#endif // _LINUX
			continue;
		}
		auto url = _queue_ptr->front();
		_queue_ptr->pop();
		++g_pop_index;
		if (url.m_url.empty())
		{
			LLOG_ERROR("[WARN] HttpRedPacketThread url is empty, break run!\n");
			_start = 0;
			break;
		}
		checkBindingRelation(url);
	}
}

void HttpRedPacketThread::checkBindingRelation(BindRegRequest& request)
{
	Lstring bindRet;
	if (!HttpRequest::Instance().HttpGet(request.m_url, bindRet, true))
	{
		LLOG_ERROR("HttpRedPacketThread::checkBindingRelation get bindinfo error %s", request.m_url.c_str());
		return;
	}
	LLOG_ERROR("checkBindingRelation  %s,bindurl %s", bindRet.c_str(), request.m_url.c_str());

	Json::Reader reader(Json::Features::strictMode());
	Json::Value  value;
	Lstring status, inviter;
	if (!reader.parse(bindRet, value))
	{
		LLOG_ERROR("Work::HanderCheckTaskFinished parsr json error %s", reader.getFormatedErrorMessages().c_str());
		return;
	}
	if (!value["status"].isNull() && value["status"].isString())
	{
		status = value["status"].asString();
	}
	if (status == "1")
	{
		if (value["extraData"].isNull() && !value["extraData"].isString())
			return;
		if (value["extraData"]["inviter"].isNull() && !value["extraData"]["inviter"].isString())
			return;

		inviter = value["extraData"]["inviter"].asString();

		LMsgCe2LDBBindingRelation send;
		send.m_invitee = request.m_invitee;
		auto it = gUserManager.GetUserByOpenId(inviter);
		if (it)
		{
			if(it->CanFenXiangRedpacket())
			{
				send.m_inviter = it->m_usert.m_id;
				gWork.SendMsgToAllLogicManager(send);
			}
		}
	}

	/*LMsgCe2LDBBindingRelation send;
	Lstring inviter = "a";
	send.m_invitee = request.m_invitee;
	auto it = gUserManager.GetUserByOpenId(inviter);
	if (it)
	{
		send.m_inviter = it->m_usert.m_id;
		gWork.SendMsgToAllLogicManager(send);
	}*/
}