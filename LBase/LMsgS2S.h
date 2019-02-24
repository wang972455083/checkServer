#ifndef _LMSG_S2S_H_
#define _LMSG_S2S_H_

#include "LMsg.h"
#include "LUser.h"
#include "LCharge.h"
#include "GameDefine.h"
#include "TCPClient.h"


//////////////////////////////////////////////////////////////////////////
//logic 跟 center 

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////

struct LogicInfo
{
	Lint		m_id;
	Lint		m_deskCount;
	Lstring		m_ip;
	Lshort		m_port;
	LSocketPtr	m_sp;
	TCPClientPtr	m_client;
	Lint		m_flag;
	Lint		m_exclusive_play;	//服务器的玩法, 0:表示没有,用于把某种玩法归到服务器,以免新玩法上线时崩溃(┬＿┬)

public:
	LogicInfo()
		:m_id(0)
		,m_deskCount(0)
		,m_port(0)
		,m_flag(0)
		,m_exclusive_play(0)
	{

	}
};



struct LMsgLMG2GateLogicInfo :public LMsg
{
	Lint		m_ID;
	Lint		m_count;
	LogicInfo	m_logic[500];

	LMsgLMG2GateLogicInfo() :LMsg(MSG_LMG_2_G_SYNC_LOGIC)
		,m_ID(0)
		,m_count(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_count);
		for (Lint i = 0; i < m_count && i < 500; ++i)
		{
			buff.Read(m_logic[i].m_id);
			buff.Read(m_logic[i].m_deskCount);
			buff.Read(m_logic[i].m_ip);
			buff.Read(m_logic[i].m_port);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < 500; ++i)
		{
			buff.Write(m_logic[i].m_id);
			buff.Write(m_logic[i].m_deskCount);
			buff.Write(m_logic[i].m_ip);
			buff.Write(m_logic[i].m_port);
		} 
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GateLogicInfo();
	}
};






struct GateInfo
{
	Lint		m_id;
	Lint		m_userCount;
	Lstring		m_ip;
	Lshort		m_port;
	LSocketPtr	m_sp;

public:
	GateInfo()
		:m_id(0)
		,m_userCount(0)
		,m_port(0)
	{

	}
};



//////////////////////////////////////////////////////////////////////////
//center发送玩家id信息到

//玩家基本信息
struct UserIdInfo
{
	Lint		m_id;
	Lstring		m_openId;
	Lstring		m_nike;
	Lstring		m_headImageUrl;
	Lint		m_sex;
	Lint		m_blockEndTime;

public:
	UserIdInfo()
		:m_id(0)
		,m_sex(0)
		,m_blockEndTime(0)
	{

	}
};



struct Season
{
	Lint            m_level; //用户获取的参赛券级别
	Lint            m_time; //用户获取参赛券时间
	Lint            m_playState; //玩法
	string          m_playType; //玩法选项
};














//////////////////////////////////////////////////////////////////////////
//gate 发送玩家消息 到 logicserver
struct LMsgG2LUserMsg :public LMsg
{
	Lint			m_userMsgId;
	Lstring			m_ip;
	Lint            m_userDataID; //用户数据库id,用于比赛场托管逻辑处理
	LBuffPtr		m_dataBuff;
	LMsg*			m_userMsg;

	LMsgG2LUserMsg() :LMsg(MSG_G_2_L_USER_MSG), m_userMsg(NULL)
		,m_userMsgId(0)
		, m_userDataID(0)
	{
	}

	virtual~LMsgG2LUserMsg()
	{
		if (m_userMsg)
			delete m_userMsg;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userMsgId);
		buff.Read(m_ip);
		buff.Read(m_userDataID);

