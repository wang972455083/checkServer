#ifndef _L_MSG_H_
#define _L_MSG_H_

#pragma warning (disable : 4018)
#pragma warning (disable : 4996)
#pragma warning (disable : 4819)
#pragma warning (disable : 4099)

#include "LSocket.h"
#include "LSingleton.h"
#include "LTool.h"
#include "LLog.h"
#include "LVideo.h"
#include "GameDefine.h"
#include "ScoreItem.h"
#include "ActiveDefine.h"

static void ReadMapVector(msgpack::object & obj, const char * name, std::vector<Lint> & out)
{
	msgpack::object array;
	ReadMapData(obj, name, array);
	if (array.type == msgpack::v1::type::ARRAY)
	{
		for (Lsize i = 0; i < array.via.array.size; ++i)
		{
			Lint v;
			msgpack::object& obj =  *(array.via.array.ptr+i);
			obj.convert(v);
			out.push_back(v);
		}
	}
}

struct LMsg;

class LMsgFactory : public LSingleton<LMsgFactory>
{
public:
	virtual bool Init();

	virtual bool Final();

	void RegestMsg(LMsg* pMsg);

	LMsg* CreateMsg(int msgId);

private:
	std::map<int,LMsg*> m_msgMap;
};


struct LMsg
{
	LMsg(int id);
	virtual~LMsg();

	LBuffPtr		GetSendBuff();
	void			WriteMsgId(LBuff& buff);
	void			ReadMsgId(LBuff& buff);

	virtual bool	Write(LBuff& buff);
	virtual bool	Write(msgpack::packer<msgpack::sbuffer>& pack);
	virtual bool	Read(LBuff& buff);
	virtual bool	Read(msgpack::object& obj);
	virtual LMsg*	Clone();
	virtual Lint	preAllocSendMemorySize();

	LBuffPtr			m_send;
	LSocketPtr			m_sp;
	Lint				m_msgId;
	bool				m_isMsgPack;
};

//客户端 服务器交互的消息
struct LMsgSC :public LMsg
{
	LMsgSC(int id) :LMsg(id)
	{
		m_isMsgPack = true;
		m_user_id = 0;
	}

	int		m_user_id;
};
//玩家状态信息定义
enum LGUserMsgState
{
	LGU_STATE_IDLE,			//空闲，发到Center
	LGU_STATE_CENTER,		//玩家在大厅，发到Center
	LGU_STATE_DESK,			//玩家在桌子，发到Logic
	LGU_STATE_MATCH,		//玩家在比赛状态，发到Logic
};
//服务器端跟客户端通讯的消息号
enum LMSG_ID
{
	MSG_ERROR_MSG = 0,//错误消息

	MSG_C_2_S_MSG = 1,//客户端发给服务器
	MSG_S_2_C_MSG = 2,//服务器返回结果


	//
	MSG_C_2_S_RE_LOGIN = 10, //客户端请求重连
	MSG_C_2_S_LOGIN = 11, //客户端请求登录
	MSG_S_2_C_LOGIN = 12, //服务器返回客户登录结果

	MSG_C_2_S_TEST = 13,
	MSG_S_2_C_TEST = 14,

	MSG_TO_GAME_MAX_MSG = 1000,   //以上是转发到gameServer里的

	MSG_C_2_S_LM_LOGIN = 1001,		//登录LM
	MSG_S_2_C_LM_LOGIN = 1002,		//登录LM

	MSG_C_2_S_DESK_OPT = 1003,		//加入活退出桌子
	MSG_S_2_C_DESK_OPT = 1004,		//加入或者退出桌子


	MSG_TO_LOGIC_MANAGER_MAX_MSG = 2000,   //以上是转发到LogicManager上的

	MSG_C_2_S_NOTICE_LOGIN = 2001,			//通知登录

	MSG_TO_LOGIC_SERVER_MAX_MSG = 3000,		//以上是转发到LogicServer的

	MSG_C_2_S_READY_OPT = 3001,				//准备或者取消准备
	MSG_S_2_C_READY_OPT = 3002,

	MSG_C_2_S_CHESS_MOVE = 3003,				//移动
	MSG_S_2_C_CHESS_MOVE = 3004,

