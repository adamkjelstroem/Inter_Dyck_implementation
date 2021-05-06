#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <functional> 
#include <cctype>
#include <locale>
#include <vector>
#include <algorithm>

static inline std::string &ltrim(std::string &s);
static inline std::string &rtrim(std::string &s);
static inline std::string &trim(std::string &s);
void split(std::string s,std::string delim,std::vector<std::string> &tokens);

#endif