#ifndef _INSIDE_NET_H_
#define _INSIDE_NET_H_

#include <unordered_set>
#include "LNet.h"
#include "LSingleton.h"
#include "LSocket.h"

class OutsideNet :public LNet, public LSingleton<OutsideNet>
{
public:
	virtual bool Init();
	virtual bool Final();

	virtual void RecvMsgPack(LBuffPtr recv,LSocketPtr s);

private:
	bool		CheckMsgHeader(LSocketPtr s, const MsgHeader& header);
	void		CreateMsgIdTable();
	bool		IsValidMsgId(Lint id);

private:

private:
	std::unordered_set<Lint>		m_msgIdTable;
};

#define gOutsideNet OutsideNet::Instance()

#endif