#pragma once
/********************************************************************
	created:	2016-8-18   11:47
	author:		willing
	
	purpose:	实现字符串的Base64转码
*********************************************************************/

#include <boost/archive/iterators/base64_from_binary.hpp>  
#include <boost/archive/iterators/binary_from_base64.hpp>  
#include <boost/archive/iterators/transform_width.hpp>  
#include <string>  
#include <iostream>  
#include <sstream>  

class Base64
{
public:
	Base64(void);
	~Base64(void);

	// Base64编码 [2016-8-18 willing]
	static bool Base64Encode(const std::string& input, std::string& output);

	// Base64解码 [2016-8-18 willing]
	static bool Base64Decode(const std::string& input, std::string& output);
};