	MSG_C_2_S_CHESS_ATTACK = 3005,				//攻击
	MSG_S_2_C_CHESS_ATTACK = 3006,				//

	MSG_C_2_S_CHESS_UPGRADE = 3007,				//升级属性
	MSG_S_2_C_CHESS_UPGRADE = 3008,				//

	MSG_S_2_C_CHESS_INFO = 3010,				//整个棋桌消息

	MSG_S_2_C_CHESS_RESULT = 3012,				//结果

	MSG_C_2_S_HEART = 4001, //心跳包
	MSG_S_2_C_HEART = 4002, //心跳包

	
	MSG_S_2_C_MAX = 4096, //服务器客户端之间最大消息号

	//////////////////////////////////////////////////////////////////////////

	MSG_S_2_S_KEEP_ALIVE = 5000,
	MSG_S_2_S_KEEP_ALIVE_ACK = 5001,
	//下面这是服务器与服务器之间的消息交互
	//////////////////////////////////////////////////////////////////////////
	//logicManager 跟 center 之间的交互


	
    //////////////////////////////////////////////////////////////////////////
    //gate 跟 logic 之间的交互
    MSG_G_2_L_LOGIN = 7000, //gate登录logic		由LogicManager转发
   
    MSG_G_2_L_USER_MSG = 8000, //gate转发玩家消息到logic
    MSG_L_2_G_USER_MSG = 8001, //logic转发玩家消息到gate
    
    MSG_G_2_L_USER_OUT_MSG = 8002, //gate转发玩家退出消息到logic
    MSG_L_2_G_USER_OUT_MSG = 8003, //logic转发玩家退出消息到gate
    
   
    
	
	//////////////////////////////////////////////////////////////////////////
	
										
	//

    //////////////////////////////////////////////////////////////////////////
    //LogicManager 与 Logic
	MSG_L_2_LMG_LOGIN = 9001,				//逻辑管理登陆

	//................

	//分隔符
	MSG_GAME_2_LM_LOGIN = 10001,
	MSG_LMG_2_G_SYNC_GAME_SERVER = 10002,

	MSG_G_2_GAME_USER_MSG = 10003, //gate转发玩家消息到gameserver
	MSG_GAME_2_G_USER_MSG = 10004, //gameserver转发玩家消息到gate
	
	MSG_G_2_LOGIC_MANAGER_USER_MSG = 10005, //gate转发玩家消息到logicManager
	MSG_LOGIC_MANAGER_2_G_USER_MSG = 10006, //logicManager转发玩家消息到gate
	

    //LogicManager 与 Gate
    MSG_G_2_SERVER_LOGIN = 12001,			//逻辑管理登陆
                              
    MSG_LMG_2_G_SYNC_LOGIC = 12002,		//同步逻辑信息
  
	//业务逻辑



	MSG_LM_2_L_DESK_OPT = 20003,  //桌子操作
	MSG_L_2_LM_DESK_OPT = 20004,

	MSG_LM_2_G_USER_STATUS_MODIFY = 20005,//  大厅和房间状态修改

	

};

struct BirdInfo
{
	Lint	m_pos;
	CardValue m_card;

	MSGPACK_DEFINE(m_pos, m_card); 

	BirdInfo():m_pos(0){}
	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack,NAME_TO_STR(m_pos),m_pos);
		m_card.Write(pack);
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
//socket断开的消息
#define MSG_CLIENT_KICK 0x7FFFFFFF

struct LMsgKick:public LMsg
{
	LMsgKick():LMsg(MSG_CLIENT_KICK){}

	virtual LMsg* Clone()
	{
		return new LMsgKick();
	}
};

//////////////////////////////////////////////////////////////////////////
//所有socket连上的消息
#define MSG_CLIENT_IN 0x7FFFFFFE

struct LMsgIn:public LMsg
{
	LMsgIn():LMsg(MSG_CLIENT_IN){}

	virtual LMsg* Clone()
	{
		return new LMsgIn();
	}
};

//////////////////////////////////////////////////////////////////////////
//http消息号
#define MSG_HTTP 0x7FFFFFFD

struct LMsgHttp:public LMsg
{
	std::string m_head;//消息头
	std::string m_body;//消息体

