#include "LUser.h"

LUser::LUser()
{
	m_id = 0;
	m_user_id = 0;
	m_sex = 1;
	m_money = 0;
	m_scores = 0;
}

LUser::~LUser()
{

}

void LUser::Read(LBuff& buff)
{
	buff.Read(m_id);
	buff.Read(m_user_id);
	buff.Read(m_name);

	buff.Read(m_sex);
	buff.Read(m_headImageUrl);
	buff.Read(m_money);
	buff.Read(m_scores);
}

void LUser::Write(LBuff& buff)
{
	buff.Write(m_id);
	buff.Write(m_user_id);
	buff.Write(m_name);

	buff.Write(m_sex);
	
	buff.Write(m_headImageUrl);
	buff.Write(m_money);
	buff.Write(m_scores);
	
}