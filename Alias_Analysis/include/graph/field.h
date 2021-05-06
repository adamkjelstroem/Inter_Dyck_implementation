#ifndef FIELD_H
#define FIELD_H

#include <functional>
#include <string>

class field{
public:	
	int field_id;
	std::string field_name;

	field(int fid,std::string s){
		this->field_id = fid;
		this->field_name = s;
	}

	field(){
		field_id = -1;
	}

	bool operator == (const field &rhs)const{
		return this->field_id == rhs.field_id;
	}
	
	bool operator < (const field &rhs)const{
		return this->field_id < rhs.field_id;
	}
};

namespace std{
	template <>
	struct hash<field>{
	public:
		std::size_t operator()(const field& k)const {
			return hash<int>()(k.field_id);
		}
	};

	template <>
	struct hash<pair<int,field>> {
	public:
		std::size_t operator()(const std::pair<int, field> &x) const{
			return std::hash<int>()(x.first) ^ std::hash<field>()(x.second);
		}
	};
}


#endif