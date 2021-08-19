#include "utils/fdll.h"
#include "graph/field.h"
#include <cassert>

// fast double linked list data structure as described in 
// Fast Algorithms for Dyck-CFL-Reachability with Applications to Alias Analysis paper

template<class T> void fdll<T>::add(const T& v){
	if(contains(v))
		return;
	linked_list.push_back(v);
	positionof[v] = std::next(linked_list.end(), -1);
}

template<class T> bool fdll<T>::contains(const T& v){
	return positionof.find(v)!=positionof.end();
}

template<class T> typename list<T>::iterator fdll<T>::erase(T& v){
	if(!this->contains(v)){
		cerr<<"erasing a value that doesnt exist in fdll"<<endl;
		assert(false);
		return linked_list.begin();	
	}
	auto ret = linked_list.erase(positionof[v]); 
	positionof.erase(v);
	return ret;
}

template<class T> int fdll<T>::size(){
	return linked_list.size(); 
}

template<class T> T fdll<T>::front(){
	return *(linked_list.begin());	
}

template<class T> void fdll<T>::pop(){
	positionof.erase(front());
	linked_list.pop_front();
}

template<class T> typename list<T>::iterator fdll<T>::begin(){
	return linked_list.begin();
}

template<class T> typename list<T>::iterator fdll<T>::end(){
	return linked_list.end();
}


template class fdll<pair<int,field>>;
template class fdll<int>;