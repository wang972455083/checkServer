#ifndef _L_TOOL_H_
#define _L_TOOL_H_

#include "LBase.h"
#include "LSingleton.h"
/*
*	brief 字符串转换成llong类型
*/
Llong L_Atoll(const char* s);

/*
*   更新随机数种子
*/
void updateRandomFactor();

/*
*	brief 在闭区间[begin,end]之间随机一个整数
*/
int L_Rand(int begin,int end);

/*
*	解析1,2,3这种类型字符串
*/
void L_ParseString(Lstring src, std::vector<Lstring>& des,Lstring par=",");

/*
*	解析1,2;3,4;这种类型字符串
*/
void L_ParseString(Lstring src, std::map<Lstring,Lstring>& des,Lstring pa1=";",Lstring pa2=",");

std::string UTF8_To_string(const std::string & str) ;

std::string string_To_UTF8(const std::string & str) ;

//活动配置相关
time_t convert_string_to_time_t(const std::string & time_string);
// 判断活动时间是否有效
bool   checkStrTimeValid(const Lstring&  strBeginTime, const Lstring&  strEndTime);
bool   checkTimeValid(const time_t& BeginTime, const time_t& EndTime);
// 判断时间配置是否合法
bool   checkActiveTimeSetVaild(const Lstring&  strBeginTime, const Lstring&  strEndTime);
bool   checkActiveTimeSetVaild(const time_t& BeginTime, const time_t& EndTime);

template<class T>
inline void ReadMapValue(msgpack::object& obj, Lstring key, T& t)
{
	t = T();
}

template<>
inline void ReadMapValue(msgpack::object& obj, Lstring key, Lstring& value)
{
	value = "";
	msgpack::object_kv*  pkv;
	msgpack::object_kv*  pkv_end;
	msgpack::object      pk, pv;
	if ((msgpack_object_type)obj.type == MSGPACK_OBJECT_MAP)
	{
		pkv = obj.via.map.ptr;
		pkv_end = obj.via.map.ptr + obj.via.map.size;
		do
		{
			pk = pkv->key;
			pv = pkv->val;
			if (pk.as<Lstring>() == key )
			{
				if((msgpack_object_type)pv.type == MSGPACK_OBJECT_STR)
					pv.convert(value);
				break;
			}
			++pkv;
		} while (pkv < pkv_end);
	}
}