		try {
			
			msgpack::unpacked  unpack;
			msgpack::unpack(unpack,buff.Data() + buff.GetOffset(), Lsize(buff.Size() - buff.GetOffset()));
			msgpack::object obj = unpack.get();

			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2LUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("LMsgG2LUserMsg::RecvMsgPack error");
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userMsgId);
		buff.Write(m_ip);
		buff.Write(m_userDataID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送玩家消息到 gate
struct LMsgL2GUserMsg : public LMsg
{
	Lint			m_user_id;
	Lint			m_user_msg_id;
	LBuffPtr		m_dataBuff;
	
	LMsgL2GUserMsg() :LMsg(MSG_L_2_G_USER_MSG)
		, m_user_id(0),m_user_msg_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_user_id);
		buff.Read(m_user_msg_id);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_user_id);
		buff.Write(m_user_msg_id);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//gate 发送玩家掉线消息 到 logic
struct LMsgG2LUserOutMsg :public LMsg
{
	Llong			m_userGateId;
	
	LMsgG2LUserOutMsg():LMsg(MSG_G_2_L_USER_OUT_MSG)
		,m_userGateId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userGateId);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userGateId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LUserOutMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送玩家消息到 gate
struct LMsgL2GUserOutMsg : public LMsg
{
	Llong			m_id;

	LMsgL2GUserOutMsg():LMsg(MSG_L_2_G_USER_OUT_MSG)
		,m_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GUserOutMsg();
	}
};






struct LMsgS2SKeepAlive : public LMsg
{
	LMsgS2SKeepAlive() : LMsg(MSG_S_2_S_KEEP_ALIVE) {}

	virtual bool Read(LBuff& buff)
	{
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2SKeepAlive();
	}
};

struct LMsgG2LLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgG2LLogin() :LMsg(MSG_G_2_L_LOGIN)
		, m_id(0)
		, m_port(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LLogin();
	}
};

struct LMsgL2LMGLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;
	Lint		m_exclusive_play;	//服务器的专属玩法

	LMsgL2LMGLogin() :LMsg(MSG_L_2_LMG_LOGIN)
		, m_id(0)
		, m_port(0)
		, m_exclusive_play(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		buff.Read(m_exclusive_play);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		buff.Write(m_exclusive_play);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGLogin();
	}
};

struct LMsgG2ServerLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgG2ServerLogin() :LMsg(MSG_G_2_SERVER_LOGIN)
		, m_id(0)
		, m_port(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2ServerLogin();
	}
};

struct LMsgS2SKeepAliveAck : public LMsg
{
	LMsgS2SKeepAliveAck() : LMsg(MSG_S_2_S_KEEP_ALIVE_ACK) {}

	virtual bool Read(LBuff& buff)
	{
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2SKeepAliveAck();
	}
};


//分隔符

struct LMsgGame2LMGLogin :public LMsg
{
	Lint		m_id;
	//Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgGame2LMGLogin() :LMsg(MSG_GAME_2_LM_LOGIN)
		, m_id(0)
		, m_port(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		//buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		//buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgGame2LMGLogin();
	}
};

struct GameServerInfo
{
	Lint		m_id;
	Lstring		m_ip;
	Lshort		m_port;
	LSocketPtr	m_sp;

	TCPClientPtr	m_client;
};

struct LMsgLMG2GateGameServerInfo :public LMsg
{
	Lint		m_ID;
	Lint		m_count;
	GameServerInfo	m_logic[500];

	LMsgLMG2GateGameServerInfo() :LMsg(MSG_LMG_2_G_SYNC_GAME_SERVER)
		, m_ID(0)
		, m_count(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_count);
		for (Lint i = 0; i < m_count && i < 500; ++i)
		{
			buff.Read(m_logic[i].m_id);
			
			buff.Read(m_logic[i].m_ip);
			buff.Read(m_logic[i].m_port);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < 500; ++i)
		{
			buff.Write(m_logic[i].m_id);
			
			buff.Write(m_logic[i].m_ip);
			buff.Write(m_logic[i].m_port);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GateGameServerInfo();
	}
}; 


struct LMsgG2GameUserMsg :public LMsg
{
	Lint			m_userMsgId;
	Lstring			m_ip;
	Lint            m_userDataID; //用户数据库id,用于比赛场托管逻辑处理
	LBuffPtr		m_dataBuff;
	LMsg*			m_userMsg;

	LMsgG2GameUserMsg() :LMsg(MSG_G_2_GAME_USER_MSG), m_userMsg(NULL)
		, m_userMsgId(0)
		, m_userDataID(0)
	{
	}

