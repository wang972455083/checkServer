#pragma once
#include "LBase.h"
#include "LSingleton.h"

#include <vector>



class User
{
public:
	User();
	~User();
public:
	void	Clear();
	Lint	GetUserId();
	void	SetUserId(Lint user_id);
	

	Lint	GetStatus();
	void	SetStatus(Lint status);

private:
	Lint	m_user_id;
	Lint	m_status;
	std::vector<Lint>	m_init_chess;  //上场的棋子
	
};

typedef std::shared_ptr<User>	LUserPtr;

