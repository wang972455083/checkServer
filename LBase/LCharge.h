#ifndef _L_CHARGE_H_
#define _L_CHARGE_H_

#include "LBuff.h"



//enum CARDS_OPER_TYPE
//{
//	//CARDS_OPER_TYPE_INIT = 0,		        //新手赠送-
//	//CARDS_OPER_TYPE_CHARGE = 1,		    //充值活动-
//	//CARDS_OPER_TYPE_FREE_GIVE = 2,	    //免费赠送-
//	//CARDS_OPER_TYPE_CREATE_ROOM = 3,      //创建房间扣除-
//	//CARDS_OPER_TYPE_ACTIVE = 4,		    //活动送房卡-
//	//CARDS_OPER_TYPE_DAILY_LOGIN = 5,      //每日登录送房卡-
//	//CARDS_OPER_TYPE_LUCK_DRAW = 6,	    //抽奖扣卡-
//	//CARDS_OPER_TYPE_PHONE_NUM = 7,	    //绑定手机号送房卡-
//	//CARDS_OPER_TYPE_FEN_XIANG = 8,		//分享送房卡-
//	//CARDS_OPER_TYPE_MATCH = 9,            //参加比赛扣除-
//	//CARDS_OPER_TYPE_MATCH_REWARD=10,      //参加比赛奖励-
//	//CARDS_OPER_TYPE_CHANGEPRICE  = 100,   //房卡价格调整补偿-
//};

class LCharge
{
public:
	LCharge();
	~LCharge();

	void	Read(LBuff& buff);
	void	Write(LBuff& buff);
public:
	Lint	m_userId;   //玩家ID
	Lint	m_time;     //房卡变动时间
	Lint	m_cardNum;  //房卡变动数量
	Lint	m_operType; //房卡变动类型
	Lint    m_balance;  //玩家房卡余额
	Lstring	m_operator; //操作者(默认为空)
	Lstring m_remark;   //备注(默认为空)
	Lint	m_nAppId;
};
#endif