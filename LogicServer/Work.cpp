#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"



#include <openssl/md5.h>
#include "TableManager.h"



using namespace boost::asio;  
using boost::asio::ip::tcp; 

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("LogicServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	TableManager::Instance().Load();

	LTime cur;
	m_30MSceTick = cur.MSecs();
	m_200MSceTick = cur.MSecs();
	m_1SceTick = cur.MSecs();
	m_5SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();

	m_loginLMFirst = true;

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	gInsideNet.Start();

	LRunnable::Start();

	ConnectToLogicManager();

	m_gcService.Start();
}

//等待
void Work::Join()
{
	gInsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();

}

//停止
void Work::Stop()
{
	m_logicManager.reset();

	m_gateInfo.clear();

	gInsideNet.Stop();
	
	LRunnable::Stop();
	
	m_gcService.Stop();
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
	if (cur.MSecs() - m_30MSceTick > 30)
	{
		m_30MSceTick = cur.MSecs();
	}

	//200毫秒循环一次
	if (cur.MSecs() - m_200MSceTick > 200)
	{
		m_200MSceTick = cur.MSecs();
	}

	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		
		m_1SceTick = cur.MSecs();
	}

	//5秒循环一次
	if (cur.MSecs() - m_5SceTick > 5000)
	{
		m_5SceTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
	
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
	}

	if(m_logicManager)
		m_logicManager->OnTick(cur);
	
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_HTTP:
		HanderHttp((LMsgHttp*)msg);
		break;
		
	case MSG_G_2_SERVER_LOGIN:
		HanderGateLogin((LMsgG2ServerLogin*)msg);
		break;
	case MSG_G_2_L_USER_MSG:
		{
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if (gate)
			{
				HanderGateUserMsg((LMsgG2LUserMsg*)msg,gate);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate not exiext");
			}
		}
		break;
	case MSG_G_2_L_USER_OUT_MSG:
		{
			LMsgG2LUserOutMsg* gmsg = (LMsgG2LUserOutMsg*)msg;
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if (gate)
			{
				//HanderUserLogout(gate->m_id,gmsg->m_userGateId);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate not exiext");
			}
		}
		break;
	
	case MSG_S_2_S_KEEP_ALIVE:
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE_ACK:
		HanderKeepAliveAck((LMsgS2SKeepAliveAck*)msg);
		break;
	case MSG_LM_2_L_DESK_OPT:
		HanderDeskOpt((LMsgLM2LDeskOpt*)msg);
		break;
	default:
		break;
	}
}



void Work::HanderUserKick(LMsgKick* msg)
{
	if (m_logicManager == msg->m_sp)
	{
		LLOG_ERROR("LogicManager DisConnect!");
		return;
	}

	HanderGateLogout(msg);
}


void Work::HanderClientIn(LMsgIn* msg)
{
	if(m_logicManager == msg->m_sp)
	{
		LLOG_ERROR("Connecting logicmanager on %s:%d succeed", m_logicManager->GetServerIP().c_str(), m_logicManager->GetServerPort());
		SendLogicLoginToLM();
	}
}

void Work::HanderHttp(LMsgHttp* msg)
{	

}