	LMsgHttp():LMsg(MSG_HTTP){}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_head);
		buff.Write(m_body);
		return true;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_head);
		buff.Read(m_body);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgHttp();
	}
};
//////////////////////////////////////////////////////////////////////////
// 向工会创建者发送房卡不足的警报
#define MSG_TEAM_CARD_ALERT	0x7FFFFFFC
struct LMsgTeamCardAlert :public LMsg
{
	Lint nCreaterID;
	Lint nTeamID;
	Lstring strTeamName;

	LMsgTeamCardAlert() :LMsg(MSG_TEAM_CARD_ALERT),
		nCreaterID(0),
		nTeamID(0)
	{}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(nCreaterID);
		buff.Write(nTeamID);
		buff.Write(strTeamName);
		return true;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(nCreaterID);
		buff.Read(nTeamID);
		buff.Read(strTeamName);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgTeamCardAlert();
	}
};
//////////////////////////////////////////////////////////////////////////
//客户端发送消息
struct LMsgC2SMsg :public LMsgSC
{
	Lstring			m_openId;
	Lstring			m_nike;
	Lstring			m_sign;
	Lstring			m_plate;
	Lstring			m_accessToken;
	Lstring			m_refreshToken;
	Lstring			m_md5;
	Lint			m_severID;
	Lstring			m_uuid;
	Lint			m_sex;
	Lstring			m_imageUrl;
	Lstring			m_nikename;
	Lint			m_wxConfirmed;
	Lstring			m_clientIp;
	Lint			m_nAppId;
	Lint			m_reLogin;		// 是否断线重连 0 正常登陆 1 断线重连
	Lint			m_deviceType;	// 设备类型 0 Android 1 IOS
	Lstring			m_deviceId;		// 设备编号

	LMsgC2SMsg() :LMsgSC(MSG_C_2_S_MSG)
		,m_severID(0)
		,m_sex(0)
		,m_wxConfirmed(0)
		,m_nAppId(0)
		,m_reLogin(0)
		,m_deviceType(0)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_openId", m_openId);
		ReadMapData(obj, "m_nike", m_nike);
		ReadMapData(obj, "m_sign", m_sign);
		ReadMapData(obj, "m_plate", m_plate);
		ReadMapData(obj, "m_accessToken", m_accessToken);
		ReadMapData(obj, "m_refreshToken", m_refreshToken);
		ReadMapData(obj, "m_md5", m_md5);
		ReadMapData(obj, "m_severID", m_severID);
		ReadMapData(obj, "m_uuid", m_uuid);
		ReadMapData(obj, "m_sex", m_sex);
		ReadMapData(obj, "m_imageUrl", m_imageUrl);
		ReadMapData(obj, "m_nikename", m_nikename);
		ReadMapData(obj, "m_wxConfirmed", m_wxConfirmed);
		ReadMapData(obj, "m_clientIp", m_clientIp);
		ReadMapData(obj, "m_nAppId", m_nAppId);
		ReadMapData(obj, "m_reLogin", m_reLogin);
		ReadMapData(obj, "m_deviceId", m_deviceId);
		ReadMapData(obj, "m_deviceType", m_deviceType);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,19);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_openId", m_openId);
		WriteKeyValue(pack, "m_nike", m_nike);
		WriteKeyValue(pack, "m_sign", m_sign);
		WriteKeyValue(pack, "m_plate", m_plate);
		WriteKeyValue(pack, "m_accessToken", m_accessToken);
		WriteKeyValue(pack, "m_refreshToken", m_refreshToken);
		WriteKeyValue(pack, "m_md5", m_md5);
		WriteKeyValue(pack, "m_severID", m_severID);
		WriteKeyValue(pack, "m_uuid", m_uuid);
		WriteKeyValue(pack, "m_sex", m_sex);
		WriteKeyValue(pack, "m_imageUrl", m_imageUrl);
		WriteKeyValue(pack, "m_nikename", m_nikename);
		WriteKeyValue(pack, "m_wxConfirmed", m_wxConfirmed);
		WriteKeyValue(pack, "m_clientIp", m_clientIp);
		WriteKeyValue(pack, "m_nAppId", m_nAppId);
		WriteKeyValue(pack, "m_reLogin", m_reLogin);
		WriteKeyValue(pack, "m_deviceId", m_deviceId);
		WriteKeyValue(pack, "m_deviceType", m_deviceType);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SMsg();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送消息
