#ifndef FDLL_H
#define FDLL_H

#include <list>
#include <unordered_map>
#include <iostream>

using namespace std;

template <class T> 
// be sure that you have hash function defined in namespace std for the type T
class fdll{
	/// multiple entries are not possible
	// see for any reference pains
	unordered_map<T,typename list<T>::iterator> positionof;
	list<T> linked_list;
public:
	void add(const T& v);

	bool contains(const T& v);

	int size();

	T front();

	void pop();

	typename list<T>::iterator begin();

	typename list<T>::iterator end();

	typename list<T>::iterator erase(T& v);

};

#endif