template<>
inline void ReadMapValue(msgpack::object& obj, Lstring key, Lint& value)
{
	value = 0;
	msgpack::object_kv*  pkv;
	msgpack::object_kv*  pkv_end;
	msgpack::object      pk, pv;
	if ((msgpack_object_type)obj.type ==  MSGPACK_OBJECT_MAP)
	{
		pkv = obj.via.map.ptr;
		pkv_end = obj.via.map.ptr + obj.via.map.size;
		do
		{
			pk = pkv->key;
			pv = pkv->val;
			if (pk.as<Lstring>() == key )
			{
				if((msgpack_object_type)pv.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
					pv.convert(value);
				break;
			}
			++pkv;
		} while (pkv < pkv_end);
	}
}

template<>
inline void ReadMapValue(msgpack::object& obj, Lstring key, Lshort& value)
{
	value = 0;
	msgpack::object_kv*  pkv;
	msgpack::object_kv*  pkv_end;
	msgpack::object      pk, pv;
	if ((msgpack_object_type)obj.type ==  MSGPACK_OBJECT_MAP)
	{
		pkv = obj.via.map.ptr;
		pkv_end = obj.via.map.ptr + obj.via.map.size;
		do
		{
			pk = pkv->key;
			pv = pkv->val;
			if (pk.as<Lstring>() == key )
			{
				if((msgpack_object_type)pv.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
					pv.convert(value);
				break;
			}
			++pkv;
		} while (pkv < pkv_end);
	}
}

template<>
inline void ReadMapValue(msgpack::object& obj, Lstring key, Lchar& value)
{
	value = 0;
	msgpack::object_kv*  pkv;
	msgpack::object_kv*  pkv_end;
	msgpack::object      pk, pv;
	if ((msgpack_object_type)obj.type ==  MSGPACK_OBJECT_MAP)
	{
		pkv = obj.via.map.ptr;
		pkv_end = obj.via.map.ptr + obj.via.map.size;
		do
		{
			pk = pkv->key;
			pv = pkv->val;
			if (pk.as<Lstring>() == key )
			{
				if((msgpack_object_type)pv.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
					pv.convert(value);
				break;
			}
			++pkv;
		} while (pkv < pkv_end);
	}
}

//从map里面读取一个对象，目前只支持读取map array
inline void ReadMapValue(msgpack::object& obj, Lstring key, msgpack::object& value)
 {
	value = msgpack::object();
 	msgpack::object_kv*  pkv;
 	msgpack::object_kv*  pkv_end;
 	msgpack::object      pk, pv;
 	if ((msgpack_object_type)obj.type ==  MSGPACK_OBJECT_MAP)
 	{
 		pkv = obj.via.map.ptr;
 		pkv_end = obj.via.map.ptr + obj.via.map.size;
 		do
 		{
 			pk = pkv->key;
 			pv = pkv->val;
 			if (pk.as<Lstring>() == key )
 			{
 				if((msgpack_object_type)pv.type == MSGPACK_OBJECT_ARRAY || (msgpack_object_type)pv.type == MSGPACK_OBJECT_MAP)
					value = pv;
 				break;
 			}
 			++pkv;
 		} while (pkv < pkv_end);
 	}
}

 template<class T>
 inline void ReadArrayValue(msgpack::object& array, Lsize index, Lstring key, T& value)
 {
	 value = T();
 }
 
 template<>
 inline void ReadArrayValue(msgpack::object& array, Lsize index, Lstring key, Lint& value)
 {
 	value = 0;
 	msgpack::object*      pav;
 	msgpack::object_kv*	 pkv;
 	msgpack::object_kv*  pkv_end;
 	msgpack::object      pk, pv;
 
	if ((msgpack_object_type)array.type == MSGPACK_OBJECT_ARRAY)
 	{
		if (index < array.via.array.size)
 		{
			pav = array.via.array.ptr + index;
 			if ((msgpack_object_type)pav->type ==  MSGPACK_OBJECT_MAP)
 			{
 				pkv = pav->via.map.ptr;
				pkv_end = pav->via.map.ptr + pav->via.map.size;
 				do
 				{
 					pk = pkv->key;
 					pv = pkv->val;
 					if (pk.as<Lstring>() == key )
 					{
 						if((msgpack_object_type)pv.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
 							pv.convert(value);
 						break;
 					}
 					++pkv;
 				} while (pkv < pkv_end);
 			}
 		}
 	}
 }
 
 template<>
 inline void ReadArrayValue(msgpack::object& array, Lsize index, Lstring key, Lshort& value)
 {
 	value = 0;
 	msgpack::object*     pav;
 	msgpack::object_kv*	 pkv;
 	msgpack::object_kv*  pkv_end;
 	msgpack::object      pk, pv;
 
	if ((msgpack_object_type)array.type == MSGPACK_OBJECT_ARRAY)
 	{
		if (index < array.via.array.size)
 		{
			pav = array.via.array.ptr + index;
 			if ((msgpack_object_type)pav->type ==  MSGPACK_OBJECT_MAP)
 			{
 				pkv = pav->via.map.ptr;
				pkv_end = pav->via.map.ptr + array.via.map.size;
 				do
 				{
 					pk = pkv->key;
 					pv = pkv->val;
 					if (pk.as<Lstring>() == key )
 					{
 						if((msgpack_object_type)pv.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
 							pv.convert(value);
 						break;
 					}
 					++pkv;
 				} while (pkv < pkv_end);
 			}
 		}
 	}
 }
 
 
 template<>
 inline void ReadArrayValue(msgpack::object& array, Lsize index,Lstring key, Lstring& value)
 {
 	value = "";
 	msgpack::object*     pav;
 	msgpack::object_kv*	 pkv;
 	msgpack::object_kv*  pkv_end;
 	msgpack::object      pk, pv;
 
	if ((msgpack_object_type)array.type == MSGPACK_OBJECT_ARRAY)
 	{
		if (index < array.via.array.size)
 		{
			pav = array.via.array.ptr + index;
 			if ((msgpack_object_type)pav->type ==  MSGPACK_OBJECT_MAP)
 			{
 				pkv = pav->via.map.ptr;
				pkv_end = pav->via.map.ptr + array.via.map.size;
 				do
 				{
 					pk = pkv->key;
 					pv = pkv->val;
 					if (pk.as<Lstring>() == key )
 					{
						if ((msgpack_object_type)pv.type == MSGPACK_OBJECT_STR)
 							pv.convert(value);
 						break;
 					}
 					++pkv;
 				} while (pkv < pkv_end);
 			}
 		}
 	}
 }
 
 
 template<>
 inline void ReadArrayValue(msgpack::object& array, Lsize index, Lstring key, Lchar& value)
 {
 	value = 0;
 	msgpack::object*     pav;
 	msgpack::object_kv*	 pkv;
 	msgpack::object_kv*  pkv_end;
 	msgpack::object      pk, pv;
 
	if ((msgpack_object_type)array.type == MSGPACK_OBJECT_ARRAY)
 	{
		if (index < array.via.array.size)
 		{
			pav = array.via.array.ptr + index;
 			if ((msgpack_object_type)pav->type ==  MSGPACK_OBJECT_MAP)
 			{
 				pkv = pav->via.map.ptr;
				pkv_end = pav->via.map.ptr + array.via.map.size;
 				do
 				{
 					pk = pkv->key;
 					pv = pkv->val;
 					if (pk.as<Lstring>() == key )
 					{
 						if((msgpack_object_type)pv.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
 							pv.convert(value);
 						break;
 					}
 					++pkv;
 				} while (pkv < pkv_end);
 			}
 		}
 	}
 }


#define ReadMapData(obj,key,data) ReadMapValue(obj,Lstring(key),data);
 
#define ReadArrayData(obj,i,key,data) ReadArrayValue(obj,i,key,data);

#define WriteKeyValue(pack,key,value) pack.pack(Lstring(key));pack.pack(value);

#define WriteKey(pack,data) pack.pack(data);

#define WriteValue(pack,data) pack.pack(data);

#define WriteMap(pack,size) pack.pack_map(size);

#define WriteArray(pack,size) pack.pack_array(size);

#endif