void Work::ConnectToLogicManager()
{
	KeepAliveSetting setting = {true, 15000, 10000};
	m_logicManager.reset(new TCPClient(gInsideNet, gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort(), setting));
	LLOG_ERROR("Work::ConnectToLogicManager begin %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
	m_logicManager->Connect();
	LLOG_ERROR("Work::ConnectToLogicManager end %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
}

void Work::SendToLogicManager(LMsg& msg)
{
	if(m_logicManager)
	{
		m_logicManager->Send(msg.GetSendBuff());
	}
}

void Work::HanderGateLogin(LMsgG2ServerLogin* msg)
{
	LLOG_ERROR("Work::HanderGateLogin Gate Login: %d", msg->m_id);

	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	auto iter = m_gateInfo.find(msg->m_id);
	if (iter != m_gateInfo.end())
	{
		if(iter->second.m_sp)
		{
			if(iter->second.m_sp != msg->m_sp)
			{
				iter->second.m_sp->Stop();
				DelGateInfo(msg->m_id);
				LLOG_ERROR("Work::HanderGateLogin: delete already existed gate info %d", msg->m_id);
			}
			else
				return;
		}

	}

	GateInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_userCount = 0;
	info.m_sp = msg->m_sp;
	m_gateInfo[info.m_id] = info;

	//SendGateInfoToCenter();
}

void Work::HanderGateLogout(LMsgKick* msg)
{
	GateInfo* info = GetGateInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR(" Work::HanderGateLogout: %d", info->m_id);
		DelGateInfo(info->m_id);
	}
}

GateInfo* Work::GetGateInfoBySp(LSocketPtr sp)
{
	std::map<Lint, GateInfo>::iterator it = m_gateInfo.begin();
	for (; it != m_gateInfo.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}

GateInfo* Work::GetGateInfoById(Lint id)
{
	if (m_gateInfo.count(id))
		return &m_gateInfo[id];

	return NULL;
}

void Work::DelGateInfo(Lint id)
{
	auto iter = m_gateInfo.find(id);
	if (iter != m_gateInfo.end())
	{
		m_gateInfo.erase(iter);
	}
}

void Work::HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate)
{
	int msgId = msg->m_userMsgId;
	if (msg->m_userMsg)
	{
		switch (msgId)
		{
		case MSG_C_2_S_READY_OPT:
			HanderDeskUserReadyOpt((LMsgC2SReadyOpt*)msg->m_userMsg);
			break;
		case MSG_C_2_S_CHESS_MOVE:
			HanderChessMove((LMsgC2SChessMove*)msg->m_userMsg);
			break;
		case MSG_C_2_S_CHESS_ATTACK:
			HanderChessAttack((LMsgC2SChessAttack*)msg->m_userMsg);
			break;
		case MSG_C_2_S_CHESS_UPGRADE:
			HanderChessUpgrade((LMsgC2SChessUpgrade*)msg->m_userMsg);
			break;
		}
	}
	else
	{
		//LLOG_ERROR("Work::HanderGateUserMsg msg error %d:%ll:%d", gateId, gateUserId, msg->m_userMsgId);
	}
}

Lstring Work::Md5Hex(const void* src ,Lsize len)
{
	unsigned char sign[17];
	memset(sign,0,sizeof(sign));

	MD5((const unsigned char*)src,len,sign);

	//转换成16进制
	char signHex[33];
	memset(signHex,0,sizeof(signHex));
	for(int i = 0 ; i < 16; ++i)
	{
		sprintf(signHex + i*2,"%02x", sign[i]);
	}

	return std::string(signHex);
}



void Work::SendLogicLoginToLM()
{
	LMsgL2LMGLogin login;
	login.m_id = gConfig.GetServerID();
	login.m_ip = gConfig.GetInsideIp();
	login.m_key = gConfig.GetCenterKey();
	login.m_port = gConfig.GetInsidePort();
	login.m_exclusive_play = gConfig.GetExclusivePlay();
	m_logicManager->Send(login.GetSendBuff());
	LLOG_ERROR("Login logicmanager succeed.");
	m_loginLMFirst = false;
}



void Work::HanderKeepAlive(LMsgS2SKeepAlive* msg)
{
	if(msg == NULL || !msg->m_sp)
		return;
	LLOG_DEBUG("KeepAlive from %s:%d received.", msg->m_sp->GetRemoteIp().c_str(), msg->m_sp->GetRemotePort());

	LMsgS2SKeepAliveAck ack;
	msg->m_sp->Send(ack.GetSendBuff());
}

void Work::HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg)
{
	if(msg == NULL)
		return;

	if(m_logicManager == msg->m_sp)
		m_logicManager->OnKeepAliveAck();
	
}

void Work::HanderDeskOpt(LMsgLM2LDeskOpt* msg)
{
	int user_id = msg->m_user_id;
	LMsgL2LMDeskOpt send;
	send.m_user_id = user_id;
	send.m_type = msg->m_type;
	int result = 0;
	if(msg->m_type == 1)
	{ 
		do
		{
			int desk_type = msg->m_desk_type;
			int cost = msg->m_cost;


			if (gDeskManager.IsHaveDesk(user_id))
			{
				result = 1;
				break;
			}

			int desk_id = msg->m_desk_id;
			if (gDeskManager.GetDeskById(desk_id))
			{
				result = 2;
				break;
			}

			LDeskPtr desk = gDeskManager.CreateDesk(desk_id);
			//desk->m_create_user_id = user_id;
			desk->m_desk_type = msg->m_desk_type;
			desk->m_cost = msg->m_cost;

			

			LUserPtr user = std::make_shared<User>(msg->m_self.m_user_id, msg->m_self.m_name, msg->m_self.m_head_icon);

			if (!gDeskManager.AddUserToDesk(desk, user))
			{
				result = 3;
				break;
			}

			if (result == 0)
			{
				FillDeskMsg(send, desk);
			}

		} while (0);

		msg->m_sp->Send(send.GetSendBuff());
	}
	//加入
	else if (msg->m_type == 2)
	{
		do
		{
			if (gDeskManager.IsHaveDesk(user_id))
			{
				result = 1;
				break;
			}

			LDeskPtr desk = gDeskManager.GetDeskById(msg->m_desk_id);
			if (desk == nullptr)
			{
				result = 4;
				break;
			}

			LUserPtr user = std::make_shared<User>(msg->m_self.m_user_id, msg->m_self.m_name, msg->m_self.m_head_icon);

			if (!gDeskManager.AddUserToDesk(desk, user))
			{
				result = 3;
				break;
			}

			send.m_result = result;
			if (result == 0)
			{
				FillDeskMsg(send, desk);
			}
		} while (0);

		msg->m_sp->Send(send.GetSendBuff());
	}
	else    //退出zhuozi
	{
		do
		{
			if (!gDeskManager.IsHaveDesk(user_id))
			{
				result = 5;
				break;
			}

			LDeskPtr desk = gDeskManager.GetDeskById(msg->m_desk_id);
			if (desk == nullptr)
			{
				result = 4;
				break;
			}

			
			gDeskManager.DeleteUserToDesk(desk, user_id);

			bool del_desk = false;
			if (desk->IsEmpty())
			{
				del_desk = gDeskManager.DeleteDesk(desk->m_desk_id);
			}
			
			send.m_result = result;
			if (result == 0)
			{
				send.m_desk_id = msg->m_desk_id;
				send.m_desk_type = desk->m_desk_type;
				send.m_cost = desk->m_cost;
				send.m_del_desk = del_desk;
			}

		} while (0);

		msg->m_sp->Send(send.GetSendBuff());
	}
}