struct LMsgS2CMsg :public LMsgSC
{
	Lint			m_errorCode;//0-成功，1-服务器还没启动成功 2-微信登陆失败 3- 微信返回失败 4-创建角色失败 5- 在原APP未退  7,非法客户端
	Lint			m_seed;
	Lint			m_id;
	Lstring			m_gateIp;
	Lshort			m_gatePort;
	Lstring			m_errorMsg;	//若登陆失败，返回玩家错误原因;
	Lint			m_totalPlayNum;


	LMsgS2CMsg() :LMsgSC(MSG_S_2_C_MSG)
		,m_errorCode(0)
		,m_seed(0)
		,m_id(0)
		,m_gatePort(0)
		,m_totalPlayNum(0)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_errorCode", m_errorCode);
		ReadMapData(obj, "m_seed", m_seed);
		ReadMapData(obj, "m_id", m_id);
		ReadMapData(obj, "m_gateIp", m_gateIp);
		ReadMapData(obj, "m_gatePort", m_gatePort);
		ReadMapData(obj, "m_errorMsg", m_errorMsg);
		ReadMapData(obj, "m_totalPlayNum", m_totalPlayNum);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_errorCode", m_errorCode);
		WriteKeyValue(pack, "m_seed", m_seed);
		WriteKeyValue(pack, "m_id", m_id);
		WriteKeyValue(pack, "m_gateIp", m_gateIp);
		WriteKeyValue(pack, "m_gatePort", m_gatePort);
		WriteKeyValue(pack, "m_errorMsg", m_errorMsg);
		WriteKeyValue(pack, "m_totalPlayNum", m_totalPlayNum);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CMsg();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送心跳包
struct LMsgC2SHeart:public LMsgSC
{
	//Lint	m_time;//时间戳

	LMsgC2SHeart() :LMsgSC(MSG_C_2_S_HEART){}

	virtual bool Read(msgpack::object& obj)
	{
		//buff.Read(m_time);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SHeart();}
};


//////////////////////////////////////////////////////////////////////////
//服务器发送心跳包
struct LMsgS2CHeart :public LMsgSC
{
	Lint     m_returnId;
	Lint	m_time;//时间戳

	LMsgS2CHeart() :LMsgSC(MSG_S_2_C_HEART)
		,m_time(0), m_returnId(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_returnId), m_returnId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CHeart(); }
};



//////////////////////////////////////////////////////////////////////////
//客户端请求登录大厅

struct LMsgC2SLogin:public LMsgSC
{
	Lint	m_seed;
	Lstring	m_md5;
	Lstring m_location;


	LMsgC2SLogin() :LMsgSC(MSG_C_2_S_LOGIN)
	,m_seed(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_seed), m_seed);
		
		ReadMapData(obj, NAME_TO_STR(m_md5), m_md5);
		ReadMapData(obj, NAME_TO_STR(m_location), m_location);
	
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_seed), m_seed);
		WriteKeyValue(pack, NAME_TO_STR(m_md5), m_md5);
		WriteKeyValue(pack, NAME_TO_STR(m_location), m_location);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//返回玩家登陆结果消息
struct LMsgS2CLogin:public LMsgSC
{
	Lint		m_errorCode;//登录错误码，0-登录成功,1-帐号未注册，2-帐号已登录，3-未知错误
	Lstring		m_name;
	Lint		m_sex;
	Lstring		m_head_image;//
	Lint		m_money;
	
	LMsgS2CLogin() :LMsgSC(MSG_S_2_C_LOGIN)
		,m_errorCode(0)
		,m_sex(0)
		,m_money(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_name), m_name);
		
		ReadMapData(obj, NAME_TO_STR(m_sex), m_sex);
		ReadMapData(obj, NAME_TO_STR(m_head_image), m_head_image);
		ReadMapData(obj, NAME_TO_STR(m_money), m_money);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		
		WriteKeyValue(pack, NAME_TO_STR(m_name), m_name);
		WriteKeyValue(pack, NAME_TO_STR(m_sex), m_sex);
		WriteKeyValue(pack, NAME_TO_STR(m_head_image), m_head_image);
		WriteKeyValue(pack, NAME_TO_STR(m_money), m_money);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CLogin();}
};