	virtual~LMsgG2GameUserMsg()
	{
		if (m_userMsg)
			delete m_userMsg;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userMsgId);
		buff.Read(m_ip);
		buff.Read(m_userDataID);

		int msgid = MSG_ERROR_MSG;

		try {
			
			msgpack::unpacked  unpack;
			msgpack::unpack(unpack,buff.Data() + buff.GetOffset(), Lsize(buff.Size() - buff.GetOffset()));
			msgpack::object obj = unpack.get();

			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2GameUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("LMsgG2GameUserMsg::RecvMsgPack error");
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userMsgId);
		buff.Write(m_ip);
		buff.Write(m_userDataID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2GameUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送玩家消息到 gate
struct LMsgGame2GUserMsg : public LMsg
{
	Lint			m_user_id;
	Lint			m_user_msg_id;
	LBuffPtr		m_dataBuff;

	LMsgGame2GUserMsg() :LMsg(MSG_GAME_2_G_USER_MSG)
		, m_user_id(0),m_user_msg_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_user_id);
		buff.Read(m_user_msg_id);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_user_id);
		buff.Write(m_user_msg_id);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgGame2GUserMsg();
	}
};



//gate到logicmanager
struct LMsgG2LMUserMsg :public LMsg
{
	Lint			m_userMsgId;
	Lstring			m_ip;
	Lint            m_userDataID; //用户数据库id,用于比赛场托管逻辑处理
	LBuffPtr		m_dataBuff;
	LMsg*			m_userMsg;

	LMsgG2LMUserMsg() :LMsg(MSG_G_2_LOGIC_MANAGER_USER_MSG), m_userMsg(NULL)
		, m_userMsgId(0)
		, m_userDataID(0)
	{
	}

	virtual~LMsgG2LMUserMsg()
	{
		if (m_userMsg)
			delete m_userMsg;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userMsgId);
		buff.Read(m_ip);
		buff.Read(m_userDataID);

		try {
			
			msgpack::unpacked  unpack;
			msgpack::unpack(unpack,buff.Data() + buff.GetOffset(), Lsize(buff.Size() - buff.GetOffset()));
			msgpack::object obj = unpack.get();

			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2LMUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("LMsgG2LMUserMsg::RecvMsgPack error");
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userMsgId);
		buff.Write(m_ip);
		buff.Write(m_userDataID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LMUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送玩家消息到 gate
struct LMsgLM2GUserMsg : public LMsg
{
	Lint			m_user_id;
	Lint			m_user_msg_id;
	LBuffPtr		m_dataBuff;

	LMsgLM2GUserMsg() :LMsg(MSG_LOGIC_MANAGER_2_G_USER_MSG)
		, m_user_id(0), m_user_msg_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_user_id);
		buff.Read(m_user_msg_id);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_user_id);
		buff.Write(m_user_msg_id);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLM2GUserMsg();
	}
};

//新增

struct MsgUserInfo
{
	int m_user_id;
	Lstring m_name;
	Lstring m_head_icon;
	Lint    m_gate_id;
	bool    m_robot;
};


struct LMsgLM2LQuickCreateRoomOpt : public LMsg
{
	Lint			m_room_id;
	
	std::vector<MsgUserInfo>	m_users;
	int				m_user_cnt;


	LMsgLM2LQuickCreateRoomOpt() :LMsg(MSG_LM_2_L_QUICK_CREATE_ROOM_OPT)
		, m_room_id(0), m_user_cnt(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_room_id);
	
		buff.Read(m_user_cnt);

		for (int i = 0; i < m_user_cnt; ++i)
		{
			MsgUserInfo user;
			buff.Read(user.m_user_id);
			buff.Read(user.m_name);
			buff.Read(user.m_head_icon);
			buff.Read(user.m_gate_id);
			buff.Read(user.m_robot);

			m_users.push_back(user);
		}
		
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_room_id);
		
		m_user_cnt = m_users.size();
		buff.Write(m_user_cnt);

		for (int i = 0; i < m_users.size(); ++i)
		{
			MsgUserInfo user = m_users[i];
			buff.Write(user.m_user_id);
			buff.Write(user.m_name);
			buff.Write(user.m_head_icon);
			buff.Write(user.m_gate_id);
			buff.Write(user.m_robot);
		}
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLM2LQuickCreateRoomOpt();
	}
};


