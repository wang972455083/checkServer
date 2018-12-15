#ifndef _CARD_DEFINE_H_
#define _CARD_DEFINE_H_

#include "LBuff.h"
#include <vector>

struct Card;

struct CardValue
{
	int m_color;
	int m_number;

	MSGPACK_DEFINE(m_color, m_number); 
	CardValue()
		:m_color(0)
		,m_number(0)
	{
	}

	CardValue(int color, int number)
		:m_color(color)
		,m_number(number)
	{
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_color), m_color);
		ReadMapData(obj, NAME_TO_STR(m_number), m_number);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_color), m_color);
		WriteKeyValue(pack, NAME_TO_STR(m_number), m_number);
		return true;
	}

	bool Equal(const CardValue& card_value)
	{
		return card_value.m_color == m_color && card_value.m_number == m_number;
	}
};

inline bool operator== (const CardValue& card1, const CardValue& card2)
{
	return (card1.m_color == card2.m_color && card1.m_number == card2.m_number);
}

struct Card
{
	int m_number;	//牌的号码1-9
	int m_color;   //牌的花色1-3 1-万，2-筒，3-条

	Card()
		:m_number(0)
		,m_color(0)
	{
	}

	Card(int color, int number)
		:m_color(color)
		,m_number(number)
	{
	}

	Card(const CardValue &card_value)
		:m_color(card_value.m_color)
		,m_number(card_value.m_number)
	{

	}

public:
	bool Equal(const Card & card) const
	{
		return card.m_number == m_number && card.m_color == m_color;
	}

	bool IsEffectiveCard()
	{
		return true;
	}
};

inline bool operator== (const Card& card1, const Card& card2)
{
	return (card1.m_color == card2.m_color && card1.m_number == card2.m_number);
}

inline bool operator< (const Card& card1, const Card& card2)
{
	if (card1.m_color == card2.m_color)
	{
		return (card1.m_number < card2.m_number);
	}

	return (card1.m_color < card2.m_color);
}

typedef std::vector<Card*> CardVector;
typedef std::vector<Card>  EntiryCardVector; 
typedef std::vector<Card>::const_iterator  EntiryCardVectorItor;


struct ThinkUnit
{
	ThinkUnit()
	{
		m_type = 0;
	}

	void Clear()
	{
		m_type = 0;
		m_card.clear();
		m_hu.clear();
	}
	Lint m_type;
	std::vector<Card*>	m_card;
	std::vector<Lint> m_hu;//胡牌的类型
};

typedef std::vector<ThinkUnit> ThinkVec;


struct StartHuCard
{
	StartHuCard()
	{
		m_type = 0;
	}

	void	Clear()
	{
		m_type = 0;
		m_card.clear();
	}
	Lint m_type;
	std::vector<Card*> m_card;
};
typedef std::vector<StartHuCard> StartHuVector;

struct PosRel
{
	//两个位置直接的操作关系
	enum OperType
	{
		OT_Invaild = 0,	//无效
		OT_Chi,			//吃
		OT_Peng,		//碰
		OT_Dian_gang,	//点杠
		OT_Dian_pao,	//胡
	};

	int m_in_pos;			//入牌位置
	int m_out_pos;			//出牌位置
	OperType m_oper_type;	//操作类型
	Card m_card;			//操作的牌

	//MSGPACK_DEFINE(m_in_pos, m_out_pos, (int)m_oper_type, m_card.m_color, m_card.m_number);
public:
	PosRel()
		:m_in_pos(0)
		,m_out_pos(0)
		,m_oper_type(OT_Invaild)
	{

	}

	PosRel(int in_pos, int out_pos, OperType oper_type, const Card &card)
		:m_in_pos(in_pos)
		,m_out_pos(out_pos)
		,m_oper_type(oper_type)
		,m_card(card)
	{

	}
};

typedef std::vector<PosRel> PosRelVec;

struct TingCard
{
	Card m_card;			//要胡的那张牌
	std::vector<int> m_hu;	//胡的番型
	int m_hufan;			//胡的番数 包括根和最大番限制
	int m_reserve_count;

public:
	TingCard()
		: m_hufan(0)
		,m_reserve_count(0)
	{
	}
};

typedef std::vector<TingCard> TingCardVec;

struct TingUnit
{
	Card m_card;						//打出的那张牌
	std::vector<TingCard> m_tingcard;	//听的牌
};

typedef std::vector<TingUnit> TingUnitVec;

#endif