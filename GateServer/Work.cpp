#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "OutsideNet.h"
#include "LMsgS2S.h"
#include "GateUser.h"
#include "BaseCommonFunc.h"
#include "LTool.h"

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("GateServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gOutsideNet.Init())
	{
		LLOG_ERROR("gOutsideNet.Init error");
		return false;
	}

	if (!gGateUserManager.Init())
	{
		LLOG_ERROR("gGateUserManager.Init error");
		return false;
	}

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	ConnectToLogicManager();
	

	gOutsideNet.Start();

	LRunnable::Start();

	m_gcService.Start();
}

//等待
void Work::Join()
{
	gOutsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	m_spMap.clear();

	m_logicInfo.clear();

	gOutsideNet.Stop();

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
			LLOG_ERROR("Pop MsgId = %d", msg->m_msgId);
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
		CheckMsgFrequency(5);
	}
	
	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();

		auto it = m_spMap.begin();
		for (; it != m_spMap.end();)
		{
			if (GetCurTime().Secs() - it->second.timestamp > 60)
			{
				/*LMsgKick kick;
				kick.m_sp = it->first;
				it->first->Stop();
				m_spMap.erase(it++);

				HanderUserKick(&kick);
				LLOG_DEBUG("Out Time Big Than 60 Kick User");*/
				++it;
			}
			else
			{
				++it;
			}
		}
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
	}

	if(m_logicManager)
		m_logicManager->OnTick(cur);

	
	
	for(auto iter = m_logicInfo.begin(); iter != m_logicInfo.end(); iter++)
	{
		if(iter->second.m_client)
			iter->second.m_client->OnTick(cur);
	}

	for (auto iter = m_game_server_info.begin(); iter != m_game_server_info.end(); iter++)
	{
		if (iter->second.m_client)
			iter->second.m_client->OnTick(cur);
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//LLOG_ERROR("Work::HanderMsg received messageID %d", msg->m_msgId);
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
	case MSG_LMG_2_G_SYNC_LOGIC:
		HanderLogicInfo((LMsgLMG2GateLogicInfo*) msg);
		break;
	case MSG_LMG_2_G_SYNC_GAME_SERVER:
		HanderGameServerInfo((LMsgLMG2GateGameServerInfo*)msg);
		break;
	case MSG_G_2_GAME_USER_MSG:
		HanderUser2GameServerMsg((LMsgG2GameUserMsg*)msg);
	break;
	case MSG_GAME_2_G_USER_MSG:
		HanderGameServer2UserMsg((LMsgGame2GUserMsg*)msg);
	break;
	case MSG_G_2_L_USER_MSG:
		HanderUser2LogicServerMsg((LMsgG2LUserMsg*)msg);
	break;
	case MSG_L_2_G_USER_MSG:
		HanderLogicServer2UserMsg((LMsgL2GUserMsg*)msg);
	break;
	case MSG_G_2_LOGIC_MANAGER_USER_MSG:
		HanderUser2LogicManagerServerMsg((LMsgG2LMUserMsg*)msg);
	break;
	case MSG_LOGIC_MANAGER_2_G_USER_MSG:
		HanderLogicManagerServer2UserMsg((LMsgLM2GUserMsg*)msg);
	break;
	case MSG_LM_2_G_USER_STATUS_MODIFY:
		HanderLM2GUserStatusModify((LMsgLM2GUserStatusModify*)msg);
	break;
	
	case MSG_L_2_G_USER_OUT_MSG:
		{
			LMsgL2GUserOutMsg*lmsg = (LMsgL2GUserOutMsg*)msg;
			GUserPtr user = gGateUserManager.GetUserById(lmsg->m_id);
			if (user)
			{
				user->m_sp->Stop();
				DelSp(user->m_sp);
				
				gGateUserManager.DelUser(user);
			}
			else
			{
				LLOG_DEBUG("ERROR: Work::HanderMsg MSG_L_2_G_USER_OUT_MSG user not exiest %d", lmsg->m_id);
			}
		}
		break;


	case MSG_S_2_S_KEEP_ALIVE_ACK:
		HanderKeepAliveAck((LMsgS2SKeepAliveAck*)msg);
		break;
	default:
		break;
	}
}

