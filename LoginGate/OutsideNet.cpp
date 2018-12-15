#include "OutsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "LTool.h"
#include "LSocket.h"
#include "BaseCommonFunc.h"

bool OutsideNet::Init()
{
	SetMsgHeaderChecker(boost::bind(&OutsideNet::CheckMsgHeader, this, boost::placeholders::_1, boost::placeholders::_2));
	LNet::Init(gConfig.GetOutsideIp(), gConfig.GetOutsidePort(), 4, true, false);
	return true;
}

bool OutsideNet::Final()
{
	return true;
}

void OutsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0;
	LMsg* msg = NULL;

	try
	{
		msgpack::unpacked  unpack;
		msgpack::unpack(&unpack, recv->Data() + recv->GetOffset(), recv->Size() - recv->GetOffset());
		msgpack::object obj = unpack.get();		
		ReadMapData(obj, "m_msgId", msgid);

		msg = LMsgFactory::Instance().CreateMsg(msgid);
		if(msg != NULL)
		{
			//LLOG_DEBUG("msgid = %d, body size = %d", msgid, recv->Size());
			msg->Read(obj);
			msg->m_sp = s;
			gWork.Push(msg);
		}
		else
		{
			recv->Read(msgid);
			if(msgid == 0x7FFFFFFE || msgid == 0x7FFFFFFF)
			{
				msg = LMsgFactory::Instance().CreateMsg(msgid);
				if(msg != NULL)
				{
					msg->Read(*recv.get());
					msg->m_sp = s;
					gWork.Push(msg);
				}
			}
		}
	}
	catch (...)
	{
		LLOG_ERROR("OutsideNet::RecvMsgPack error");
	}

	if(msg == NULL)
	{
		DisableIP(s->GetRemoteIp());
		s->Kick();
	}
}

bool OutsideNet::CheckMsgHeader(LSocketPtr s, const MsgHeader& header)
{
	bool succeed = false;
	if(header.bodySize >= 14 && header.bodySize <= 2048)
	{
		Lint msgId = InverseTransfromMsgId(header.timestamp, header.msgId);
		succeed = IsValidMsgId(msgId);
	}
	return succeed;
}

bool OutsideNet::IsValidMsgId(Lint id)
{
	bool result = false;

	result = result || (id == MSG_C_2_S_MSG);
	result = result || (id == MSG_C_2_S_LOGIN_TOKEN);

	return result;
}