#include "LVideo.h"

VideoLog::VideoLog() : m_Id("")
{
	memset(m_userId, 0, sizeof(m_userId));
	m_time = 0;
	m_curCircle = 0;
	m_maxCircle = 0;
	m_deskId = 0;
	memset(m_score,0,sizeof(m_score));
	m_flag = 0;
	m_zhuang = 0;
	m_nAppId = 0;
	m_deskCard.clear();
}

VideoLog::~VideoLog()
{

}

void VideoLog::AddOper(Lint oper, Lint pos, std::vector<CardValue>& cards)
{
	VideoOper o;
	o.m_oper = oper;
	o.m_pos = pos;
	for (int i = 0; i < cards.size(); i ++)
	{
		o.m_cards.push_back(cards[i]);
	}
	m_oper.push_back(o);
}


void VideoLog::DealCard(Lint* userId, std::vector<CardValue>* vec,Lint time,Lint zhuang,Lint* score,Lint deskId,Lint cur,Lint max,Lint flag, std::vector<Lint>* playType)
{
	memcpy(m_userId, userId, sizeof(m_userId));
	memcpy(m_score,score,sizeof(m_score));
	for (Lint i = 0; i < 4; ++i)
	{
		m_handCard[i] = vec[i];
	}
	m_time = time;
	m_deskId = deskId;
	m_maxCircle = max;
	m_curCircle = cur;
	m_zhuang = zhuang;
	m_flag = flag;
	auto itype = playType->begin();
	for (; itype != playType->end(); ++itype)
	{
		m_playType.push_back( *itype );
	}
}

Lstring	VideoLog::ToString()
{
	std::stringstream ss;

	for (Lint i = 0; i < 4; ++i)
	{
		for (Lint j = 0; j < m_handCard[i].size(); ++j)
		{
			ss << m_handCard[i][j].m_color << "," << m_handCard[i][j].m_number;
			if (j + 1 == m_handCard[i].size())
			{
				ss << ";";
			}
			else
			{
				ss << ",";
			}
		}
		if (m_handCard[i].empty())
		{
			ss << ";";
		}
	}

	for (Lint i = 0; i < m_oper.size(); ++i)
	{
		ss << m_oper[i].ToString();
	}
	return ss.str();
}

void VideoLog::FromString(Lstring str)
{
	std::vector<Lstring> des,subDes;
	L_ParseString(str, des, ";");
	if (des.size() >= 4)
	{
		for (Lint i = 0; i < 4; ++i)
		{
			subDes.clear();
			L_ParseString(des[i], subDes, ",");
			for (Lint j = 0; j+1 < subDes.size(); j+=2)
			{
				CardValue v;
				v.m_color = atoi(subDes[j].c_str());
				v.m_number = atoi(subDes[j+1].c_str());
				m_handCard[i].push_back(v);
			}
		}

		for (Lint i = 4; i < des.size(); ++i)
		{
			VideoOper o;
			o.FromString(des[i]);
			if (o.m_oper != 0)
			{
				m_oper.push_back(o);
			}
		}
	}
}

void VideoLog::AddResultTips(Lint oper, Lint pos, std::vector<CardValue>& cards)
{
	VideoOper o;
	o.m_oper = oper;
	o.m_pos = pos;
	for (int i = 0; i < cards.size(); i ++)
	{
		o.m_cards.push_back(cards[i]);
	}
	m_resultTips.push_back(o);
}

Lstring VideoLog::ResultToString()
{
	std::stringstream ss;

	if(m_deskCard.size())
	{
		for (Lint j = 0; j < m_deskCard.size(); ++j)
		{
			ss << m_deskCard[j].m_color << "," << m_deskCard[j].m_number;
			if (j + 1 == m_deskCard.size())
			{
				ss << ";";
			}
			else
			{
				ss << ",";
			}
		}
	}
	else
	{
		ss<<";";
	}


	for (Lint i = 0; i < m_resultTips.size(); ++i)
	{
		ss << m_resultTips[i].ToString();
	}
	return ss.str();
}

void VideoLog::FromResultString(Lstring result)
{
	std::vector<Lstring> des,subDes;
	L_ParseString(result, des, ";");
	if (des.size())
	{
		subDes.clear();
		L_ParseString(des[0], subDes, ",");
		for (Lint j = 0; j+1 < subDes.size(); j+=2)
		{
			CardValue v;
			v.m_color = atoi(subDes[j].c_str());
			v.m_number = atoi(subDes[j+1].c_str());
			m_deskCard.push_back(v);
		}

		for (Lint i = 1; i < des.size(); ++i)
		{
			VideoOper o;
			o.FromString(des[i]);
			if (o.m_oper != 0)
			{
				m_resultTips.push_back(o);
			}
		}
	}
}

