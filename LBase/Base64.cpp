#include "Base64.h"


Base64::Base64(void)
{
}


Base64::~Base64(void)
{
}

bool Base64::Base64Encode(const std::string& input, std::string& output)
{
	output = "";
	typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> > Base64EncodeIterator;  
	std::stringstream result;  
	copy(Base64EncodeIterator(input.begin()) , Base64EncodeIterator(input.end()), std::ostream_iterator<char>(result));  
	size_t equal_count = (3 - input.length() % 3) % 3;  
	for (size_t i = 0; i < equal_count; i++) {  
		result.put('=');  
	}  
	output = std::move(result.str());
	return !output.empty(); 
}

bool Base64::Base64Decode(const std::string& input, std::string& output)
{
	typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIterator;  
	std::stringstream result;  
	try {  
		std::copy(Base64DecodeIterator(input.begin()) , Base64DecodeIterator(input.end()), std::ostream_iterator<char>(result));  
	} catch(...) {  
		return false;  
	}  
	output = std::move(result.str());
	return !output.empty();  
}