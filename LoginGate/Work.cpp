#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "OutsideNet.h"
#include "BaseCommonFunc.h"
#include "HttpRequest.h"

//初始化
bool Work::Init()
{
	//首先设置log文件，不然log没有开启，log文件没办法查看
	LLog::Instance().SetFileName("LoginGate");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	//设置log级别
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	//网络消息初始化
	LMsgFactory::Instance().Init();

	//外部端口初始化
	if (!gOutsideNet.Init())
	{
		LLOG_ERROR("gOutsideNet.Init error");
		return false;
	}

	if (!m_wxAuthService.Init()) {
		LLOG_ERROR("m_wxAuthService.Init error");
		return false;
	}

	LTime cur;
	m_30MSecTick = cur.MSecs();
	m_200MSecTick = cur.MSecs();
	m_1SecTick = cur.MSecs();
	m_5SecTick = cur.MSecs();
	m_30SecTick = cur.MSecs();
	m_600SecTick = cur.MSecs();

	m_allocClientId = 0;

	return true;
}

bool Work::Final()
{
	m_wxAuthService.Final();
	return true;
}

//启动
void Work::Start()
{
	//主线程开启
	LRunnable::Start();

	//外部网络开启
	gOutsideNet.Start();

	ConnectToCenter();

	m_gcService.Start();
	m_wxAuthService.Start();
}

//等待
void Work::Join()
{
	//等待网络线程终止
	gOutsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	gOutsideNet.Stop();

	LRunnable::Stop();

	m_gcService.Stop();
	m_wxAuthService.Stop();

	m_mapClientInfo.clear();
	m_mapId2Client.clear();
}

void Work::Clear()
{
}

void Work::Run()
{
	while(!GetStop())
	{
		//这里处理游戏主循环
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			Tick(cur);	
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
			HanderMsg(msg);
			//delete msg;
			m_gcService.Collect(msg);
		}
	}
}