void Work::HanderDeskUserReadyOpt(LMsgC2SReadyOpt* msg)
{
	int desk_id = msg->m_desk_id;
	int user_id = msg->m_user_id;

	LDeskPtr desk = gDeskManager.GetDeskById(desk_id);
	if (desk == nullptr)
		return;

	LUserPtr user = desk->GetDeskUser(user_id);
	if (user == nullptr)
		return;

	GateInfo* gate_server = GetGateInfoBySp(msg->m_sp);
	if (gate_server == nullptr)
		return;

	user->SetGateServerId(gate_server->m_id);
	//准备
	if (msg->m_type == 0)
	{
		user->SetStatus(STATUS_READY);

		desk->CheckStart();
	}
	else
	{
		user->SetStatus(STATUS_NULL);
	}

	

	LMsgS2CReadyOpt send;
	send.m_user_id = msg->m_user_id;
	send.m_status = user->GetStatus();

	SendMessageToUser(msg->m_sp, user_id, send);
}

void Work::HanderChessMove(LMsgC2SChessMove* msg)
{
	int desk_id = msg->m_desk_id;

	LDeskPtr desk = gDeskManager.GetDeskById(desk_id);
	if (desk == nullptr)
		return;

	if (desk->GetDeskStatus() != DS_CHESS)
		return;

	desk->HanderChessMove(msg);

}

void Work::HanderChessAttack(LMsgC2SChessAttack* msg)
{
	int desk_id = msg->m_desk_id;

	LDeskPtr desk = gDeskManager.GetDeskById(desk_id);
	if (desk == nullptr)
		return;

	if (desk->GetDeskStatus() != DS_CHESS)
		return;

	desk->HanderChessAttack(msg);
}

void Work::HanderChessUpgrade(LMsgC2SChessUpgrade* msg)
{
	int desk_id = msg->m_desk_id;

	LDeskPtr desk = gDeskManager.GetDeskById(desk_id);
	if (desk == nullptr)
		return;

	if (desk->GetDeskStatus() != DS_CHESS)
		return;

	desk->HanderChessUpgrade(msg);
}


void Work::SendMessageToUser(LSocketPtr sp, Lint user_id, LMsg& msg)
{
	GateInfo* info = GetGateInfoBySp(sp);
	if (nullptr != info)
	{
		LMsgL2GUserMsg sendMsg;
		sendMsg.m_user_id = user_id;
		sendMsg.m_user_msg_id = msg.m_msgId;
		sendMsg.m_dataBuff = msg.GetSendBuff();

		info->m_sp->Send(sendMsg.GetSendBuff());
	}
}

void Work::FillDeskMsg(LMsgL2LMDeskOpt& send, LDeskPtr desk)
{
	send.m_desk_id = desk->m_desk_id;
	send.m_desk_type = desk->m_desk_type;
	send.m_cost = desk->m_cost;

	for (int i = 0; i < desk->GetAllUsers().size(); ++i)
	{
		LUserPtr user = desk->GetAllUsers()[i];

		MsgUserInfo msgUser;
		msgUser.m_user_id = user->GetUserId();
		msgUser.m_name = user->GetName();
		msgUser.m_head_icon = user->GetHeadIcon();

		send.m_users.push_back(msgUser);
	}
}



