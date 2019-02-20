#include "DUser.h"
#include "LTime.h"

DUser::DUser()
{
	m_online = true;
}

DUser::~DUser()
{
	m_online = false;
	m_sp = nullptr;
}



void DUser::SetOnline(bool online)
{
	m_online = online;
}

void DUser::SetSp(LSocketPtr sp)
{
	m_sp = sp;
}


LSocketPtr DUser::GetSp()
{
	return m_sp;
}