void Work::Tick(LTime& cur)
{
	//30毫秒循环一次
	if (cur.MSecs() - m_30MSecTick > 30)
	{
		m_30MSecTick = cur.MSecs();
	}

	//200毫秒循环一次
	if (cur.MSecs() - m_200MSecTick > 200)
	{
		m_200MSecTick = cur.MSecs();
	}

	//1000毫秒循环一次
	if (cur.MSecs() - m_1SecTick > 1000)
	{
		m_1SecTick = cur.MSecs();
	}

	//5秒循环一次
	if (cur.MSecs() - m_5SecTick > 5000)
	{
		m_5SecTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SecTick > 30*1000)
	{
		m_30SecTick = cur.MSecs();
		CheckTimeout(cur);
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SecTick > 5*60 * 1000)
	{
		m_600SecTick = cur.MSecs();
	}

	if(m_centerClient)
		m_centerClient->OnTick(cur);
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	if (msg == NULL)
	{
		return;
	}

	switch(msg->m_msgId)
	{
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_C_2_S_MSG:
		HanderUserLogin((LMsgC2SMsg*)msg);
		break;
	case MSG_LG_CE_USER_MSG_WRAPPER:
		HanderCEWrappedMsg((LMsgLGCEUserMsgWrapper*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE_ACK:
		m_centerClient->OnKeepAliveAck();
		break;
	case MSG_C_2_S_LOGIN_TOKEN:
		HanderLoginToken(dynamic_cast<LMsgC2SLoginToken*>(msg));
		break;
	default:
		{
			LLOG_DEBUG("Work::HanderMsg -- Wrong msg. msgid = %d", msg->m_msgId);
			if(msg->m_sp)
			{
				msg->m_sp->Stop();
				DelSp(msg->m_sp);
			}
			break;
		}
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if(msg == NULL)
		return;

	if(m_centerClient == msg->m_sp)
	{
		LLOG_ERROR("Connecting center on %s:%d succeed", m_centerClient->GetServerIP().c_str(), m_centerClient->GetServerPort());
		return;
	}

	AddSp(msg->m_sp);
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if(msg == NULL)
		return;
	if(m_centerClient == msg->m_sp)
		LLOG_ERROR("lose connection with center server!!!");
	else
		DelSp(msg->m_sp);
}

void Work::HanderUserLogin(LMsgC2SMsg* msg)
{
	LLOG_DEBUG("Work::HanderUserLogin");
	if (msg == NULL)
	{
		return;
	}

	auto itId = m_mapClientInfo.find(msg->m_sp);
	if(itId == m_mapClientInfo.end())
	{
		//kick
		return;
	}

	//MD5验证
	if (!(gConfig.GetLocalLogin() && !msg->m_plate.compare("local")) && (!CheckMd5Hex(msg)))
	{
		msg->m_sp->Stop();
		DelSp(msg->m_sp);
		return;
	}

	itId->second.time = GetCurTime().Secs();
	LMsgLGCEUserMsgWrapper wrapper;
	wrapper.m_idOnGate = itId->second.idOnGate;
	msg->m_clientIp = msg->m_sp ? msg->m_sp->GetRemoteIp() : "";
	wrapper.m_userMsg = msg;
	wrapper.m_deleteUserMsg = false;
	m_centerClient->Send(wrapper.GetSendBuff());
}

void Work::HanderCEWrappedMsg(LMsgLGCEUserMsgWrapper* msg)
{
	LLOG_DEBUG("Enter Work::HanderCEWrappedMsg");
	if(msg == NULL || msg->m_userMsg == NULL)
	{
		LLOG_ERROR("[ERROR] HanderCEWrappedMsg msg null");
		return;
	}

	auto itSp = m_mapId2Client.find(msg->m_idOnGate);
	if(itSp == m_mapId2Client.end() || itSp->second.get() == NULL)
	{
		LLOG_ERROR("[ERROR] idonGate is error");
		return;
	}

	if(msg->m_userMsg->m_msgId == MSG_S_2_C_MSG)
	{
		LMsgS2CMsg* retLoginMsg = (LMsgS2CMsg*)(msg->m_userMsg);
		if(retLoginMsg->m_errorCode == 6)		// MD5校验失败直接断开连接
		{
			itSp->second->Stop();
			DelSp(itSp->second);
			return;
		}

		itSp->second->Send(msg->m_userMsg->GetSendBuff());
	}
	else if (msg->m_userMsg->m_msgId == MSG_C_2_S_MSG) 
	{
		LMsgC2SMsg *umsg = (LMsgC2SMsg *)(msg->m_userMsg);

		if (umsg->m_wxConfirmed == 2) 
		{ 
			LLOG_DEBUG("[DEBUG] Auth failed del socket in Work.");
			// 由WeiXinAuthService发到这里的消息，表示微信验证失败。需要关闭socket了。
			itSp->second->Stop();
			DelSp(itSp->second);
			return;
		}
		else
		{
			LLOG_DEBUG("[DEBUG] Push msg to wx Auth Service");
			m_wxAuthService.Push(msg);
		}
	}

	LLOG_DEBUG("Leave Work::HanderCEWrappedMsg");
}

void Work::HanderLoginToken(LMsgC2SLoginToken *msg)
{
	if (NULL == msg)
	{
		LLOG_ERROR("%s:: message is error.",__FUNCTION__);
		return;
	}

	SendLoginToken(msg->m_sp);

	LLOG_DEBUG("%s:: send login token.",__FUNCTION__);
}

void Work::AddSp(LSocketPtr sp)
{
	auto itSp = m_mapClientInfo.find(sp);
	if(itSp != m_mapClientInfo.end())
	{
		return;
	}

	Llong id = ++m_allocClientId;
	m_mapId2Client[id] = sp;

	CLIENT_INFO clientInfo;
	clientInfo.idOnGate = id;
	clientInfo.time = GetCurTime().Secs();
	clientInfo.m_token = boost::lexical_cast<std::string>(m_secret_key());
	
	m_mapClientInfo[sp] = clientInfo;
}

void Work::DelSp(LSocketPtr sp)
{
	auto itSp = m_mapClientInfo.find(sp);
	if(itSp != m_mapClientInfo.end())
	{
		m_mapId2Client.erase(itSp->second.idOnGate);
		m_mapClientInfo.erase(itSp);
	}
}

void Work::ConnectToCenter(void)
{
	KeepAliveSetting setting = {true, 15000, 5000};
	MsgPackReceiver receiver = [this](LBuffPtr recv, LSocketPtr s)
	{
		LMsg* msg = ::ParseInsideNetPacket(recv, s);
		if(msg)
			this->Push(msg);
	};

	m_centerClient.reset(new TCPClient(gOutsideNet, gConfig.GetCenterIp(), gConfig.GetCenterPort(), setting, receiver));
	LLOG_ERROR("Start to connect center on %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	bool succeed = m_centerClient->Connect();
	if(!succeed)
	{
		LLOG_ERROR("Since failed to connect center on %s:%d, stopping...", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
		Sleep(1000);
		Stop();
	}
}

void Work::CheckTimeout(LTime& cur)
{
	//关闭不活动的socket链接
	for (auto itSp = m_mapClientInfo.begin() ; itSp != m_mapClientInfo.end(); )
	{
		if((cur.Secs() - itSp->second.time) >= 30)
		{
			LLOG_DEBUG("Due to timeout, connection with endpoint %s:%d was closed.", itSp->first->GetRemoteIp().c_str(), itSp->first->GetRemotePort());
			itSp->first->Stop();
			m_mapId2Client.erase(itSp->second.idOnGate);
			m_mapClientInfo.erase(itSp++);
		}
		else
		{
			++itSp;
		}
	}
}

bool Work::CheckMd5Hex(const LMsgC2SMsg* msg)
{

	if (NULL != msg && !msg->m_md5.empty())
	{
		
		auto it = m_mapClientInfo.find(msg->m_sp);

		if (it != m_mapClientInfo.end())
		{
			Lstring str = msg->m_openId;
			str += msg->m_accessToken;
			str += msg->m_refreshToken;
			str += msg->m_uuid;		
			str += it->second.m_token;
			str += "3c6e0b8a9c15224a8228b9a98ca1531d";
			
			if(HttpRequest::Instance().Md5Hex(str.c_str(),str.size()) == msg->m_md5)
			{
				return true;
			}
			else
			{
				LLOG_ERROR("Work::HanderUserWechatLogin msd5 error %s : %s : %s",msg->m_openId.c_str(),msg->m_accessToken.c_str(),msg->m_md5.c_str());
			}
		}
	}

	return false;
}

void Work::SendLoginToken(LSocketPtr client_sp)
{
	if (NULL != client_sp)
	{
		auto it = m_mapClientInfo.find(client_sp);

		if (it != m_mapClientInfo.end())
		{
			LMsgS2CLoginToken login_token(it->second.m_token);
			client_sp->Send(login_token.GetSendBuff());
		}
	}
}

void Work::SendToCenter(LMsg& msg)
{
	if (m_centerClient)
	{
		m_centerClient->Send(msg.GetSendBuff());
	}
}