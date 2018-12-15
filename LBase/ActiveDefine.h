#ifndef _ACTIVE_DEFINE_H_
#define _ACTIVE_DEFINE_H_

#include "LSocket.h"
#include "LTool.h"

struct UserReward
{
	Lint m_reward_id;
	std::string m_user_name;

	MSGPACK_DEFINE(m_reward_id, m_user_name); 

public:
	UserReward()
		:m_reward_id(0)
	{

	}

public:
	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_reward_id), m_reward_id);
		ReadMapData(obj, NAME_TO_STR(m_user_name), m_user_name);

		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_reward_id), m_reward_id);
		WriteKeyValue(pack, NAME_TO_STR(m_user_name), m_user_name);

		return true;
	}
};

#endif