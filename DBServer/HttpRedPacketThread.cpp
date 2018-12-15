#include "HttpRedPacketThread.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "ActiveManager.h"



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

	//for test
	//RedPacketRequest rt;
	//rt.m_url = "www.baidu.com";
	//rt.m_userUUId = "wanglu";
	//gHRP.push(rt);
	//rt.m_url = "www";
	//gHRP.push(rt);

}

void HttpRedPacketThread::stop()
{
	if (_start == 0)
	{
		return;
	}

	if (_queue_ptr != NULL)
	{
		_queue_ptr->push(RedPacketRequest());
	}

	/** join函数，作用是等待直到线程执行结束；可不加，但不能保证退出Destroy函数前线程被终结 */
	if (_thread != NULL)
	{
		_thread->join();
	}
	LLOG_ERROR("remote log thread exit, push_index=%d, pop_index=%d\n", g_push_index, g_pop_index);
}

void HttpRedPacketThread::push(RedPacketRequest& request)
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
		auto request = _queue_ptr->front();
		_queue_ptr->pop();
		++g_pop_index;
		if (request.m_url.empty())
		{
			LLOG_ERROR("[WARN] HttpRedPacketThread url is empty, break run!\n");
			_start = 0;
			break;
		}
		requestRedPacket(request);
	}
}

void HttpRedPacketThread::requestRedPacket(RedPacketRequest& request)
{
	do
	{
		Lstring bindRet;
		if (!HttpRequest::Instance().HttpGet(request.m_url, bindRet, true))
		{
			LLOG_ERROR("HttpRedPacketThread::checkBindingRelation get bindinfo error %s", request.m_url.c_str());			
			break;
		}

		Json::Reader reader(Json::Features::strictMode());
		Json::Value  value;
		Lstring status;
		if (!reader.parse(bindRet, value))
		{
			LLOG_ERROR("Work::HanderCheckTaskFinished parsr json error %s", reader.getFormatedErrorMessages().c_str());
			break;
		}
		if (!value["status"].isNull() && value["status"].isString())
		{
			status = value["status"].asString();
		}
		else
		{
			break;
		}

		LLOG_ERROR("requestRedPacket user: %d, invitee: %d, type : %d,status: %s,url: %s", request.m_userId, request.m_invitee, request.m_type, status.c_str(), request.m_url.c_str());
	
		if (status != "1") //异常
		{
			if (status == "-1")
			{
				LMsgS2CRequestRedPacket send;;
				send.m_error = NotBind;//未绑定微信公众号
				gWork.SendToLMUser(request.m_userId, send);
				break;
			}
			if (status == "-4" || status == "-2")
			{
				LMsgS2CRequestRedPacket send;;
				send.m_error = OutOfMoney;//今日红包余额不足
				gWork.SendToLMUser(request.m_userId, send);
				break;
			}
			if (status == "-5")
			{
				LMsgS2CRequestRedPacket send;;
				send.m_error = OutOfChance;//重复领取
				gWork.SendToLMUser(request.m_userId, send);
				break;
			}
			if (status == "-3")
			{
				LMsgS2CRequestRedPacket send;;
				send.m_error = OutOfMoney;//当前玩家达到限额
				gWork.SendToLMUser(request.m_userId, send);
				break;
			}
			break;
		}
		else
		{
			std::stringstream ss;
			ss << "INSERT INTO redpacketactivitylog(SpecialId,Userid,Gifttype,GiftCount,Date) VALUES(";
			ss << 1004 << ",";
			ss << request.m_invitee << ",";
			ss << request.m_type << ",";
			ss << request.m_money << ",";
			ss << gWork.GetCurTime().Secs() << ")";
			Lstring* sql = new Lstring(ss.str());
			gWork.SaveMysql(*sql);

			LMsgS2CRequestRedPacket send;
			send.m_money = request.m_money;
			send.m_error = 0;
			send.m_type = request.m_type;
			send.m_inviteeId = request.m_invitee;
			gWork.SendToLMUser(request.m_userId, send);
			return;
		}
	} while (0);

	gActiveManager.ReturnRedPacketBack(request.m_index, request.m_money, request.m_invitee, request.m_type);
	
	
	/*std::stringstream ss;
	ss << "INSERT INTO redpacketactivitylog(SpecialId,Userid,Gifttype,GiftCount,Date) VALUES(";
	ss << 1004 << ",";
	ss << request.m_invitee << ",";
	ss << request.m_type << ",";
	ss << request.m_money << ",";
	ss << gWork.GetCurTime().Secs() << ")";
	Lstring* sql = new Lstring(ss.str());
	gWork.SaveMysql(*sql);

	LMsgS2CRequestRedPacket send;
	send.m_money = request.m_money;
	send.m_error = 0;
	send.m_type = request.m_type;
	send.m_inviteeId = request.m_invitee;
	gWork.SendToLMUser(request.m_userId, send);*/
}