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

//桌子类型
enum DESK_TYPE
{
	DESK_COIN = 1,		//金币桌
	DESK_DIAMOND = 2,	//钻石桌
};

enum USER_STATUS
{
	US_CENTER = 0, //大厅
	US_DESK = 1,  //在桌子里
};
//桌子满员人数
const int DESK_FULL_USER_CNT = 2;

enum PLAYER_STATUS
{
	STATUS_NULL,
	STATUS_READY,
};

enum DESK_STATUS
{
	DS_WAIT = 0,			//等待状态
	DS_CHESS = 1,			//棋局状态
};

enum BLOCK_PROTERY
{
	BP_HP = 1,
	BP_ATTACK,
	BP_COIN,
};


#if _LINUX
typedef bool BOOL;
#define TRUE                1
#endif


typedef char *NPSTR, *LPSTR, *PSTR;

//var 转换成字符串 var"
#define NAME_TO_STR(var)  (#var)

#endif
