#include "LCharge.h"


LCharge::LCharge()
{
	m_userId = 0;
	m_time = 0;
	m_cardNum = 0;
	m_operType = 0;
	m_balance = 0;
	m_nAppId = 0;
}

LCharge::~LCharge()
{

}

void LCharge::Write(LBuff& buff)
{
	buff.Write(m_userId);
	buff.Write(m_time);
	buff.Write(m_cardNum);
	buff.Write(m_operType);
	buff.Write(m_balance);
	buff.Write(m_operator);
	buff.Write(m_remark);
	buff.Write(m_nAppId);
}

void LCharge::Read(LBuff& buff)
{
	buff.Read(m_userId);
	buff.Read(m_time);
	buff.Read(m_cardNum);
	buff.Read(m_operType);
	buff.Read(m_balance);
	buff.Read(m_operator);
	buff.Read(m_remark);
	buff.Read(m_nAppId);
}