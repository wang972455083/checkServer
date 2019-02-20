#ifndef _GATE_USER_H_
#define _GATE_USER_H_

#include "LSingleton.h"
#include "LSocket.h"

struct GateUser
{
	Lint			m_user_id;	                    //userId
	LSocketPtr		m_sp;
	bool			m_online;
	Lint			m_userState;				//玩家当前状态
	Lint			m_logicID;					//玩家在哪个logic上面
	Lint			m_game_server_id;			//在哪個gameserver上
	Lint			m_status;					//状态，大厅或者桌子上
	GateUser()
	{
		m_user_id = 0;
		m_online = false;
		m_userState = 0;
		m_logicID = 0;
		m_game_server_id = 0;
		m_status = 0;
	}
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserLogicID(){return m_logicID;}
	void	setUserLogicID(Lint nValue){m_logicID = nValue;}
	Lint	getUserGameServerID() { return m_game_server_id; }
	void	setUserGameServerID(Lint nValue) { m_game_server_id = nValue; }
	void    SetUserId(Lint uid) { m_user_id = uid; }
	Lint	getUserId() { return m_user_id; }
};

typedef std::shared_ptr<GateUser> GUserPtr;

//////////////////////////////////////////////////////////////////////////
class GateUserManager :public LSingleton<GateUserManager>
{
public:

	virtual	bool	Init();
	virtual bool	Final();


	GUserPtr GetUserById(Lint user_id);
	GUserPtr GetUserBySp(LSocketPtr sp);
	void	AddUserToSpMap(GUserPtr user);
	void	AddUserToUidMap(GUserPtr user);
	void	DelUser(GUserPtr user);


	GUserPtr	CreateUser(LSocketPtr sp);

	void		Login(GUserPtr user, int user_id);
	
private:
	std::map<Lint, GUserPtr>	m_gateUserIdMap;
	std::map<LSocketPtr, GUserPtr>  m_gateUserSpMap;
};

#define gGateUserManager GateUserManager::Instance()

#endif