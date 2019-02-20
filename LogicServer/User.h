#pragma once
#include "LBase.h"
#include "LSingleton.h"
#include "Work.h"

class User
{
public:
	User(int user_id,Lstring name,Lstring head_icon,int gate_server_id,bool robot);
	~User();
public:
	void	Clear();
	Lint	GetUserId();
	void	SetUserId(Lint user_id);
	

	Lint	GetStatus();
	void	SetStatus(Lint status);

	Lint	GetGateServerId()
	{
		return m_gate_id;
	}
	void	SetGateServerId(Lint gate_id)
	{
		m_gate_id = gate_id;
	}

	Lstring GetName()
	{
		return m_name;
	}

	Lstring GetHeadIcon()
	{
		return m_head_icon;
	}

	Lint GetStar()
	{
		return m_star;
	}

	void SetStar(int star)
	{
		m_star = star;
	}

	Lint GetDeskId()
	{
		return m_desk_id;
	}

	void SetDeskId(int desk_id)
	{
		m_desk_id = desk_id;
	}

	void Send(LMsgSC& msg);
	
	bool IsRobot();
	
	void SetOnline(bool online);
	bool IsOnline();

private:
	Lint	m_user_id;
	Lint	m_status;
	Lint	m_gate_id;
	Lstring	m_name;
	Lstring m_head_icon;

	Lint    m_star;

	bool	m_robot;
	int		m_desk_id;
	bool	m_online;

};

typedef std::shared_ptr<User> LUserPtr;

