#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "LTool.h"


bool OutsideNet::Init()
{
	LNet::Init(gConfig.GetOutsideIp(), gConfig.GetOutsidePort(), 4, true, false);
	return true;
}

bool OutsideNet::Final()
{
	return true;
}

void OutsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0 ;

	try{
		msgpack::unpacked  unpack;
		msgpack::unpack(&unpack, recv->Data() + recv->GetOffset(), recv->Size() - recv->GetOffset());
		msgpack::object obj = unpack.get();		
		ReadMapData(obj, "m_msgId", msgid);

		LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
		if(msg != NULL)
		{
			msg->Read(obj);
			msg->m_sp = s;
			gWork.Push(msg);
			//LLOG_DEBUG("InsideNet::RecvMsgPack user recv msgid=%d",msgid);
		}
		else
		{
			recv->Read(msgid);
			msg = LMsgFactory::Instance().CreateMsg(msgid);
			if (msg)
			{
				msg->Read(*recv.get());
				msg->m_sp = s;
				gWork.Push(msg);
				//LLOG_DEBUG("InsideNet::RecvMsgPack recv msgid=%d", msgid);
			}
			else
			{
				LLOG_ERROR("InsideNet::RecvMsgPack error msgId=%d", msgid);
			}
		}
	}
	catch (...)
	{
		LLOG_ERROR("InsideNet::RecvMsgPack error");
	}
}


//////////////////////////////////////////////////////////////////////////

bool GmNet::Init()
{
	LNet::Init(gConfig.GetInsideIp(), gConfig.GetInsidePort(), 1, true, true);
	return true;
}

bool GmNet::Final()
{
	return true;
}

void GmNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0;
	recv->Read(msgid);
	LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
	if (msg != NULL)
	{
		msg->Read(*recv.get());
		msg->m_sp = s;
		gWork.Push(msg);
		LLOG_DEBUG("GmNet::RecvMsgPack recv msgid=%d", msgid);
	}
	else
	{
		LLOG_ERROR("GmNet::RecvMsgPack error msgId=%d", msgid);
	}
}
