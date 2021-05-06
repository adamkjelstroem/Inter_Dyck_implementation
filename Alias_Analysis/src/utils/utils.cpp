#include "utils/utils.h"

using namespace std;


// trim from start
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

void split(string s,string delim,std::vector<std::string> &tokens){
	size_t curr_pos=0,last_pos=0,size = s.size();
	while(true){
		curr_pos = s.find(delim,last_pos);
		if(curr_pos==string::npos){
			std::string substring = s.substr(last_pos);
			tokens.push_back(trim(substring));
			break;
		}
		std::string substring = s.substr(last_pos,curr_pos-last_pos);
		tokens.push_back(trim(substring));
		last_pos = curr_pos+2;
	}
}