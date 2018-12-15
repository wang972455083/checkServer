#ifndef __BaseCommonFunc_h
#define __BaseCommonFunc_h

#include <string>
#include "LBase.h"
#include "LMsg.h"

void			DisableIP(const std::string& ip);

unsigned short CalcChecksum(unsigned char *data, int dataSize);

bool			IsValidCellphoneNo(const std::string& no);

Llong			GetOSTickCount();

LMsg*			ParseInsideNetPacket(LBuffPtr recv, LSocketPtr s);

Lint			TransfromMsgId(Lint timestamp, Lint msgId);
Lint			InverseTransfromMsgId(Lint timestamp, Lint transedMsgId);

Lint			GetDistance(Lint cur_pos, Lint des_pos, Lint desk_user_count);
std::string&	Trim(std::string &str);
std::string&	replace_all(std::string& str,const std::string& old_value,const std::string& new_value);
std::string&	replace_all_distinct(std::string& str,const std::string& old_value,const std::string& new_value);

std::string convertInt2String(int iValue);
std::string convertUInt2String(unsigned int uValue);

std::string convertLL2String(Llong lValue);

#endif		// __BaseCommonFunc_h