void Work::AddSp(LSocketPtr sp)
{
	auto iter = m_spMap.find(sp);
	if(iter != m_spMap.end())
	{
		iter->second.timestamp = GetCurTime().Secs();
		iter->second.msgCount++;
	}
	else
	{
		ClientContext context = {GetCurTime().Secs(), 0};
		m_spMap[sp] = context;
	}
}

void Work::DelSp(LSocketPtr sp)
{
	auto iter = m_spMap.find(sp);
	if (iter != m_spMap.end())
	{
		m_spMap.erase(iter);
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if(m_logicManager == msg->m_sp || IsGameServerSP(msg->m_sp) || IsLogicSP(msg->m_sp))
		return;


	//玩家离线
	GUserPtr user = gGateUserManager.GetUserBySp(msg->m_sp);
	if (user)
	{
		if(user->m_online)
			SendToLogicUserOut(user);

		gGateUserManager.DelUser(user);
	}
	//以后恢复
	DelSp(msg->m_sp);
	LLOG_ERROR("kick user  succeed");
	
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if (msg->m_sp == m_logicManager)
	{
		LLOG_ERROR("Connecting logicmanager on %s:%d succeed", m_logicManager->GetServerIP().c_str(), m_logicManager->GetServerPort());
		SendGateLoginToLM();
		return;
	}
	
	auto logicClient = GetLogicClientBySP(msg->m_sp);
	if(logicClient)
	{
		LLOG_ERROR("Connecting logic on %s:%d succeed", logicClient->GetServerIP().c_str(), logicClient->GetServerPort());
		SendGateLoginToLogic(logicClient);
		return;
	}
	
	auto gameClinet = GetGameServerClientBySP(msg->m_sp);
	if (gameClinet)
	{
		LLOG_ERROR("Connecting GameServer on %s:%d succeed", gameClinet->GetServerIP().c_str(), gameClinet->GetServerPort());
		SendGateLoginToGameServer(gameClinet);
		return;
	}

	AddSp(msg->m_sp);

	LLOG_ERROR("Connecting Client succeed");

	GUserPtr info = gGateUserManager.GetUserBySp(msg->m_sp);
	if (info != NULL)
	{
		LLOG_DEBUG("Work::HanderClientIn already in ");
		return;
	}

	//这里添加一个新连接进来
	gGateUserManager.CreateUser(msg->m_sp);
}

void Work::HanderHttp(LMsgHttp* msg)
{	
}

void Work::ConnectToLogicManager()
{
	KeepAliveSetting setting = {true, 15000, 5000};
	MsgPackReceiver receiver = [this](LBuffPtr recv, LSocketPtr s)
								{
									LMsg* msg = ::ParseInsideNetPacket(recv, s);
									if(msg)
										this->Push(msg);
								};

	m_logicManager.reset(new TCPClient(gOutsideNet, gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort(), setting, receiver));
	bool succeed = m_logicManager->Connect();
	if(!succeed)
		LLOG_ERROR("Failed to connect with logicmanager.");
}



void Work::SendToLogicUserOut(GUserPtr user)
{
	if (user == nullptr)
		return;

	LMsgG2SUserLogOut send;
	send.m_user_id = user->m_user_id;
	if (m_logicManager)
	{
		//通各LogicManager下线
		m_logicManager->Send(send.GetSendBuff());
	}
	GameServerInfo* server = GetGameServerById(user);
	if (server != NULL && server->m_client)
	{
		//通知Logic下线
		server->m_client->Send(send.GetSendBuff());
	}
}

void Work::HanderLogicInfo(LMsgLMG2GateLogicInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderGameServerInfo %d", msg->m_ID);
		return;
	}

	std::map<int, LogicInfo> mapFromLM;
	for(int i = 0; i < msg->m_count; i++)
	{
		mapFromLM[msg->m_logic[i].m_id] = msg->m_logic[i];
	}

	//遍历哪些logic已经不在logicmanager上了
	for(auto itLogic = m_logicInfo.begin(); itLogic != m_logicInfo.end();)
	{
		if(mapFromLM.find(itLogic->first) == mapFromLM.end()) //这个logic已经不在logicmananger上了
		{
			LLOG_ERROR("Logic %d does not exist, so delete it", itLogic->first);
			itLogic->second.m_client->Disconnect();
			itLogic = m_logicInfo.erase(itLogic);
		}
		else //如果依然存在
		{
			mapFromLM.erase(itLogic->first);
			itLogic++;
		}
	}

	//加入新进入的
	for(auto itNew = mapFromLM.begin(); itNew != mapFromLM.end(); itNew++)
	{
		m_logicInfo[itNew->first] = itNew->second;
		KeepAliveSetting setting = {true, 15000, 5000};
		MsgPackReceiver receiver = [this](LBuffPtr recv, LSocketPtr s)
									{
										LMsg* msg = ::ParseInsideNetPacket(recv, s);
										if(msg)
											this->Push(msg);
									};

		m_logicInfo[itNew->first].m_client.reset(new TCPClient(gOutsideNet, itNew->second.m_ip, itNew->second.m_port, setting, receiver));
		m_logicInfo[itNew->first].m_client->Connect();
	}
}

