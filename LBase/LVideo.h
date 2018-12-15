//查牌功能


#ifndef _L_VIDEO_H_
#define _L_VIDEO_H_

#include "LTool.h"
#include "LBuff.h"
#include "CardDefine.h"
#include "GameDefine.h"

enum VIDEO_OPER
{
	VIDEO_OPER_GET_CARD = 1,	//摸牌
	VIDEO_OPER_OUT_CARD,		//出牌
	VIDEO_OPER_AN_GANG,			//暗杠
	VIDEO_OPER_MING_GANG,		//我出牌明杠
	VIDEO_OPER_PENG_CARD,		//碰牌
	VIDEO_OPER_GANG,			//别人出牌，我杠
	VIDEO_OPER_SHOUPAO,			//收炮
	VIDEO_OPER_ZIMO,			//自摸
	VIDEO_OPER_HUANGZHUANG,		//流局
	VIDEO_OPER_EAT,				//吃
	VIDEO_OPER_BU_HUA,			//补花
	VIDEO_OPER_SELF_BU = 12,	//明补自已的
	VIDEO_OPER_OTHER_BU = 13,	//明补他人的
	VIDEO_OPER_AN_BU = 14,		//暗补
 	VIDEO_OPEN_THINK = 21,		//玩家思想
 	VIDEO_OPEN_DOING = 22,		//玩家决定
	VIDEO_OPEN_BIRD_SHOW =61,	//扎鸟;
	VIDEO_OPEN_SET_GA = 62,		//定噶
	VIDEO_OPEN_LIAN_ZHUANG = 63,//连庄
	VIDEO_OPEN_BAO_CARD = 64,	//宝牌
	VIDEO_OPER_HUIPAI		= 68,		//会牌
	VIDEO_OPER_BUY_ITEM		= 69,		//购买炸弹或者跑子
	
	VIDEO_OPER_TIPS	= 70,			//提示
	VIDEO_OPER_FANG_FENG = 71,			//放风
	VIDEO_OPER_ZHANG_MAO = 72,	//长毛
	VIDEO_OPER_END_INFO = 73, 
	VIDEO_OPER_QIANG_GANG_HU = 74,	//抢杠胡
};

struct VideoOper
{
	Lint		m_pos;
	Lint		m_oper;
	std::vector<CardValue> m_cards;		//第一个为目标牌（吃，碰，等），后面的是已有的牌

	MSGPACK_DEFINE(m_pos,m_oper,m_cards); 
	VideoOper()
	{
		m_pos = 0;
		m_oper = 0;
		m_cards.clear();
	}

	Lstring		ToString()
	{
		std::stringstream ss;
		ss << m_pos << ",";
		ss << m_oper << ",";
		for (int i = 0; i< m_cards.size(); i++)
		{
			ss << m_cards[i].m_color << "|" << m_cards[i].m_number;
			if (i < m_cards.size() - 1)
			{
				ss << ",";
			}
		}
		ss << ";";
		return ss.str();
	}

	void	FromString(Lstring str)
	{
		std::vector<Lstring> des;
		L_ParseString(str, des, ",");
		if (des.size() >= 3)
		{
			m_pos = atoi(des[0].c_str());
			m_oper = atoi(des[1].c_str());
			std::vector<Lstring> splids;
			for (int i = 2; i < des.size(); i ++)
			{
				L_ParseString(des[i], splids, "|");
				if (splids.size() >= 2)
				{
					CardValue card;
					card.m_color = atoi(splids[0].c_str());
					card.m_number = atoi(splids[1].c_str());
					m_cards.push_back(card);
				}
			}
		}
	}

	void	PkFromString(Lstring& str)
	{
		std::vector<Lstring> des;
		L_ParseString(str, des, ",");
		if (des.size() >= 2)
		{
			m_pos = atoi(des[0].c_str());
			m_oper = atoi(des[1].c_str());
			std::vector<Lstring> splids;
			for (Lsize i = 2; i < des.size(); i ++)
			{
				Lint value = atoi(des[i].c_str());
				CardValue card;
				card.m_color = value % 4;
				card.m_number = value / 4 + 1;
				m_cards.push_back(card);
			}
		}
	}
};

class VideoLog
{
public:
	VideoLog();
	virtual~VideoLog();
	void		AddOper(Lint oper,Lint pos, std::vector<CardValue>& cards);

	void		DealCard(Lint* userId, std::vector<CardValue>* vec, Lint time,Lint zhuang,Lint* score,Lint deskId,Lint cur,Lint max,Lint flag, std::vector<Lint>* playType);

	Lstring		ToString();

	void		FromString(Lstring str);

	void		Write(LBuff& buff);

	void		Read(LBuff& buff);

	void		Clear();

	Lstring		GetInsertSql();

	Lstring		PlayTypeToStrint();

	void		PlayTypeFromString(Lstring str);

	void        ParseStr2Card(Lstring str);
	Lint        GetUserCounts()
	{
		if(m_flag == LBase_Jing_Dian_Dou_Di_Zhu)
			return 3;

		return 4;
	}
	//录像新增的结算界面的数据显示
	void		AddResultTips(Lint oper,Lint pos, std::vector<CardValue>& cards);
	Lstring		ResultToString();
	void		FromResultString(Lstring result);
	void		FromResulrStringDouDiZhu(Lstring result);
public:
	Lint							m_time;
	Lstring							m_Id;
	Lint							m_zhuang;
	Lint							m_deskId;
	Lint							m_curCircle;
	Lint							m_maxCircle;
	Lint							m_score[4];
	Lint							m_userId[4];
	Lint							m_flag;//牌局玩法
	std::vector<CardValue>			m_handCard[4];
	std::vector<VideoOper>			m_oper;
	std::vector<Lint>				m_playType;
	Lstring							m_str;
	Lstring							m_str_playtype;
	Lint							m_nAppId;
	std::vector<VideoOper>			m_resultTips;  //结算界面显示数据
	Lstring							m_str_result_data;
	std::vector<CardValue>			m_deskCard;
};


#endif