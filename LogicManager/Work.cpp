#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"


#include <openssl/md5.h>
#include "RoomManager.h"
#include "LVipLog.h"


#include <boost/regex.hpp>
#include "RemoteLogThread.h"

#include "QuickStartManager.h"
#include "UserManager.h"




using namespace boost::asio;  
using boost::asio::ip::tcp; 

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("LogicManager");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	if (!gRoomManager.Init())
	{
		LLOG_ERROR("DeskManager.Init error");
		return false;
	}

	if (!gUserManager.Init())
	{
		return false;
	}
	
	/*if(gRLT.init(gConfig.GetRemoteLogThreadStart(), 
		(const char*)gConfig.GetRemoteLogUrl().c_str(),
		(const char*)gConfig.GetRemoteLogCacheFileName().c_str(),
		(size_t)gConfig.GetMaxCachedLogSize(),
		(size_t)gConfig.GetMaxCachedLogNum(),
		(size_t)gConfig.GetMaxFileLogSize()) < 0)
	{
		LLOG_ERROR("gRLT.init error");
		return false;
	}*/

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	gQuickStart.Init();
	
	LLOG_ERROR("LogicManager Succeed");
	LTime cur;
	m_30MSceTick = cur.MSecs();
	m_200MSceTick = cur.MSecs();
	m_1SceTick = cur.MSecs();
	m_5SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();

	m_open_ios_pay = false;
	m_open_android_pay = false;

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

	gRLT.start();

	ConnectToCenter();


	m_gcService.Start();

	gQuickStart.Start();
}

//等待
void Work::Join()
{
	gInsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();

	gQuickStart.Join();

}

//停止
void Work::Stop()
{
	//m_centerClient.reset();

	m_gateInfo.clear();

	gInsideNet.Stop();

	LRunnable::Stop();

	gRLT.stop();

	m_gcService.Stop();

	gQuickStart.Stop();
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
		EraseExpiredLogics(cur);
		
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		SendGateInfoToCenter();
		checkAndTouchLogic();
		
	
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
	}

	/*if(m_centerClient)
		m_centerClient->OnTick(cur);*/
	
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

	
	/************************************************************/
		//////////////////////////////////////////////////////////////////////////
		//logic 跟 logicmanager之间的交互
	case MSG_L_2_LMG_LOGIN:
		HanderLogicLogin((LMsgL2LMGLogin*)msg);
		break;
	

		//////////////////////////////////////////////////////////////////////////
		//gate 跟 logicmanager之间的交互
	case MSG_G_2_SERVER_LOGIN:
		HanderGateLogin((LMsgG2ServerLogin*)msg);
		break;
	case MSG_G_2_LOGIC_MANAGER_USER_MSG:
		{
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if (gate)
			{
				HanderGateUserMsg((LMsgG2LMUserMsg*)msg,gate);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate not exiext");
			}
		}
		break;
	case MSG_G_2_L_USER_OUT_MSG:
		{
			LLOG_DEBUG("Work::HanderMsg()  MSG_G_2_L_USER_OUT_MSG");
			LMsgG2LUserOutMsg* gmsg = (LMsgG2LUserOutMsg*)msg;
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if (gate)
			{
				HanderUserLogout(gate->m_id,gmsg->m_userGateId);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate not exiext");
			}
		}
		break;

	case MSG_S_2_S_KEEP_ALIVE_ACK:
		HanderKeepAliveAck((LMsgS2SKeepAliveAck*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE:
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);
		break;


		//新增加的
	case MSG_GAME_2_LM_LOGIN:
		HanderGame2LMLogin((LMsgGame2LMGLogin*)msg);
		break;
	case MSG_LM_2_LM_QUICK_CREATE_ROOM:
		HanderLM2LMQuickCreateRoom((LMsgLM2LMQuckCreateRoom*)msg);
		break;
	case MSG_L_2_LM_QUICK_CREATE_ROOM_OPT:
		HanderL2LMQuickCreateRoomOpt((LMsgL2LMQuickCreateRoomOpt*)msg);
		break;
	default:
		LLOG_DEBUG("Unknown message id: %d", msg->m_msgId);
		break;
	}
}