//*****************************************************************************

struct LMsgC2SLMLogin :public LMsgSC
{
	LMsgC2SLMLogin() :LMsgSC(MSG_C_2_S_LM_LOGIN)
		
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SLMLogin();
	}
};

//返回玩家登陆结果消息
struct LMsgS2CLMLogin :public LMsgSC
{
	Lint		m_result;//登录错误码，0-登录成功,

	LMsgS2CLMLogin() :LMsgSC(MSG_S_2_C_LM_LOGIN)
		, m_result(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_result), m_result);
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CLMLogin(); }
};


struct MsgUserInfo
{
	int   m_user_id;
	Lstring m_name;
	Lstring m_head_icon;
	

	MSGPACK_DEFINE(m_user_id,m_name, m_head_icon);

public:
	MsgUserInfo()
	{
		m_user_id = 0;
		
	}

	MsgUserInfo& operator = (const MsgUserInfo& other)
	{
		if (&other == this)
			return *this;

		m_user_id = other.m_user_id;
		m_name = other.m_name;
		m_head_icon = other.m_head_icon;

		return *this;
	}
};

struct DeskMsg
{
	Lint	m_desk_id;
	Lint	m_create_id;
	Lint	m_desk_type;
	Lint	m_cost;

	std::vector<MsgUserInfo>	m_users;

	MSGPACK_DEFINE(m_desk_id, m_create_id, m_desk_type, m_cost, m_users);

public:
	DeskMsg() :m_desk_id(0), m_create_id(0), m_desk_type(0),m_cost(0)
	{
		m_users.clear();
	}
};



struct LMsgC2SDeskOpt :public LMsgSC
{
	Lint	m_type;				//1,创建  2，加入  3，退出
	Lint	m_desk_id;

	Lint	m_desk_type;
	Lint	m_cost;

	//MsgUserInfo	m_self;
	Lstring m_name;
	Lstring m_head_icon;


	enum
	{
		TYPE_CREATE_DESK = 1,
		TYPE_ADD_TO_DESK = 2,
		TYPE_QUIT_DESK = 3,
	};

	LMsgC2SDeskOpt() :LMsgSC(MSG_C_2_S_DESK_OPT)
		, m_desk_id(0),m_desk_type(0),m_cost(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_desk_id), m_desk_id);

		ReadMapData(obj, NAME_TO_STR(m_desk_type), m_desk_type);
		ReadMapData(obj, NAME_TO_STR(m_cost), m_cost);
		ReadMapData(obj, NAME_TO_STR(m_name), m_name);
		ReadMapData(obj, NAME_TO_STR(m_head_icon), m_head_icon);

		/*msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_list), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				DeskMsg v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_list.push_back(v);
			}
		}*/
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_desk_id), m_desk_id);

		WriteKeyValue(pack, NAME_TO_STR(m_desk_type), m_desk_type);
		WriteKeyValue(pack, NAME_TO_STR(m_cost), m_cost);
		WriteKeyValue(pack, NAME_TO_STR(m_name), m_name);
		WriteKeyValue(pack, NAME_TO_STR(m_head_icon), m_head_icon);
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SDeskOpt();
	}
};



//返回玩家登陆结果消息
struct LMsgS2CDeskOpt :public LMsgSC
{
	Lint		m_errorCode;//0-成功,1-玩家已经存在桌子 2,退出的桌子不正确
	Lint		m_type;
	DeskMsg		m_desk;
	
	

	LMsgS2CDeskOpt() :LMsgSC(MSG_S_2_C_DESK_OPT)
		, m_errorCode(0)
		,m_type(0)
		

	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		
		ReadMapData(obj, NAME_TO_STR(m_desk), m_desk);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		
		WriteKeyValue(pack, NAME_TO_STR(m_desk), m_desk);
		
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CDeskOpt(); }
};
struct LMsgC2SNoticeLogin :public LMsgSC
{
	Lint	m_user_id;

	LMsgC2SNoticeLogin() :LMsgSC(MSG_C_2_S_NOTICE_LOGIN)
		, m_user_id(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SNoticeLogin();
	}
};//LogicManager



//***********************************LogicServer

struct LMsgC2SReadyOpt :public LMsgSC
{
	Lint	m_type;				//0,取消准备  1，准备
	Lint	m_desk_id;
	
	

