#ifndef _L_BASE_H_
#define _L_BASE_H_

#include <cctype>
#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <sstream>
#include <algorithm>

#define  _WIN32_WINNT  0x0600


#include <boost/thread.hpp>
#include <boost/asio.hpp>
//#include <boost/make_shared.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/lexical_cast.hpp>

#include <mysql.h>
//#include <mysql/mysql.h>

//#include <msgpack/msgpack.hpp>
#include <msgpack.hpp>


#if _LINUX
#define sprintf_s snprintf
#define gmtime_s gmtime_r
#include <json/json.h>
#else
#include <jsoncpp/json.h>
#endif


//#include <Windows.h>


//基本的整数类型如下
//char(8) short(16) int(32) float(32) double(64)
//
//
#if _LINUX
	typedef unsigned long       DWORD;
	typedef void *PVOID;
#endif

typedef char Lchar;
typedef unsigned char Luchar;

typedef short Lshort;
typedef unsigned short Lushort;

typedef int	Lint;
typedef unsigned int Luint;

typedef long long Llong;
typedef unsigned long long Lulong;

typedef float Lfloat;

typedef double Ldouble;

typedef std::size_t Lsize;

typedef std::string Lstring;


enum USER_STATUS
{
	US_CENTER = 0, //大厅
	US_ROOM = 1,  //在桌子里
};



enum ROOM_STATUS
{
	RS_WAIT,  //
	RS_GAME,
};



enum
{
	ROOM_USER_CNT = 2,  //房间人数
	DESK_USER_CNT = 2,	 //桌子人数
	LIST_USER_MAX_CNT = 10000000,  //玩家队列最大数
};

enum
{
	RT_SUCCEED = 0,   //成功
	RT_ERROR = 1,	  //错误
};


enum CARD_TYPE
{
	CARD_NULL,
	CARD_BU,		//布
	CARD_JIAN_DAO,	//剪刀
	CARD_SHI_TOU,	//石头
};

//玩家筹码都是1颗星
enum DeskType
{
	DT_FIRST_TYPE = 1,  //普通玩法
	DT_SECOND_TYPE = 2,  //先手玩家亮牌奖励玩法（A和B两玩家对决，开局时系统为A提示一张牌，系统奖励为1颗星，系统金币奖励只会A在选择布并且赢的时候获得，其他的可能就按照正常判断奖励，即B永远也不会获得系统奖励）
	//筹码至少是2颗星
	DT_THIRD_TYPE = 3,		//开局前（A和B两个玩家对决，筹码n颗星。开始系统选出一张牌，假设为布并且有n颗星的奖励。根据玩家选出不同的牌有以下结果 1如果两个人不是平局，如果两个人中存在一个出现布，则赢家赢取输家和系统的星星一共2n颗星星，如果不存在则赢家只能赢取输家的n颗星星。 2如果两个人都出布则两个人平分系统的n金币,每人获得n/2金币 3，如果两个人平局并且都没有选择布，则两个人各输系统n/2金币）
	//筹码至少是3颗星
	DT_FOURTH_TYPE = 4,  //系统提示两张牌奖励（A和B两个玩家对决，筹码n金币。开始时系统为A和B两个人各提示一张牌，不能一样。，假设A提示的是剪刀，B提示的是布，因为剪刀胜于布，所以系统对A的奖励为n/2+1，对B的奖励为n/2。根据不同的玩家有以下的结果。 1 ，赢局:如果A真的出剪刀而B也真的出布，则A和B各得系统奖励，不结算各自输赢。如果其中一个出现与系统提示牌相同的，则获取输家的金币和与系统提示相同的玩家的系统奖励金币，如果都没有出现与系统提示相同，则赢家只赢取输家的金币  2，如果平局，如果玩家与系统提示的牌相同，则只赢取系统提示的牌。 ）
};

enum
{
	MAX_POS = 2,
};

enum
{
	POS_1 = 1,
	POS_2 = 2
};

enum
{
	MAX_SELECT_TIME = 15,

};


enum ROOMUSER_MODIFY
{
	MODIFY_DEL_USER = 1,
	MODIFY_UPDATE_USER = 2,
};


#if _LINUX
typedef bool BOOL;
#define TRUE                1
#endif


typedef char *NPSTR, *LPSTR, *PSTR;

//var 转换成字符串 var"
#define NAME_TO_STR(var)  (#var)

#endif