void Work::HanderUserLogin(Lint id,Lint seed,Lint gateId,Llong gateUserId,Lstring& md5,Lstring& ip, const Lstring &location)
{

}
// 处理玩家离线， 如果状态为在游戏中，并且Logic存在，则不删除；否则删除user
void Work::HanderUserLogout(Lint gateId, Llong gateUserId)
{
	
}

void Work::HanderUserKick(LMsgKick* msg)
{
	/*if (msg->m_sp == m_centerClient)
	{
		HanderCenterLogout();
	}
	
	else>*/
	{
		//WARNING：这里好像有问题，如果同时在线人数过多，这里会不停的遍历
		HanderLogicLogout(msg);
		
		HanderGateLogout(msg);

		HanderGameLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	//SocketPtrManager::Instance().AddSocketPtr(msg->m_sp);

	/*if(m_centerClient == msg->m_sp)
	{
		LLOG_ERROR("Connecting center on %s:%d succeed", m_centerClient->GetServerIP().c_str(), m_centerClient->GetServerPort());
		SendLoginToCenter();
		SendGateInfoToCenter();
	}*/
	
}

static char* MSDK_PATH = "/cgi-bin/sdk_pay";

static char* ADMIN_PATH = "/cgi-bin/admin";

static void SendRet(Lstring errorInfo, LSocketPtr send)
{
	char strOK[512] = { 0 };

	std::string strSendBuf;
	strSendBuf.reserve(1024);
	strSendBuf += "HTTP/1.1 200 OK\r\n";

	sprintf_s(strOK, sizeof(strOK)-1, "Content-Length:%d\r\n", (Lint)errorInfo.size());
	strSendBuf += strOK;
	strSendBuf += "Content-Type: text/html; charset=utf-8\r\n\r\n";

	memset(strOK,0,sizeof(strOK));
	sprintf_s(strOK, sizeof(strOK)-1, "%s", errorInfo.c_str());
	strSendBuf += strOK;

	LBuffPtr buff(new LBuff());
	buff->Write(strSendBuf.c_str(), strSendBuf.size());
	send->Send(buff);
	send->Kick();
}
void Work::HanderHttp(LMsgHttp* msg)
{	
}

void Work::ConnectToCenter()
{
	/*KeepAliveSetting setting = {true, 15000, 60000};
	m_centerClient.reset(new TCPClient(gInsideNet, gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort(), setting));
	LLOG_ERROR("Work::ConnectCenter begin %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	bool succeed = m_centerClient->Connect();
	if(succeed)
	{
		LLOG_ERROR("Work::ConnectCenter end %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	}
	else
	{
		LLOG_ERROR("Since failed to connect center on %s:%d, stopping...", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
		Sleep(1000);
		Stop();
	}*/
}

void Work::SendToCenter(LMsg& msg)
{
	/*if(m_centerClient)
	{
		m_centerClient->Send(msg.GetSendBuff());
	}*/
}

//发送网关信息到center
void Work::SendGateInfoToCenter()
{
	/*LMsgL2CeGateInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;
	auto it = m_gateInfo.begin();
	for (; it != m_gateInfo.end(); ++it)
	{
		//it->second.m_userCount = gUserManager.GetGatUserCount(it->first);
		//info.m_gate[info.m_count++] = (it->second);
	}

	//gUserManager.GetGatUserCountByAppID(info.m_userCountMap);

	info.m_AppCount = info.m_userCountMap.size();

	if (m_centerClient)
	{
		m_centerClient->Send(info.GetSendBuff());
	}

	*/
}

void Work::SendLogicInfoToGates(Lint nGateID)
{
	LMsgLMG2GateLogicInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		info.m_logic[info.m_count++] = (it->second);
	}
	if (nGateID > 0)
	{
		SendMessageToGate(nGateID, info);
	}
	else
	{
		SendMessageToAllGate(info);
	}
}










UserLoginInfo*  Work::GetUserLoginInfo(Lint userId)
{
	if (m_reqLoginUser.count(userId))
	{
		return &m_reqLoginUser[userId];
	}
	return NULL;
}


void Work::DelUserLoginInfo(Lint userId)
{
	if (m_reqLoginUser.count(userId))
	{
		m_reqLoginUser.erase(userId);
	}
}



void Work::HanderCenterLogout()
{
	LLOG_ERROR("warning:lose connection with center server!!!");
	//等待重连
}

