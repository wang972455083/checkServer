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

	MSG_C_2_S_QUICK_ROOM_OPT = 1003,	//快速加入房间操作
	MSG_S_2_C_QUICK_ROOM_OPT = 1004,	//

	MSG_S_2_C_QUICK_ROOM_RESULT = 1005,	//快速创建房间结果

	MSG_C_2_S_NOTICE_LOGIN = 1006,			//通知登录

	MSG_TO_LOGIC_MANAGER_MAX_MSG = 2000,   //以上是转发到LogicManager上的







	

	

	MSG_C_2_S_CREATE_DESK_ASK = 3001,			//请求创建桌子
	MSG_S_2_C_NOTICE_CREATE_DESK_ASK = 3002,	//通知请求建桌子
	MSG_C_2_S_CREATE_DESK_RESPON = 3003,		//回复创建桌子
	MSG_S_2_C_CREATE_DESK_RESULT = 3004,	//创建桌子结果

	MSG_C_2_S_SELECT_CARD			= 3005,		//选择卡牌
	MSG_S_2_C_SELECT_CARD			= 3006,		//选择卡牌

	MSG_S_2_C_ROOM_INFO_MODIFY		= 3007,		//房间信息修改
	MSG_S_2_C_DESK_RESULT		= 3008,		//桌子比试结果

	MSG_TO_LOGIC_SERVER_MAX_MSG = 4000,		//以上是转发到LogicServer的


	MSG_C_2_S_HEART = 4001, //心跳包
	MSG_S_2_C_HEART = 4002, //心跳包

	MSG_S_2_S_MODIFY_COIN = 4003,  //金币修改

	
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


		
	MSG_LM_2_L_QUICK_CREATE_ROOM_OPT = 20003,  //快速创建房间
	MSG_L_2_LM_QUICK_CREATE_ROOM_OPT = 20004,

	MSG_LM_2_G_USER_STATUS_MODIFY = 20005,//  大厅和房间状态修改
	MSG_LM_2_LM_QUICK_CREATE_ROOM = 20006,//  创建房间

	

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

struct MsgParter
{
	MsgParter()
	{
		m_user_id = 0;
		m_card = 0;
	}


	Lint m_user_id;
	Lint m_card;	//选择的牌

	MSGPACK_DEFINE(m_user_id,m_card);
};

struct MsgDeskInfo
{
	std::vector<MsgParter> m_parters;
	Lint					m_star;

	MsgDeskInfo()
	{
		m_parters.clear();
		m_star = 0;
	}

	MSGPACK_DEFINE(m_parters, m_star);
};


struct RoomUser
{
	int   m_user_id;
	Lstring m_name;
	Lstring m_head_icon;
	Lint    m_star;		//金币数
	Lint	m_desk_id;
	
	MSGPACK_DEFINE(m_user_id,m_name, m_head_icon, m_star,m_desk_id);

public:
	RoomUser()
	{
		m_user_id = 0;
		m_star = 0;
		m_desk_id = 0;
		
	}

	RoomUser& operator = (const RoomUser& other)
	{
		if (&other == this)
			return *this;

		m_user_id = other.m_user_id;
		m_name = other.m_name;
		m_head_icon = other.m_head_icon;
		m_star = other.m_star;
		m_desk_id = other.m_desk_id;

		return *this;
	}
};

struct RoomMsg
{
	Lint	m_room_id;

	std::vector<RoomUser>	m_users;
	std::vector<MsgDeskInfo>	m_desk_infos;

	MSGPACK_DEFINE(m_room_id, m_users, m_desk_infos);

public:
	RoomMsg() :m_room_id(0)
	{
		m_users.clear();
		m_desk_infos.clear();
	}
};


//快速加入
struct LMsgC2SQuickRoomOpt :public LMsgSC
{
	Lint	m_type;			// 0，快速加入，  1取消加入	
	Lstring	m_name;
	Lstring m_head_icon;

	

	LMsgC2SQuickRoomOpt() :LMsgSC(MSG_C_2_S_QUICK_ROOM_OPT)
		, m_type(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
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
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_name), m_name);

		WriteKeyValue(pack, NAME_TO_STR(m_head_icon), m_head_icon);
		
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SQuickRoomOpt();
	}
};




struct LMsgS2CQuickRoomOpt :public LMsgSC
{
	Lint		m_result;//0-成功,1,失败
	Lint		m_type;