void Work::HanderGameServerInfo(LMsgLMG2GateGameServerInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}


	LLOG_ERROR("Work::HanderGameServerInfo %d", msg->m_ID);

	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderGameServerInfo %d", msg->m_ID);
		return;
	}

	std::map<int, GameServerInfo> mapFromLM;
	for (int i = 0; i < msg->m_count; i++)
	{
		mapFromLM[msg->m_logic[i].m_id] = msg->m_logic[i];
	}

	LLOG_ERROR("Work::HanderGameServerInfo mapFromLM size:%d", mapFromLM.size());
	
	for (auto itLogic = m_game_server_info.begin(); itLogic != m_game_server_info.end();)
	{
		if (mapFromLM.find(itLogic->first) == mapFromLM.end()) //这个logic已经不在logicmananger上了
		{
			LLOG_ERROR("GameServer %d does not exist, so delete it", itLogic->first);
			itLogic->second.m_client->Disconnect();
			itLogic = m_game_server_info.erase(itLogic);
		}
		else //如果依然存在
		{
			mapFromLM.erase(itLogic->first);
			itLogic++;
		}
	}

	//加入新进入的
	for (auto itNew = mapFromLM.begin(); itNew != mapFromLM.end(); itNew++)
	{
		m_game_server_info[itNew->first] = itNew->second;
		KeepAliveSetting setting = { true, 15000, 5000 };
		MsgPackReceiver receiver = [this](LBuffPtr recv, LSocketPtr s)
		{
			LMsg* msg = ::ParseInsideNetPacket(recv, s);
			if (msg)
				this->Push(msg);
		};

		m_game_server_info[itNew->first].m_client.reset(new TCPClient(gOutsideNet, itNew->second.m_ip, itNew->second.m_port, setting, receiver));
		m_game_server_info[itNew->first].m_client->Connect();

		LLOG_ERROR("Work::HanderGameServerInfo Succeed ");
	}
}


void Work::SendMessageToAllUse(LMsg* msg,Lint appId)
{
	if (msg == NULL)
	{
		return;
	}
	

	auto it = m_spMap.begin();
	for (; it != m_spMap.end(); ++ it)
	{
		it->first->Send(msg->GetSendBuff());
	}
}

