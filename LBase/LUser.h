#ifndef _L_USER_H_
#define _L_USER_H_ 

#include "LBase.h"
#include "LBuff.h"


class LUser
{
public:
	LUser();
	~LUser();

	void	Read(LBuff& buff);
	void	Write(LBuff& buff);

public:
	Lint		m_id;
	Lint		m_user_id;
	Lstring		m_name;
	Lint		m_sex;
	Lstring		m_headImageUrl;
	Lint		m_money;
};

#endif