	LMsgS2CQuickRoomOpt() :LMsgSC(MSG_S_2_C_QUICK_ROOM_OPT)
		, m_result(0),m_type(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_result), m_result);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CQuickRoomOpt(); }
};

struct LMsgS2CQuickRoomResult :public LMsgSC
{
	Lint		m_result;//0-成功,1,失败
	RoomMsg		m_room;

	LMsgS2CQuickRoomResult() :LMsgSC(MSG_S_2_C_QUICK_ROOM_RESULT)
		, m_result(0)
	{}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_result), m_result);
		ReadMapData(obj, NAME_TO_STR(m_room), m_room);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_room), m_room);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CQuickRoomOpt(); }
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

//****************************************************************************

struct LMsgC2STest :public LMsgSC
{
	std::vector<RoomUser> m_users;


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
				RoomUser v;
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


	RoomMsg					m_desk;


	LMsgS2CTest() :LMsgSC(MSG_S_2_C_TEST)
	{
		
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		
		ReadMapData(obj, NAME_TO_STR(m_desk), m_desk);


		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		
		WriteKeyValue(pack, NAME_TO_STR(m_desk), m_desk);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CTest();
	}
};
//*****************************************************************************LogicServer

struct LMsgC2SCreateDeskAsk :public LMsgSC
{
	Lint	m_room_id;
	Lint	m_opponent_id;   //对手id
	Lint	m_star;			 //竞技筹码星星数


	LMsgC2SCreateDeskAsk() :LMsgSC(MSG_C_2_S_CREATE_DESK_ASK)
	{
		m_room_id = 0;
		m_opponent_id = 0;
		m_star = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		
		ReadMapData(obj, NAME_TO_STR(m_room_id), m_room_id);
		//LLOG_ERROR("LMsgC2SCreateDeskAsk m_room_id = %d", m_room_id);
		ReadMapData(obj, NAME_TO_STR(m_opponent_id), m_opponent_id);
		ReadMapData(obj, NAME_TO_STR(m_star), m_star);
	
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_room_id), m_room_id);
		WriteKeyValue(pack, NAME_TO_STR(m_opponent_id), m_opponent_id);
		WriteKeyValue(pack, NAME_TO_STR(m_star), m_star);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SCreateDeskAsk();
	}
};

struct LMsgS2CNoticeCreateDeskAsk :public LMsgSC
{
	Lint	m_room_id;
	Lint	m_opponent_id;   //对手id
	Lint	m_star;			 //竞技筹码星星数


	LMsgS2CNoticeCreateDeskAsk() :LMsgSC(MSG_S_2_C_NOTICE_CREATE_DESK_ASK)
	{
		m_room_id = 0;
		m_opponent_id = 0;
		m_star = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_room_id), m_room_id);
		ReadMapData(obj, NAME_TO_STR(m_opponent_id), m_opponent_id);
		ReadMapData(obj, NAME_TO_STR(m_star), m_star);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_room_id), m_room_id);
		WriteKeyValue(pack, NAME_TO_STR(m_opponent_id), m_opponent_id);
		WriteKeyValue(pack, NAME_TO_STR(m_star), m_star);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CNoticeCreateDeskAsk();
	}
};


struct LMsgC2SCreateDeskRespon :public LMsgSC
{
	Lint		m_result;		 // 0,同意 1，拒绝
	Lint		m_room_id;
	Lint		m_opponent_id;   //对手id
	Lint		m_star;			 //竞技筹码星星数


	enum 
	{
		RS_AGREE = 0,
		RS_REFUSE = 1,
	};


	LMsgC2SCreateDeskRespon() :LMsgSC(MSG_C_2_S_CREATE_DESK_RESPON)
	{
		m_result = 0;
		m_room_id = 0;
		m_opponent_id = 0;
		m_star = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_result), m_result);
		ReadMapData(obj, NAME_TO_STR(m_room_id), m_room_id);
		ReadMapData(obj, NAME_TO_STR(m_opponent_id), m_opponent_id);
		ReadMapData(obj, NAME_TO_STR(m_star), m_star);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_room_id), m_room_id);
		WriteKeyValue(pack, NAME_TO_STR(m_opponent_id), m_opponent_id);
		WriteKeyValue(pack, NAME_TO_STR(m_star), m_star);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SCreateDeskRespon();
	}
};