struct LMsgL2LMQuickCreateRoomOpt: public LMsg
{
	Lint			m_result; 
	Lint			m_room_id;

	std::vector<RoomUser>	m_users;
	int				m_user_cnt;


	LMsgL2LMQuickCreateRoomOpt() :LMsg(MSG_L_2_LM_QUICK_CREATE_ROOM_OPT)
		, m_room_id(0), m_user_cnt(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_result);
		buff.Read(m_room_id);

		buff.Read(m_user_cnt);

		for (int i = 0; i < m_user_cnt; ++i)
		{
			RoomUser user;
			buff.Read(user.m_user_id);
			buff.Read(user.m_name);
			buff.Read(user.m_head_icon);
			buff.Read(user.m_star);

			m_users.push_back(user);
		}


		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_result);
		buff.Write(m_room_id);

		m_user_cnt = m_users.size();
		buff.Write(m_user_cnt);

		for (int i = 0; i < m_users.size(); ++i)
		{
			RoomUser user = m_users[i];
			buff.Write(user.m_user_id);
			buff.Write(user.m_name);
			buff.Write(user.m_head_icon);
			buff.Write(user.m_star);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMQuickCreateRoomOpt();
	}
};

/*
struct LMsgL2LMDeskOpt : public LMsg
{
	Lint			m_result;			//返回结果 :0成功  1角色已经在桌子中了 2,桌子已经存在  3，桌子已经满了加入失败
										// 4,桌子不存在  5，角色不在桌子中
	Lint			m_user_id;
	Lint			m_type;
	Lint			m_desk_id;

	Lint			m_desk_type;		//桌子类型  金币桌  钻石桌
	Lint			m_cost;				//钻石桌消耗的钻石

	bool			m_del_desk;			//是否删掉桌子

	int				m_user_cnt;			//房间人数
	std::vector<MsgUserInfo>	m_users;	//房间人信息



	LMsgL2LMDeskOpt() :LMsg(MSG_L_2_LM_DESK_OPT)
		, m_result(0), m_user_id(0),m_type(0), m_desk_id(0), m_desk_type(0), m_cost(0), m_del_desk(0), m_user_cnt(0)
	{
		m_users.clear();
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_result);
		buff.Read(m_user_id);
		buff.Read(m_type);
		buff.Read(m_desk_id);
		buff.Read(m_desk_type);
		buff.Read(m_cost);
		buff.Read(m_del_desk);

		buff.Read(m_user_cnt);

		for (int i = 0; i < m_user_cnt; ++i)
		{
			MsgUserInfo user;
			buff.Read(user.m_user_id);
			buff.Read(user.m_name);
			buff.Read(user.m_head_icon);
			//buff.Read(user.m_pos);

			m_users.push_back(user);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_result);
		buff.Write(m_user_id);
		buff.Write(m_type);
		buff.Write(m_desk_id);
		buff.Write(m_desk_type);
		buff.Write(m_cost);
		buff.Write(m_del_desk);

		int user_cnt = m_users.size();
		buff.Write(user_cnt);
		for (int i = 0; i < m_users.size(); ++i)
		{
			MsgUserInfo& user = m_users[i];
			buff.Write(user.m_user_id);
			buff.Write(user.m_name);
			buff.Write(user.m_head_icon);
			//buff.Write(user.m_pos);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMDeskOpt();
	}
};
*/

struct LMsgLM2GUserStatusModify : public LMsg
{
	Lint			m_user_id;	
	Lint			m_status;		//状态0，大厅  1，房间
	Lint			m_logic_server_id;  


	LMsgLM2GUserStatusModify() :LMsg(MSG_LM_2_G_USER_STATUS_MODIFY)
		, m_user_id(0), m_status(0), m_logic_server_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_user_id);
		buff.Read(m_status);
		buff.Read(m_logic_server_id);
		

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_user_id);
		buff.Write(m_status);
		buff.Write(m_logic_server_id);
	

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLM2GUserStatusModify();
	}
};


