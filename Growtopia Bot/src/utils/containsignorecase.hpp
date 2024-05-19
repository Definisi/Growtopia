#include <string>
#include <algorithm>



bool contains_ignore_case(const std::string& str, const std::string& substr) {
	std::string strLower = str;
	std::string substrLower = substr;
	std::transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
	std::transform(substrLower.begin(), substrLower.end(), substrLower.begin(), ::tolower);

	return strLower.find(substrLower) != std::string::npos;
}