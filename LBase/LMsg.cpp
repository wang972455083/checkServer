#include "LMsg.h"
#include "LLog.h"
#include "LMsgS2S.h"


LMsg::LMsg(int id)
:m_msgId(id), m_isMsgPack(false)
{

}

LMsg::~LMsg()
{

}

LBuffPtr LMsg::GetSendBuff()
{
	if(!m_send)
	{
		m_send.reset(new LBuff());
		if(preAllocSendMemorySize() > BUFF_SIZE)
		{
			m_send->Alloc(preAllocSendMemorySize());
		}

		if (m_isMsgPack)
		{
			msgpack::sbuffer buffer;
			msgpack::packer<msgpack::sbuffer> pac(&buffer);
			Write(pac);

			MsgHeader header = { (unsigned short)(buffer.size()), 0, 0, m_msgId };
			m_send->Write((char*)&header, sizeof(header));
			m_send->Write(buffer.data(), buffer.size());
		}
		else
		{
			LBuff tmpBuff;
			WriteMsgId(tmpBuff);
			Write(tmpBuff);

			MsgHeader header = { (unsigned short)(tmpBuff.Size()), 0, 0, m_msgId };
			m_send->Write((char*)&header, sizeof(header));
			m_send->Write(tmpBuff.Data(), tmpBuff.Size());
		}
	}
	return m_send;
}

void LMsg::WriteMsgId(LBuff& buff)
{
	buff.Write(m_msgId);
}

void LMsg::ReadMsgId(LBuff& buff)
{
	buff.Read(m_msgId);
}

bool LMsg::Read(LBuff& buff)
{
	return true;
}

bool LMsg::Read(msgpack::object& pack)
{
	return true;
}

bool LMsg::Write(LBuff& buff)
{
	return true;
}

bool LMsg::Write(msgpack::packer<msgpack::sbuffer>& pack)
{
	return true;
}

LMsg* LMsg::Clone()
{
	return NULL;
}

Lint LMsg::preAllocSendMemorySize()
{
	return BUFF_SIZE;
}

//////////////////////////////////////////////////////////////////////////
bool LMsgFactory::Init()
{
	Final();

	RegestMsg(new LMsgIn());
	RegestMsg(new LMsgKick());
	RegestMsg(new LMsgHttp());

	RegestMsg(new LMsgC2SMsg());
	RegestMsg(new LMsgS2CMsg());

	RegestMsg(new LMsgC2SHeart());
	RegestMsg(new LMsgS2CHeart());

	RegestMsg(new LMsgC2SLogin());
	RegestMsg(new LMsgS2CLogin());

	//RegestMsg(new LMsgC2SDeskOpt());
	//RegestMsg(new LMsgS2CDeskOpt());

	RegestMsg(new LMsgC2SNoticeLogin());

	RegestMsg(new LMsgG2ServerLogin());
	RegestMsg(new LMsgG2LUserMsg());
	RegestMsg(new LMsgL2GUserMsg());
	
	

	RegestMsg(new LMsgG2LUserOutMsg());
	RegestMsg(new LMsgL2GUserOutMsg());

	

			
	
	RegestMsg(new LMsgL2LMGLogin());
	RegestMsg(new LMsgLMG2GateLogicInfo());

	RegestMsg(new LMsgS2SKeepAlive());
	RegestMsg(new LMsgS2SKeepAliveAck());

	RegestMsg(new LMsgGame2LMGLogin());
	RegestMsg(new LMsgLMG2GateGameServerInfo());
	RegestMsg(new LMsgG2GameUserMsg());
	RegestMsg(new LMsgGame2GUserMsg());
	RegestMsg(new LMsgG2LMUserMsg());
	RegestMsg(new LMsgLM2GUserMsg());

	//RegestMsg(new LMsgLM2LDeskOpt());
	//RegestMsg(new LMsgL2LMDeskOpt());

	RegestMsg(new LMsgC2SLMLogin());
	RegestMsg(new LMsgS2CLMLogin());

	RegestMsg(new LMsgC2STest());
	RegestMsg(new LMsgS2CTest());

	RegestMsg(new LMsgC2SQuickRoomOpt());
	RegestMsg(new LMsgS2CQuickRoomOpt());

	RegestMsg(new LMsgS2CQuickRoomResult());

	RegestMsg(new LMsgLM2LMQuckCreateRoom());

	RegestMsg(new LMsgLM2LQuickCreateRoomOpt());
	RegestMsg(new LMsgL2LMQuickCreateRoomOpt());

	RegestMsg(new LMsgC2SCreateDeskAsk());
	RegestMsg(new LMsgS2CNoticeCreateDeskAsk());
	RegestMsg(new LMsgC2SCreateDeskRespon());
	RegestMsg(new LMsgS2CNoticeCreateDeskResult());



	RegestMsg(new LMsgLM2GUserStatusModify());

	return true;
}

bool LMsgFactory::Final()
{
	std::map<int,LMsg*>::iterator it = m_msgMap.begin();
	for(;it != m_msgMap.end();++it)
	{
		delete it->second;
	}
	return true;
}

void LMsgFactory::RegestMsg(LMsg* pMsg)
{
	std::map<int,LMsg*>::iterator it = m_msgMap.find(pMsg->m_msgId);
	if(it != m_msgMap.end())
	{
		LLOG_INFO("LMsgFactory::RegestMsg msg exiset msgId:%d",pMsg->m_msgId);
	}
	else
	{
		m_msgMap[pMsg->m_msgId] = pMsg;
	}
}

LMsg* LMsgFactory::CreateMsg(int msgId)
{
	std::map<int,LMsg*>::iterator it = m_msgMap.find(msgId);
	if(it == m_msgMap.end())
	{
		return NULL;
	}
	else
	{
		return it->second->Clone();
	}
}
