#pragma once
#include "memory"
#include "Point.h"

#define PropertyBuilderByName(type, name, access_permission)\
    access_permission:\
        type m_##name;\
    public:\
    inline void set##name(type v) {\
        m_##name = v;\
    }\
    inline type get##name() {\
        return m_##name;\
    }\



//红方在下边，黑方在上边。期盼是左下角为（0,0）

enum CHESS_COLOR
{
	CHESS_RED,
	CHESS_BLACK,
	CHESS_ERROR_COLOR,
};

enum PosLimit
{
	POS_LIMIT_X = 8,
	POS_LIMIT_Y = 9
};

enum TYPE_CHESS
{
	CHESS_NULL,
	CHESS_JIANG,
	CHESS_SHI,
	CHESS_XIANG,
	CHESS_MA,
	CHESS_CHE,
	CHESS_PAO,
	CHESS_BING,
	CHESS_MAX
};

//动态前进就是在最大值和最小值的范围内随便移动
enum TYPE_OPERATOR_MOVE
{
	MOVE_NULL ,							//没有动作
	MOVE_ADVANCE_FIXED ,				//前进固定个数
	MOVE_ADVANCE_DYNAMIC,				//动态前进
	MOVE_BACK_FIXED,					//后退
	MOVE_BACK_DYNAMIC,					//动态后退
	MOVE_HORIZONTAL_FIXED,				//水平固定移动
	MOVE_HORIZONTAL_DYNAMIC,			//水平动态移动
	MOVE_LEAN_FIXED,					//倾斜固定移动
	MOVE_LEAN_DYNAMIC,					//倾斜固定移动
	MOVE_RI_ZI_GE						//日字移动

};

//方向
enum MOVE_DIREC
{
	MT_HORIZONTAL = 1,	//水平方向
	MT_VERTICAL,		//垂直方向
	MT_LEAN,			//倾斜方向
	
};