	LMsgC2SReadyOpt() :LMsgSC(MSG_C_2_S_READY_OPT)
		, m_type(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_desk_id), m_desk_id);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_desk_id), m_desk_id);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SReadyOpt();
	}
};



//返回玩家登陆结果消息
struct LMsgS2CReadyOpt :public LMsgSC
{
	Lint		m_errorCode;//0-成功
	Lint		m_status;    //0，无准备状态  1，准备状态

	LMsgS2CReadyOpt() :LMsgSC(MSG_S_2_C_READY_OPT)
		, m_errorCode(0)
		, m_status(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_status), m_status);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_status), m_status);
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CDeskOpt(); }
};

struct PointMsg
{
	Lint	m_posX;
	Lint	m_posY;

	PointMsg()
	{
		m_posX = 0;
		m_posY = 0;
	}

	MSGPACK_DEFINE(m_posX, m_posY);
};



struct LMsgC2SChessMove :public LMsgSC
{
	Lint	m_desk_id;			//桌子id
	Lint	m_move_id;			//棋子id
	PointMsg m_to_point;			//移动到的位置


	LMsgC2SChessMove() :LMsgSC(MSG_C_2_S_CHESS_MOVE)
		,m_desk_id(0),m_move_id(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		
		ReadMapData(obj, NAME_TO_STR(m_desk_id), m_desk_id);
		ReadMapData(obj, NAME_TO_STR(m_move_id), m_move_id);
		ReadMapData(obj, NAME_TO_STR(m_to_point), m_to_point);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_desk_id), m_desk_id);
		WriteKeyValue(pack, NAME_TO_STR(m_move_id), m_move_id);
		WriteKeyValue(pack, NAME_TO_STR(m_to_point), m_to_point);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SChessMove();
	}
};




struct LMsgS2CChessMove :public LMsgSC
{
	Lint		m_errorCode;//0-成功  1,不在操作角色轮次 2,棋子不存在 3,位置错误 4,移动到的位置有棋子 5,移动棋子颜色不对
	Lint		m_move_id;
	PointMsg	m_to_point;

	LMsgS2CChessMove() :LMsgSC(MSG_S_2_C_CHESS_MOVE)
		, m_errorCode(0)
		,m_move_id(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_move_id), m_move_id);
		ReadMapData(obj, NAME_TO_STR(m_to_point), m_to_point);
		

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_move_id), m_move_id);
		WriteKeyValue(pack, NAME_TO_STR(m_to_point), m_to_point);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CChessMove(); }
};

struct LMsgC2SChessAttack :public LMsgSC
{
	Lint	m_desk_id;			//桌子id
	Lint	m_attack_id;				//攻击id
	Lint	m_get_damage_id;			//被攻击id


	LMsgC2SChessAttack() :LMsgSC(MSG_C_2_S_CHESS_ATTACK)
		, m_desk_id(0), m_attack_id(0),m_get_damage_id(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);

		ReadMapData(obj, NAME_TO_STR(m_desk_id), m_desk_id);
		ReadMapData(obj, NAME_TO_STR(m_attack_id), m_attack_id);
		ReadMapData(obj, NAME_TO_STR(m_get_damage_id), m_get_damage_id);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_desk_id), m_desk_id);
		WriteKeyValue(pack, NAME_TO_STR(m_attack_id), m_attack_id);
		WriteKeyValue(pack, NAME_TO_STR(m_get_damage_id), m_get_damage_id);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SChessAttack();
	}
};

struct BlockMsg
{
	Lint		m_id;
	Lint		m_table_id;
	Lint		m_color;
	Lint		m_type;
	Lint		m_hp;
	Lint		m_max_hp;
	Lint		m_attack;
	Lint		m_max_attack;
	Lint		m_coin_level;
	Lint		m_max_coin_level;
	PointMsg	m_pos;

	BlockMsg()
	{
		m_id = 0;
		m_table_id = 0;
		m_color = 0;
		m_type = 0;
		m_hp = 0;
		m_max_hp = 0;
		m_attack = 0;
		m_max_attack = 0;
		m_coin_level = 0;
		m_max_coin_level = 0;
	}

