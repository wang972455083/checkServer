#include "User.h"


User::User(int user_id,Lstring name,Lstring head_icon,int gate_server_id,bool robot)
{
	m_user_id = user_id;
	m_name = name;
	m_head_icon = head_icon;

	m_status = 0;
	m_gate_id = gate_server_id;

	m_star = 10;

	m_robot = robot;
	

	m_desk_id = 0;
}

User::~User()
{
	Clear();
}

void User::Clear()
{
	m_user_id = 0;
	m_name = "";

	m_status = 0;
}

Lint User::GetUserId()
{
	return m_user_id;
}


void User::SetUserId(Lint user_id)
{
	m_user_id = user_id;
}



Lint User::GetStatus()
{
	return m_status;
}

void User::SetStatus(Lint status)
{
	m_status = status;
}


void User::Send(LMsgSC& msg)
{
	msg.m_user_id = m_user_id;

	GateInfo* info = gWork.GetGateInfoById(m_gate_id);
	if (info)
	{
		LMsgL2GUserMsg sendMsg;
		sendMsg.m_user_id = m_user_id;
		sendMsg.m_user_msg_id = msg.m_msgId;
		sendMsg.m_dataBuff = msg.GetSendBuff();

		info->m_sp->Send(sendMsg.GetSendBuff());
	}
}