struct LMsgLM2LMQuckCreateRoom : public LMsg
{

	std::vector<int>	m_users;
	LMsgLM2LMQuckCreateRoom() :LMsg(MSG_LM_2_LM_QUICK_CREATE_ROOM)
	{
		m_users.clear();
	}

	virtual bool Read(LBuff& buff)
	{
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLM2LMQuckCreateRoom();
	}
};

struct LMsgS2SModifyCoin :public LMsg
{
	Lint			m_user_id;
	Lint			m_type;			// 0,增加  1，减少
	Lint			m_coin;			//金币数


	enum 
	{
		TYPE_ADD,
		TYPE_DESC,
	};

	LMsgS2SModifyCoin() :LMsg(MSG_S_2_S_MODIFY_COIN)
	{
		m_user_id = 0;
		m_type = 0;
		m_coin = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_user_id);
		buff.Read(m_type);
		buff.Read(m_coin);


		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_user_id);
		buff.Write(m_type);
		buff.Write(m_coin);


		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2SModifyCoin();
	}
};

struct LMsgL2LAutoSelectCard :public LMsg
{
	Lint			m_user_id;
	Lint			m_room_id;
	Lint			m_desk_id;			
	Lint			m_card;			


	LMsgL2LAutoSelectCard() :LMsg(MSG_L_2_L_AUTO_SELECT_CARD)
	{
		m_user_id = 0;
		m_room_id = 0;
		m_desk_id = 0;
		m_card = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_user_id);
		buff.Read(m_room_id);
		buff.Read(m_desk_id);
		buff.Read(m_card);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_user_id);
		buff.Write(m_room_id);
		buff.Write(m_desk_id);
		buff.Write(m_card);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LAutoSelectCard();
	}
};


struct LMsgL2LMQuitRoom :public LMsg
{
	std::vector<int>			m_users;
	int							m_count;
	
	LMsgL2LMQuitRoom() :LMsg(MSG_L_2_LM_QUIT_ROOM)
	{
		m_users.clear();
		m_count = 0;
		
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_count);

		for (int i = 0; i < m_count; ++i)
		{
			int user_id = 0;
			buff.Read(user_id);
			m_users.push_back(user_id);
		}
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_count = m_users.size();
		buff.Write(m_count);

		for (int i = 0; i < m_users.size(); ++i)
		{
			buff.Write(m_users[i]);
		}
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMQuitRoom();
	}
};


struct LMsgG2SUserLogOut :public LMsg
{
	int			m_user_id;

	LMsgG2SUserLogOut() :LMsg(MSG_G_2_S_USER_OUT)
	{
		m_user_id = 0;

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_user_id);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_user_id);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2SUserLogOut();
	}
};

struct LMsgLM2LUserLogOut :public LMsg
{
	int			m_room_id;
	int			m_user_id;

	LMsgLM2LUserLogOut() :LMsg(MSG_LM_2_L_USER_OUT)
	{
		m_room_id = 0;
		m_user_id = 0;

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_room_id);
		buff.Read(m_user_id);


		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_room_id);
		buff.Write(m_user_id);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLM2LUserLogOut();
	}
};

struct LMsgLM2LUserLogin :public LMsg
{
	int			m_room_id;
	int			m_user_id;
	int			m_gate_id;

	LMsgLM2LUserLogin() :LMsg(MSG_LM_2_L_USER_LOGIN)
	{
		m_room_id = 0;
		m_user_id = 0;
		m_gate_id = 0;

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_room_id);
		buff.Read(m_user_id);
		buff.Read(m_gate_id);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_room_id);
		buff.Write(m_user_id);
		buff.Write(m_gate_id);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLM2LUserLogin();
	}
};

struct LMsgL2LMRecyleRoom : public LMsg
{
	Lint			m_room_id;

	LMsgL2LMRecyleRoom() :LMsg(MSG_L_2_LM_RECYLE_ROOM)
		, m_room_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_room_id);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_room_id);

	
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMRecyleRoom();
	}
};


#endif