LogicInfo* Work::GetLogicInfoBySp(LSocketPtr sp)
{
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		if (sp == it->second.m_client)
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



TCPClientPtr Work::GetLogicClientBySP(LSocketPtr sp)
{
	TCPClientPtr client;
	for(auto itSp = m_logicInfo.begin(); itSp != m_logicInfo.end(); itSp++)
	{
		if(itSp->second.m_client == sp)
		{
			client = itSp->second.m_client;
			break;
		}
	}

	return client;
}

bool Work::IsLogicSP(LSocketPtr sp)
{
	for(auto itSp = m_logicInfo.begin(); itSp != m_logicInfo.end(); itSp++)
	{
		if(sp == itSp->second.m_client)
		{
			return true;
		}
	}

	return false;
}



void Work::SendGateLoginToLM()
{
	LMsgG2ServerLogin login;
	login.m_id = gConfig.GetId();
	login.m_ip = gConfig.GetOutsideIp();
	login.m_key = gConfig.GetLogicKey();
	login.m_port = gConfig.GetOutsidePort();
	m_logicManager->Send(login.GetSendBuff());
	LLOG_ERROR("Login LogicManager succeed.");
}

void Work::SendGateLoginToLogic(TCPClientPtr logicClient)
{
	LMsgG2ServerLogin login;
	login.m_id = gConfig.GetId();
	login.m_ip = gConfig.GetOutsideIp();
	login.m_key = gConfig.GetLogicKey();
	login.m_port = gConfig.GetOutsidePort();
	logicClient->Send(login.GetSendBuff());
	LLOG_ERROR("Login logic %s:%d succeed", logicClient->GetServerIP().c_str(), logicClient->GetServerPort());
}

void Work::SendGateLoginToGameServer(TCPClientPtr gameClient)
{
	LMsgG2ServerLogin login;
	login.m_id = gConfig.GetId();
	login.m_ip = gConfig.GetOutsideIp();
	login.m_key = gConfig.GetLogicKey();
	login.m_port = gConfig.GetOutsidePort();
	gameClient->Send(login.GetSendBuff());

	LLOG_ERROR("Login GameServer successfully. %s:%d", gameClient->GetServerIP().c_str(), gameClient->GetServerPort());
}

void Work::HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg)
{
	if(msg == NULL)
		return;

	if (m_logicManager == msg->m_sp)
	{
		m_logicManager->OnKeepAliveAck();
		return;
	}
		
	if (TCPClientPtr logicClient = GetLogicClientBySP(msg->m_sp))
	{
		logicClient->OnKeepAliveAck();
		return;
	}	

	if (TCPClientPtr gameClient = GetGameServerClientBySP(msg->m_sp))
	{
		gameClient->OnKeepAliveAck();
		return;
	}
}

void Work::CheckMsgFrequency(Lint interval)
{
	Lint threshold = gConfig.GetMsgFrequency() * interval;

	for(auto iter = m_spMap.begin(); iter != m_spMap.end();)
	{
		LLOG_DEBUG("Client %s:%d's sending frequency is %d message every %d seconds", iter->first->GetRemoteIp().c_str(), iter->first->GetRemotePort(), iter->second.msgCount, interval);
		if(iter->second.msgCount > threshold)
		{
			LLOG_ERROR("client %s:%d's sending frequency is too fast, kick it.", iter->first->GetRemoteIp().c_str(), iter->first->GetRemotePort());
			DisableIP(iter->first->GetRemoteIp());
			LMsgKick kick;
			kick.m_sp = iter->first;
			iter->first->Stop();
			m_spMap.erase(iter++);

			HanderUserKick(&kick);
		}
		else
		{
			iter->second.msgCount = 0;
			iter++;
		}
	}
}






TCPClientPtr	Work::GetGameServerClientBySP(LSocketPtr sp)
{
	TCPClientPtr client;
	auto iter = m_game_server_info.begin();
	for (; iter != m_game_server_info.end(); ++iter)
	{
		if (iter->second.m_client == sp)
		{
			client = iter->second.m_client;
			break;
		}
	}

	return client;
}
bool	Work::IsGameServerSP(LSocketPtr sp)
{
	for (auto itSp = m_game_server_info.begin(); itSp != m_game_server_info.end(); itSp++)
	{
		if (sp == itSp->second.m_client)
		{
			return true;
		}
	}

	return false;
}