void Work::HanderDBServerLogout()
{
	LLOG_ERROR("warning DB server lose connect!!!!!!");
	//等待重来
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
				LLOG_ERROR("Work::HanderGateLogin delete already existed gate %d", msg->m_id);
			}
			else
				return;
		}
	}

	LLOG_ERROR("Work::HanderGateLogin Succeed! %d", msg->m_id);

	//广播GATE消息
	GateInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_userCount = 0;
	info.m_sp = msg->m_sp;
	m_gateInfo[info.m_id] = info;

	//同步Logic消息
	SendLogicInfoToGates(msg->m_id);
	SendGameServerInfoToGates();
	
	SendGateInfoToCenter();
}

void Work::HanderGateLogout(LMsgKick* msg)
{
	GateInfo* info = GetGateInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR("Work::HanderGateLogout Succeed! %d", info->m_id);
		DelGateInfo(info->m_id);
		SendGateInfoToCenter();
	}
}

GateInfo* Work::GetGateInfoBySp(LSocketPtr sp)
{
	auto it = m_gateInfo.begin();
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


void Work::HanderGateUserMsg(LMsgG2LMUserMsg* msg,GateInfo* gate)
{
	
	Lint gateId = gate->m_id;

	if (msg->m_userMsg)
	{
		switch (msg->m_userMsgId)
		{

		case MSG_C_2_S_LM_LOGIN:
			HanderUserLogin(msg->m_sp, (LMsgC2SLMLogin*)msg->m_userMsg);
			break;
		case MSG_C_2_S_QUICK_ROOM_OPT:
			HanderC2SQuickRoomOpt(msg->m_sp,(LMsgC2SQuickRoomOpt*)msg->m_userMsg);
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



void Work::HanderUserReLogin(Lint id,Lint seed,Lint gateId,Llong gateUserId,Lstring& md5,Lstring& ip)
{
	/*User* user = gUserManager.GetUserbyDataId(id);
	if (user)
	{
		LMsgL2CeUerReLogin msg;
		msg.m_ID = gConfig.GetServerID();
		msg.m_userid = id;
		msg.m_seed = seed;
		msg.m_md5 = md5;
		msg.m_gateId = gateId;
		msg.m_gateUserId = gateUserId;
		msg.m_ip = ip;
		msg.m_uuid = user->m_userData.m_unioid;
		SendToCenter(msg);
	}*/
}



void Work::HanderLogicLogin(LMsgL2LMGLogin* msg)
{
	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderLogicLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	//广播Logic消息
	LLOG_ERROR("Work::HanderLogicLogin Succeed! %d", msg->m_id);
	LogicInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_sp = msg->m_sp;
	info.m_exclusive_play = msg->m_exclusive_play;
	m_logicInfo[info.m_id] = info;

	SendLogicInfoToGates();
	
}



void Work::HanderLogicLogout(LMsgKick* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LogicInfo* info = GetLogicInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR("Work::HanderLogicLogout Succeed! %d", info->m_id);
		gRoomManager.RecycleDumpServer(info->m_id);
		DelLogicInfo(info->m_id);
		SendLogicInfoToGates();
		
	}
}



LogicInfo* Work::GetLogicInfoBySp(LSocketPtr sp)
{
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}



LogicInfo* Work::GetLogicInfoById(Lint id)
{
	if (m_logicInfo.count(id))
		return &m_logicInfo[id];

	return NULL;
}



LogicInfo* Work::GetLogicInfoByPresure()
{
	Lint nMiniCouunt = 100000;
	LogicInfo* logic = NULL;
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		if (it->second.m_deskCount < nMiniCouunt )		//分配策略
		{
			nMiniCouunt = it->second.m_deskCount;
			logic = &it->second;
		}
	}
	return logic;
}


LogicInfo* Work::GetLogicInfoByGameType(Lint game_type)
{
	LogicInfo* logic = NULL;

	if ((game_type == LBase_Jing_Dian_Dou_Di_Zhu|| game_type == LBase_Wei_Fang_Pu_Ke))
	{
		int nExclusive_play = int(EP_DOU_DI_ZHU);
		Lint nMiniCouunt = 100000;

		for (auto it = m_logicInfo.begin(); it != m_logicInfo.end(); ++it)
		{	
			if (it->second.m_deskCount < nMiniCouunt 
				&& nExclusive_play == it->second.m_exclusive_play	//分配专属玩法的服务器
				&& -1!=it->second.m_flag)		//分配策略
			{
				nMiniCouunt = it->second.m_deskCount;
				logic = &it->second;
			}
		}
	}
	else
	{
		logic = GetLogicInfoByPresure();
	}

	return logic;
}


void Work::DelLogicInfo(Lint id)
{
	auto iter = m_logicInfo.find(id);
	if (iter != m_logicInfo.end())
	{
		m_logicInfo.erase(iter);
	}
}



void Work::SendMessageToAllLogic(LMsg& msg)
{
	auto logic = m_logicInfo.begin();
	for (; logic != m_logicInfo.end(); ++logic)
	{
		logic->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToAllGate(LMsg& msg)
{
	auto gate = m_gateInfo.begin();
	for (; gate != m_gateInfo.end(); ++gate)
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToLogic(Lint logicID, LMsg& msg)
{
	auto logic = m_logicInfo.find(logicID);
	if (logic != m_logicInfo.end())
	{
		logic->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToGate(Lint gateID, LMsg& msg)
{
	auto gate = m_gateInfo.find(gateID);
	if ( gate != m_gateInfo.end())
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}


void Work::checkAndTouchLogic()
{
	
	
}
void Work::SendLoginToCenter()
{
	/*LMsgL2CeLogin login;
	login.m_ID = gConfig.GetServerID();
	login.m_key = gConfig.GetCenterKey();
	login.m_ServerName = gConfig.GetServerName();
	login.m_NeedLoadUsers = gUserManager.NeedLoadUsers();
	m_centerClient->Send(login.GetSendBuff());
	LLOG_ERROR("Login centerserver succeed.");*/
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

	/*if(m_centerClient == msg->m_sp)
		m_centerClient->OnKeepAliveAck();*/

}



void Work::EraseExpiredLogics(LTime& cur)
{
	LLOG_DEBUG("Enter Work::EraseExpiredLogics");
	bool logicUpdated = false;
	for(auto iter = m_toEraseLogics.begin(); iter != m_toEraseLogics.end();)
	{
		if(cur.Secs() - iter->second > 120)
		{
			LLOG_ERROR("During 120 seconds, logic server %d was not reconnected with logicmanager, so clear desk infos on it", iter->first);
			gRoomManager.RecycleDumpServer(iter->first);
			DelLogicInfo(iter->first);
			logicUpdated = true;
			m_toEraseLogics.erase(iter++);
		}
		else
			++iter;
	}
	if(logicUpdated)
		SendLogicInfoToGates();

	LLOG_DEBUG("Leave Work::EraseExpiredLogics");
}




void Work::OnUserLogin(User *user)
{
	
}







//分割線
void Work::HanderGame2LMLogin(LMsgGame2LMGLogin* msg)
{
	if (nullptr == msg)
		return;

	//广播Logic消息
	LLOG_ERROR("Work::HanderGameLogin Succeed! %d", msg->m_id);
	GameServerInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_sp = msg->m_sp;
	
	m_game_server_info[info.m_id] = info;

	SendGameServerInfoToGates();
}

void Work::SendGameServerInfoToGates(Lint GateID)
{
	LMsgLMG2GateGameServerInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;
	auto it = m_game_server_info.begin();
	for (; it != m_game_server_info.end(); ++it)
	{
		info.m_logic[info.m_count++] = (it->second);
	}

	if (GateID == 0)
	{
		SendMessageToAllGate(info);
	}
	else
	{
		SendMessageToGate(GateID,info);
	}
	

	LLOG_ERROR("Work::SendGameServerInfoToGates Succeed! %d", m_game_server_info.size());
}

GameServerInfo* Work::GetGameInfoBySp(LSocketPtr sp)
{
	auto it = m_game_server_info.begin();
	for (; it != m_game_server_info.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}

void Work::DelGameInfo(Lint id)
{
	auto it = m_game_server_info.find(id);
	if (it != m_game_server_info.end())
		m_game_server_info.erase(it);
}

void Work::HanderGameLogout(LMsgKick* msg)
{
	if (msg == NULL)
	{
		return;
	}
	GameServerInfo* info = GetGameInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR("Work::HanderGameLogout Succeed! %d", info->m_id);
		
		DelGameInfo(info->m_id);
		SendGameServerInfoToGates();

	}
}


/*void Work::HanderDeskOpt(LMsgC2SDeskOpt* msg)
{
	Lint user_id = msg->m_user_id;

	if (msg->m_type == LMsgC2SDeskOpt::TYPE_CREATE_DESK)
	{
		int desk_id = gDeskManager.GetUserDeskID(user_id);
		if (desk_id == 0)
		{
			desk_id = gDeskManager.GetFreeDeskId();
			if (desk_id == 0)
			{
				LLOG_ERROR("Desk List is Empty!");
				return;
			}


			LogicInfo* logic = GetLogicInfoByPresure();
			if (nullptr == logic)
				return;

			LMsgLM2LDeskOpt	 send;
			send.m_user_id = user_id;
			send.m_type = msg->m_type;
			send.m_desk_id = desk_id;
			send.m_desk_type = msg->m_desk_type;
			send.m_cost = msg->m_cost;
			send.m_self.m_user_id = msg->m_user_id;
			send.m_self.m_name = msg->m_name;
			send.m_self.m_head_icon = msg->m_head_icon;

			logic->m_sp->Send(send.GetSendBuff());

		}
		else
		{
			LMsgS2CDeskOpt send;
			send.m_errorCode = 1;

			SendMessageToUser(msg->m_sp, user_id, send);
		}
	}
	else if (msg->m_type == LMsgC2SDeskOpt::TYPE_ADD_TO_DESK)
	{
		int desk_id = gDeskManager.GetUserDeskID(user_id);
		if (desk_id == 0)
		{
			desk_id = msg->m_desk_id;

			DeskPtr desk = gDeskManager.GetDesk(desk_id);

			if (desk)
			{
				LogicInfo* logic = GetLogicInfoById(desk->m_logic_server_id);
				if (logic)
				{
					LMsgLM2LDeskOpt send;
					send.m_user_id = msg->m_user_id;
					send.m_type = msg->m_type;
					send.m_desk_id = msg->m_desk_id;
					send.m_self.m_user_id = msg->m_user_id;
					send.m_self.m_name = msg->m_name;
					send.m_self.m_head_icon = msg->m_head_icon;

					logic->m_sp->Send(send.GetSendBuff());
				}
			}
		}
		else
		{
			LMsgS2CDeskOpt send;
			send.m_errorCode = 1;

			SendMessageToUser(msg->m_sp, user_id, send);
		}
	}
	else if (msg->m_type == LMsgC2SDeskOpt::TYPE_QUIT_DESK)
	{
		if (gDeskManager.GetUserDeskID(user_id) == msg->m_desk_id)
		{
			DeskPtr desk = gDeskManager.GetDesk(msg->m_desk_id);

			if (desk)
			{
				LogicInfo* logic = GetLogicInfoById(desk->m_logic_server_id);
				if (logic)
				{
					LMsgLM2LDeskOpt send;
					send.m_user_id = msg->m_user_id;
					send.m_type = msg->m_type;
					send.m_desk_id = msg->m_desk_id;
					
					

					logic->m_sp->Send(send.GetSendBuff());
				}
			}
		}
		else
		{
			LMsgS2CDeskOpt send;
			send.m_errorCode = 2;

			SendMessageToUser(msg->m_sp, user_id, send);
		}
	}
}*/

/*void Work::HanderDeskOptRespon(LMsgL2LMDeskOpt* msg)
{
	LMsgS2CDeskOpt send;
	send.m_errorCode = msg->m_result;
	send.m_user_id = msg->m_user_id;
	send.m_type = msg->m_type;
	Lint user_id = msg->m_user_id;

	if (msg->m_type == LMsgC2SDeskOpt::TYPE_CREATE_DESK)
	{
		if (msg->m_result)
		{
			gDeskManager.RecycleDeskID(msg->m_desk_id);
		}
		else
		{
			DeskPtr desk = gDeskManager.CreateDesk(msg->m_desk_id);
			if (desk == nullptr)
			{
				gDeskManager.RecycleDeskID(msg->m_desk_id);
			}

			LogicInfo* logic = GetLogicInfoBySp(msg->m_sp);
			if (logic == nullptr)
				return;

			logic->m_deskCount++;
			desk->m_logic_server_id = logic->m_id;

			gDeskManager.AddUserToDesk(msg->m_desk_id, user_id);

			FillDeskMsg(send.m_desk, msg);

			msgpack::sbuffer buffer;
			msgpack::packer<msgpack::sbuffer> pac(&buffer);
			send.Write(pac);

			msgpack::unpacked  unpack;
			msgpack::unpack(&unpack, buffer.data(), buffer.size());

			msgpack::object  obj = unpack.get();

			cout << obj << endl;


			ModifyUserStatus(user_id, US_DESK, desk->m_logic_server_id);
		}
	}
	//加入桌子
	else if (msg->m_type == LMsgC2SDeskOpt::TYPE_ADD_TO_DESK)
	{
		if (msg->m_result == 0)
		{
			gDeskManager.AddUserToDesk(msg->m_desk_id, user_id);

			send.m_desk.m_desk_id = msg->m_desk_id;
			send.m_desk.m_desk_type = msg->m_desk_type;
			send.m_desk.m_cost = msg->m_cost;

			DeskPtr desk = gDeskManager.GetDesk(msg->m_desk_id);
			if (nullptr == desk)
				return;

			FillDeskMsg(send.m_desk, msg);

			ModifyUserStatus(user_id, US_DESK, desk->m_logic_server_id);
		}
		
	}
	else if(msg->m_type == LMsgC2SDeskOpt::TYPE_QUIT_DESK)
	{
		if (msg->m_result == 0)
		{
			gDeskManager.DelUserToDesk(msg->m_desk_id, user_id);
			if (msg->m_del_desk)
			{
				gDeskManager.DeleteDesk(msg->m_desk_id);
			}

			ModifyUserStatus(user_id, US_CENTER, 0);
		}
	}

	LSocketPtr sp = GetGateSpByUserId(user_id);
	if (sp)
	{
		SendMessageToUser(sp, user_id, send);
	}
}*/


/*void Work::FillDeskMsg(DeskMsg& send, LMsgL2LMDeskOpt* msg)
{
	send.m_desk_id = msg->m_desk_id;
	send.m_desk_type = msg->m_desk_type;
	send.m_cost = msg->m_cost;

	for (int i = 0; i < msg->m_users.size(); ++i)
	{
		send.m_users.push_back(msg->m_users[i]);
	}
}*/

void Work::HanderC2SQuickRoomOpt(LSocketPtr sp,LMsgC2SQuickRoomOpt* msg)
{
	if (sp == nullptr)
		return;

	LUserPtr user = gUserManager.GetUserById(msg->m_user_id);
	if (user == nullptr)
		return;

	LMsgS2CQuickRoomOpt send;
	send.m_result = RT_SUCCEED;
	send.m_type = msg->m_type;
	send.m_user_id = msg->m_user_id;

	if (msg->m_type == 0)
	{
		if (user->GetRoomId() || user->IsQuickStart())
			return;


		gQuickStart.PushUser(msg->m_user_id);
			
		//测试专用
		for (int i = 0; i < 8; ++i)
		{
			int user_id = 1000 + i;

			gQuickStart.PushUser(user_id);
		}
	}
	else
	{
		if (user->GetRoomId() || !user->IsQuickStart())
			return;

		if (gQuickStart.RemoveUser(msg->m_user_id))
		{
			user->SetQuickStartStatus(false);
		}
		else
		{
			return;
		}
	}

	SendMessageToUser(sp, msg->m_user_id, send);
}


void Work::SendMessageToUser(LSocketPtr sp, Lint user_id,LMsg& msg)
{
	GateInfo* info =	GetGateInfoBySp(sp);
	if (nullptr != info)
	{
		LMsgLM2GUserMsg sendMsg;
		sendMsg.m_user_id = user_id;
		sendMsg.m_user_msg_id = msg.m_msgId;
		sendMsg.m_dataBuff = msg.GetSendBuff();

		info->m_sp->Send(sendMsg.GetSendBuff());
	}
}

void Work::HanderUserLogin(LSocketPtr sp, LMsgC2SLMLogin* msg)
{
	if (sp == nullptr || msg == nullptr)
		return;

	int user_id = msg->m_user_id;
	LUserPtr user = gUserManager.GetUserById(user_id);
	if (user == nullptr)
	{
		user = std::make_shared<User>(user_id);
		gUserManager.AddUser(user);
	}

	user->SetUserSp(sp);
	

	LMsgS2CLMLogin send;
	send.m_user_id = user_id;

	if (sp)
	{
		SendMessageToUser(sp, user_id, send);
	}
}

LSocketPtr Work::GetGateSpByUserId(int user_id)
{
	auto it = m_users_gate_sp.find(user_id);
	if (it != m_users_gate_sp.end())
	{
		return it->second;
	}

	return nullptr;
}

void Work::ModifyUserStatus(int user_id, USER_STATUS status, int logic_server_id)
{
	LMsgLM2GUserStatusModify send;
	send.m_user_id = user_id;
	send.m_status = status;
	send.m_logic_server_id = logic_server_id;

	LSocketPtr sp = GetGateSpByUserId(user_id);
	if (sp)
	{
		SendMessageToUser(sp, user_id, send);
	}
}

void Work::HanderLM2LMQuickCreateRoom(LMsgLM2LMQuckCreateRoom* msg)
{
	if (nullptr == msg)
		return;

	
	int room_id = gRoomManager.GetFreeRoomId();
	if (room_id == 0)
	{
		LLOG_ERROR("Room List is Empty!");
		return;
	}

	LMsgLM2LQuickCreateRoomOpt send;
	send.m_room_id = room_id;
	//在这里先加入把玩家先加入房间
	for (int i = 0; i < msg->m_users.size(); ++i)
	{
		int user_id = msg->m_users[i];
		MsgUserInfo msgUser;
		msgUser.m_user_id = user_id;
		LUserPtr user = gUserManager.GetUserById(user_id);
		
		if (user)
		{
			user->SetRoomId(room_id);
			user->SetQuickStartStatus(false);



			msgUser.m_name = user->GetName();
			msgUser.m_head_icon = user->GetHeadIcon();

			GateInfo* pGate = GetGateInfoBySp(user->GetUserSp());
			if (pGate == nullptr)
			{
				msgUser.m_robot = true;
			}
			else
			{
				msgUser.m_gate_id = pGate->m_id;
				msgUser.m_robot = false;
			}
		}
		else
		{
			msgUser.m_robot = true;
			msgUser.m_name = "haha";
			//msgUser.m_head_icon = user->GetHeadIcon();
		}
		send.m_users.push_back(msgUser);
	}


	LogicInfo* logic = GetLogicInfoByPresure();
	if (nullptr == logic)
		return;


	logic->m_sp->Send(send.GetSendBuff());

	return;
}

void Work::HanderL2LMQuickCreateRoomOpt(LMsgL2LMQuickCreateRoomOpt* msg)
{
	int logic_server_id = GetLogicInfoBySp(msg->m_sp)->m_id;


	LMsgS2CQuickRoomResult send;
	send.m_result = msg->m_result;
	
	if (msg->m_result == RT_SUCCEED)
	{
		send.m_room.m_room_id = msg->m_room_id;

		for (int i = 0; i < msg->m_users.size(); ++i)
		{
			send.m_room.m_users.push_back(msg->m_users[i]);
		}
	}



	for (int i = 0; i < msg->m_users.size(); ++i)
	{
		int user_id = msg->m_users[i].m_user_id;
		LUserPtr user = gUserManager.GetUserById(user_id);
		if(user == nullptr)
			continue;

		send.m_user_id = user_id;

	/*	msgpack::sbuffer buffer;
		msgpack::packer<msgpack::sbuffer> pac(&buffer);
		send.Write(pac);

		msgpack::unpacked  unpack;
		msgpack::unpack(&unpack, buffer.data(), buffer.size());

		msgpack::object  obj = unpack.get();
		cout << obj;*/

		

		

		LMsgLM2GUserStatusModify modify;
		modify.m_user_id = user_id;
		modify.m_status = US_ROOM;
		modify.m_logic_server_id = logic_server_id;

		LSocketPtr sp = user->GetUserSp();
		if (sp)
		{
			sp->Send(modify.GetSendBuff());

			

			SendMessageToUser(sp, user_id, send);
		}
	}
}