	MSGPACK_DEFINE(m_id, m_table_id,m_color,m_type, m_hp,m_max_hp,m_attack,m_max_attack,m_coin_level,m_max_coin_level,m_pos)
};


struct LMsgS2CChessAttack :public LMsgSC
{
	Lint			m_errorCode;//0-成功  1,不在操作角色轮次 2,棋子不存在 3,棋子颜色不正确 4,不能攻击
	BlockMsg		m_attack_block;
	BlockMsg		m_get_damage_block;

	LMsgS2CChessAttack() :LMsgSC(MSG_S_2_C_CHESS_ATTACK)
		, m_errorCode(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_attack_block), m_attack_block);
		ReadMapData(obj, NAME_TO_STR(m_get_damage_block), m_get_damage_block);


		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_attack_block), m_attack_block);
		WriteKeyValue(pack, NAME_TO_STR(m_get_damage_block), m_get_damage_block);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CChessAttack(); }
};

struct LMsgC2SChessUpgrade :public LMsgSC
{
	Lint	m_desk_id;			//桌子id
	Lint	m_block_id;				//棋子
	Lint	m_protery;			//属性


	LMsgC2SChessUpgrade() :LMsgSC(MSG_C_2_S_CHESS_UPGRADE)
		, m_desk_id(0), m_block_id(0), m_protery(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);

		ReadMapData(obj, NAME_TO_STR(m_desk_id), m_desk_id);
		ReadMapData(obj, NAME_TO_STR(m_block_id), m_block_id);
		ReadMapData(obj, NAME_TO_STR(m_protery), m_protery);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_desk_id), m_desk_id);
		WriteKeyValue(pack, NAME_TO_STR(m_block_id), m_block_id);
		WriteKeyValue(pack, NAME_TO_STR(m_protery), m_protery);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SChessUpgrade();
	}
};

struct LMsgS2CChessUpgrade :public LMsgSC
{
	Lint			m_errorCode;//0-成功  1,不在操作角色轮次 2,棋子不存在 3,棋子颜色不正确 4,不能攻击
	BlockMsg		m_block;


	LMsgS2CChessUpgrade() :LMsgSC(MSG_S_2_C_CHESS_UPGRADE)
		, m_errorCode(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_block), m_block);
	
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_block), m_block);
	

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CChessUpgrade(); }
};

struct LMsgS2CChessInfo :public LMsgSC
{
	Lint			m_user_color;  //玩家属于哪一方
	Lint			m_cur_desk_color;	//当前该哪一方操作
	std::vector<BlockMsg>		m_chess_boards;  //所有的棋子
	


	LMsgS2CChessInfo() :LMsgSC(MSG_S_2_C_CHESS_INFO)
		,m_user_color(0),m_cur_desk_color(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_user_color), m_user_color);
		WriteKeyValue(pack, NAME_TO_STR(m_cur_desk_color), m_cur_desk_color);

		WriteKeyValue(pack, NAME_TO_STR(m_chess_boards), m_chess_boards);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CChessUpgrade(); }
};


struct LMsgS2CChessResult :public LMsgSC
{
	Lint			m_winner;  //赢得一方
	

	LMsgS2CChessResult() :LMsgSC(MSG_S_2_C_CHESS_RESULT)
		, m_winner(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_winner), m_winner);
		

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CChessResult(); }
};


//****************************************************************************

struct LMsgC2STest :public LMsgSC
{
	std::vector<MsgUserInfo> m_users;


	LMsgC2STest() :LMsgSC(MSG_C_2_S_TEST)
	{
		m_users.clear();
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		//ReadMapData(obj, NAME_TO_STR(m_users), m_users);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_users), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				MsgUserInfo v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_users.push_back(v);
			}
		}


		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_users), m_users);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2STest();
	}
};

struct LMsgS2CTest :public LMsgSC
{
	std::vector<MsgUserInfo> m_users;

	DeskMsg					m_desk;


	LMsgS2CTest() :LMsgSC(MSG_S_2_C_TEST)
	{
		m_users.clear();
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_users), m_users);
		ReadMapData(obj, NAME_TO_STR(m_desk), m_desk);


		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_users), m_users);
		WriteKeyValue(pack, NAME_TO_STR(m_desk), m_desk);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CTest();
	}
};
//****************************************************************************

#endif