struct MsgDeskUser
{

	MsgDeskUser()
	{
		m_user_id = 0;
		m_select_card = 0;
		m_tip_card = 0;
		m_pos = 0;
	}


	int   m_user_id;
	int   m_select_card;
	int   m_tip_card;
	int    m_pos;		//金币数

	MSGPACK_DEFINE(m_user_id, m_select_card, m_tip_card, m_pos);

};

struct MsgDesk
{

	MsgDesk()
	{
		m_desk_id = 0;
		m_star = 0;
		m_cur_pos = 0;
		m_desk_users.clear();
		m_select_left_time = 0;
		m_type = 0;
	}

	int  m_desk_id;
	int  m_star;
	int  m_cur_pos;
	std::vector<MsgDeskUser>	m_desk_users;
	int	m_select_left_time;	 //选择时间
	int m_type;

	MSGPACK_DEFINE(m_desk_id, m_star, m_cur_pos, m_desk_users, m_select_left_time);
};

struct LMsgS2CNoticeCreateDeskResult :public LMsgSC
{
	Lint		m_result;		 // 0,同意 1，拒绝 2,星星不足，3，对方已经在竞赛中
	MsgDesk		m_desk;


	enum
	{
		RS_AGREE = 0,
		RS_REFUSE = 1,

		RS_STAR_ERROR = 2,
		RS_IN_DESK = 3,
	};


	LMsgS2CNoticeCreateDeskResult() :LMsgSC(MSG_S_2_C_CREATE_DESK_RESULT)
	{
		m_result = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_result), m_result);
		ReadMapData(obj, NAME_TO_STR(m_desk), m_desk);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_desk), m_desk);
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CNoticeCreateDeskResult();
	}
};




struct LMsgS2CRoomInfoModify :public LMsgSC
{
	std::vector<RoomUser>			m_users;		//修改的user休息
	MsgDeskInfo						m_desk_info;	//增加的桌子比赛信息


	LMsgS2CRoomInfoModify() :LMsgSC(MSG_S_2_C_ROOM_INFO_MODIFY)
	{

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_users), m_users);
		ReadMapData(obj, NAME_TO_STR(m_desk_info), m_desk_info);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);

		WriteKeyValue(pack, NAME_TO_STR(m_users), m_users);
		WriteKeyValue(pack, NAME_TO_STR(m_desk_info), m_desk_info);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CRoomInfoModify();
	}
};


struct LMsgC2SSelectCard :public LMsgSC
{
	Lint		m_room_id;
	Lint		m_desk_id;
	Lint		m_card;

	LMsgC2SSelectCard() :LMsgSC(MSG_C_2_S_SELECT_CARD)
	{
		m_room_id = 0;
		m_desk_id = 0;
		m_card = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_room_id), m_room_id);
		ReadMapData(obj, NAME_TO_STR(m_desk_id), m_desk_id);
		ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_room_id), m_room_id);
		WriteKeyValue(pack, NAME_TO_STR(m_desk_id), m_desk_id);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SSelectCard();
	}
};

struct LMsgS2CSelectCard :public LMsgSC
{
	Lint			m_oper_user_id;		//操作的uid
	Lint			m_card;
	LMsgS2CSelectCard() :LMsgSC(MSG_S_2_C_SELECT_CARD)
	{
		m_oper_user_id = 0;
		m_card = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_oper_user_id), m_oper_user_id);
		ReadMapData(obj, NAME_TO_STR(m_card), m_card);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_oper_user_id), m_oper_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CSelectCard();
	}
};

struct LMsgS2CDeskResult :public LMsgSC
{
	Lint			m_desk_id;
	Lint			m_winner;		//0是平局 其他是赢家
	
	LMsgS2CDeskResult() :LMsgSC(MSG_S_2_C_DESK_RESULT)
	{
		m_winner = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_user_id), m_user_id);
		ReadMapData(obj, NAME_TO_STR(m_desk_id), m_desk_id);
		ReadMapData(obj, NAME_TO_STR(m_winner), m_winner);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_user_id), m_user_id);
		WriteKeyValue(pack, NAME_TO_STR(m_desk_id), m_desk_id);
		WriteKeyValue(pack, NAME_TO_STR(m_winner), m_winner);
		

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CDeskResult();
	}
};

//*******************************************************************


//*****************************************************************************

#endif