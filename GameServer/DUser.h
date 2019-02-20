#ifndef _D_USER_H_
#define _D_USER_H_


#include "LUser.h"
#include "LSocket.h"

class DUser
{
public:
	DUser();
	~DUser();

	void SetOnline(bool online);
	void SetSp(LSocketPtr sp);
	LSocketPtr GetSp();
	
public:
	LUser m_usert;
	bool  m_online;
	LSocketPtr	m_sp;
	
};


#endif