void	VideoLog::FromResulrStringDouDiZhu(Lstring result)
{
	std::vector<Lstring> des;
	L_ParseString(result, des, ";");
	if (des.size())
	{
		for (Lint i = 0; i < des.size(); ++i)
		{
			
			std::vector<Lstring> str;
			L_ParseString(des[i], str, ",");
			if(str.size()>=3)
			{
				VideoOper o;
				o.m_pos = atoi(str[0].c_str());
				o.m_oper = atoi(str[1].c_str());

				CardValue card;
				card.m_number = atoi(str[2].c_str());

				o.m_cards.push_back(card);

				m_resultTips.push_back(o);
			}
		}
	}
}

void VideoLog::Write(LBuff& buff)
{
	buff.Write(m_Id);
	buff.Write(m_time);
	buff.Write(m_userId[0]);
	buff.Write(m_userId[1]);
	buff.Write(m_userId[2]);
	buff.Write(m_userId[3]);
	buff.Write(m_zhuang);
	buff.Write(m_deskId);
	buff.Write(m_curCircle);
	buff.Write(m_maxCircle);
	buff.Write(m_score[0]);
	buff.Write(m_score[1]);
	buff.Write(m_score[2]);
	buff.Write(m_score[3]);
	buff.Write(m_flag);

	buff.Write(m_str);
	buff.Write(m_str_playtype);
	buff.Write(m_nAppId);
	buff.Write(m_str_result_data);
}

void VideoLog::Read(LBuff& buff)
{
	buff.Read(m_Id);
	buff.Read(m_time);
	buff.Read(m_userId[0]);
	buff.Read(m_userId[1]);
	buff.Read(m_userId[2]);
	buff.Read(m_userId[3]);
	buff.Read(m_zhuang);
	buff.Read(m_deskId);
	buff.Read(m_curCircle);
	buff.Read(m_maxCircle);
	buff.Read(m_score[0]);
	buff.Read(m_score[1]);
	buff.Read(m_score[2]);
	buff.Read(m_score[3]);
	buff.Read(m_flag);
	m_str.clear();
	buff.Read(m_str);
	m_str_playtype.clear();
	buff.Read(m_str_playtype);
	buff.Read(m_nAppId);
	m_str_result_data.clear();
	buff.Read(m_str_result_data);
}

void VideoLog::Clear()
{
	m_time = 0;
	m_Id = "";
	memset(m_userId, 0, sizeof(m_userId));
	for (Lint i = 0; i < 4; ++i)
	{
		m_handCard[i].clear();
	}
	m_oper.clear();
	m_deskCard.clear();
	m_resultTips.clear();
	m_str.clear();
	m_str_playtype.clear();
	m_str_result_data.clear();
}

Lstring VideoLog::GetInsertSql()
{
	std::stringstream ss;
	ss << "INSERT INTO Video (Id,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,Flag,Data, PlayType,serverID,resultData) VALUES (";
	ss << "'" << m_Id << "',";
	ss << "'" << m_time << "',";
	ss << "'" << m_userId[0] << "',";
	ss << "'" << m_userId[1] << "',";
	ss << "'" << m_userId[2] << "',";
	ss << "'" << m_userId[3] << "',";

	ss << "'" << m_zhuang << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << m_maxCircle << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << m_score[0] << "',";
	ss << "'" << m_score[1] << "',";
	ss << "'" << m_score[2] << "',";
	ss << "'" << m_score[3] << "',";
	ss << "'" << m_flag << "',";

	ss << "'" << ToString() << "',";
	ss << "'" << PlayTypeToStrint() << "',";
	ss << "'" << m_nAppId << "',";
	ss << "'" << ResultToString() << "')";
	return ss.str();
}

Lstring VideoLog::PlayTypeToStrint()
{
	std::stringstream ss;

	for (Lint i = 0; i < m_playType.size(); ++i)
	{
		ss << m_playType[i] <<";";
	}
	return ss.str();
}

void VideoLog::PlayTypeFromString(Lstring str)
{
	std::vector<Lstring> des;
	L_ParseString(str, des, ";");
	if (des.size() > 0)
	{
		for(Lint j = 0 ; j < des.size(); ++j)
		{
			m_playType.push_back( atoi(des[j].c_str()));
		}
	}
}

void VideoLog::ParseStr2Card(Lstring str)
{
	std::vector<Lstring> des,subDes;
	L_ParseString(str, des, ";");

	Lint userCount = GetUserCounts();

	if (des.size() >= userCount)
	{
		for (Lint i = 0; i < userCount; ++i)
		{
			subDes.clear();
			L_ParseString(des[i], subDes, ",");
			for (Lint j = 0; j < subDes.size(); j+=1)
			{
				CardValue v;
				Lint value = atoi(subDes[j].c_str());
				v.m_color = value % 4;		//客户端需求
				v.m_number = value / 4 + 1;
				m_handCard[i].push_back(v);
			}
		}

		for (Lint i = userCount; i < des.size(); ++i)
		{
			VideoOper o;
			o.PkFromString(des[i]);
			m_oper.push_back(o);
		}
	}

}