GameServerInfo* Work::GetGameServerById(GUserPtr user)
{
	GameServerInfo* game_server = nullptr;

	if (user == nullptr)
		return game_server;

	int user_game_id = user->getUserGameServerID();

	auto it = m_game_server_info.find(user_game_id);
	if (it != m_game_server_info.end())
	{
		game_server = &(it->second);
	}
	else
	{
		game_server = SelectNewGameServer(user->getUserId());
	}

	return game_server;
}


//負載均衡
GameServerInfo*	Work::SelectNewGameServer(Lint uid)
{
	GameServerInfo* game_server = nullptr;

	int index = uid % m_game_server_info.size();
	for (auto it = m_game_server_info.begin(); it != m_game_server_info.end(); ++it)
	{
		if (index == 0)
		{
			game_server = &(it->second);
			break;
		}

		index--;
	}

	return game_server;
}

void Work::SendToGameServer(GUserPtr user, LMsg* msg)
{
	if (nullptr == user || nullptr == msg)
		return;

	GameServerInfo* game_server = GetGameServerById(user);
	if (game_server)
	{
		game_server->m_client->Send(msg->GetSendBuff());
	}
}


void Work::HanderUser2GameServerMsg(LMsgG2GameUserMsg* msg)
{
	
	GUserPtr user = gGateUserManager.GetUserBySp(msg->m_sp);
	if (nullptr == user)
		return;

	if (msg->m_userMsgId == MSG_C_2_S_LOGIN)
	{
		
		msgpack::unpacked  unpack = msgpack::unpack( msg->m_dataBuff->Data() + msg->m_dataBuff->GetOffset(),Lsize( msg->m_dataBuff->Size() - msg->m_dataBuff->GetOffset()));
		msgpack::object obj = unpack.get();
		int user_id = 0;

		ReadMapData(obj, "m_user_id", user_id);

		if (user_id == 0)
		{
			LLOG_DEBUG("ERROR: Message do nto have user_id");
			return;
		}
		
		gGateUserManager.Login(user, user_id);
	}
	
	SendToGameServer(user, msg);
}

void Work::HanderGameServer2UserMsg(LMsgGame2GUserMsg* msg)
{
	//LMsgGame2GUserMsg* userMsg = (LMsgGame2GUserMsg*)msg;
	int user_id = msg->m_user_id;

	GUserPtr user = gGateUserManager.GetUserById(user_id);
	if (nullptr == user)
		return;

	user->m_sp->Send(msg->m_dataBuff);
}


void Work::HanderUser2LogicManagerServerMsg(LMsgG2LMUserMsg* msg)
{

	LLOG_ERROR("HanderUser2LogicManagerServerMsg");
	m_logicManager->Send(msg->GetSendBuff());
}

void Work::HanderLogicManagerServer2UserMsg(LMsgLM2GUserMsg* msg)
{
	int user_id = msg->m_user_id;

	GUserPtr user = gGateUserManager.GetUserById(user_id);
	if (nullptr == user)
		return;

	user->m_sp->Send(msg->m_dataBuff);
}


void Work::HanderUser2LogicServerMsg(LMsgG2LUserMsg* msg)
{
	

	GUserPtr user = gGateUserManager.GetUserBySp(msg->m_sp);
	if (user)
	{
		LogicInfo* logic = GetLogicInfoById(user->m_logicID);
		if (logic)
		{
			logic->m_client->Send(msg->GetSendBuff());
		}
	}
}
void Work::HanderLogicServer2UserMsg(LMsgL2GUserMsg* msg)
{
	int user_id = msg->m_user_id;
	GUserPtr user = gGateUserManager.GetUserById(user_id);
	if (user)
	{
		user->m_sp->Send(msg->m_dataBuff);
	}
}

void Work::HanderLM2GUserStatusModify(LMsgLM2GUserStatusModify* msg)
{
	GUserPtr user = gGateUserManager.GetUserById(msg->m_user_id);
	if (user)
	{
		user->m_status = msg->m_status;
		user->m_logicID = msg->m_logic_server_id;

		LLOG_ERROR("HanderLM2GUserStatusModify users");
	}
}