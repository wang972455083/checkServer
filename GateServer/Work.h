#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "GateUser.h"
#include "LMsgS2S.h"
#include "TCPClient.h"
#include "GarbageCollectionService.h"

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			Tick(LTime& cur);
	void			HanderMsg(LMsg* msg);

	LTime&			GetCurTime();
	
public:
	//处理http消息 
	void			HanderHttp(LMsgHttp* msg);	
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);
	//处理LogicManager同步的Logic消息
	void			HanderLogicInfo(LMsgLMG2GateLogicInfo* msg);
	void			HanderGameServerInfo(LMsgLMG2GateGameServerInfo* msg);

public:
	void			HanderUser2GameServerMsg(LMsgG2GameUserMsg* msg);
	void			HanderGameServer2UserMsg(LMsgGame2GUserMsg* msg);


	void			HanderUser2LogicManagerServerMsg(LMsgG2LMUserMsg* msg);
	void			HanderLogicManagerServer2UserMsg(LMsgLM2GUserMsg* msg);

	void			HanderUser2LogicServerMsg(LMsgG2LUserMsg* msg);
	void			HanderLogicServer2UserMsg(LMsgL2GUserMsg* msg);

	void			HanderLM2GUserStatusModify(LMsgLM2GUserStatusModify* msg);

public:

	void			ConnectToLogicManager();
	
	//发送消息到所有User
	void			SendMessageToAllUse(LMsg* msg,Lint appId=0);

	void			SendToLogicUserOut(GUserPtr user);
	

	void			HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg);

	LogicInfo*		GetLogicInfoBySp(LSocketPtr sp);
	LogicInfo*		GetLogicInfoById(Lint id);

	//socket管理
	void			AddSp(LSocketPtr sp);
	void			DelSp(LSocketPtr sp);
protected:
	TCPClientPtr	GetLogicClientBySP(LSocketPtr sp); 
	bool			IsLogicSP(LSocketPtr sp);
protected:
	TCPClientPtr	GetGameServerClientBySP(LSocketPtr sp);
	bool			IsGameServerSP(LSocketPtr sp);
	GameServerInfo* GetGameServerById(GUserPtr user);
	void			SendToGameServer(GUserPtr user, LMsg* msg);

private:
	void			SendGateLoginToLM();
	void			SendGateLoginToLogic(TCPClientPtr logicClient);
	void            SendGateLoginToGameServer(TCPClientPtr gameClient);
	void			CheckMsgFrequency(Lint interval);
	
public:
	//bool			Login(G sp, Lint user_id);
	GameServerInfo*	SelectNewGameServer(Lint uid);


public:
	
private:
	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30毫秒循环一次

	Llong		m_200MSceTick;//200毫秒循环一次

	Llong		m_1SceTick;//1秒循环一次

	Llong		m_5SceTick;//5秒循环一次

	Llong		m_30SceTick;//30秒循环一次

	Llong		m_600SceTick;//5分钟秒循环一次

private:
	
	TCPClientPtr  m_logicManager;
	std::map<Lint, LogicInfo>	m_logicInfo;

	std::map<Lint, GameServerInfo>	m_game_server_info;

	typedef struct
	{
		Lint		timestamp;
		Lint		msgCount;
	}ClientContext;

	std::map<LSocketPtr, ClientContext>	m_spMap;

	
	
	GarbageCollectionService m_gcService;
};


#define gWork Work::Instance()

#endif