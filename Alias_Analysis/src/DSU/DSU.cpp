#include "DSU/DSU.h"
// #include <cassert>

void DSU::init(int n){
	this->N = n;
	parent_pos.resize(this->N);
	vtxatpos.resize(this->N);
	posofvtx.resize(this->N);
	for(int i=0;i<N;i++){
		parent_pos[i]=-1;
		vtxatpos[i]=i;
		posofvtx[i]=i;
	}
}

int DSU::getN(){
	return this->N;
}

//returns you the id of the root
int DSU::root(int vtx){
	return vtxatpos[rootpos(posofvtx[vtx])];
	// return (parent_pos[v]<0?v:parent_pos[v]=DSU::root(parent_pos[v]));
}

// returns you the position of the root
int DSU::rootpos(int pos){
	return parent_pos[pos]<0?pos:parent_pos[pos]=rootpos(parent_pos[pos]);
}

// gives the rank of the vtx r
int DSU::rank(int r){
	// assert(r==root(r));
	return -1*parent_pos[posofvtx[r]];
}


// merge by height
int DSU::merge(int a,int b){
	// assert(a==root(a) and b==root(b));
	//precondition a and b are root
	// if( (a=root(a)) == (b=root(b)) )
	if(a==b)
		return a;
	int posa = posofvtx[a];
	int posb = posofvtx[b];
	N--;
	if(parent_pos[posa]>parent_pos[posb]){
		parent_pos[posa] = posb;
		return b;
	}
	if(parent_pos[posa]==parent_pos[posb])
		parent_pos[posa]--;
	parent_pos[posb] = posa;
	return a;
}

void DSU::hardmerge(int r,int a){
	// assert(r==root(r) and a==root(a));
	if(a==r)
		return;
	int posr = posofvtx[r];
	int posa = posofvtx[a];
	N--;
	if(parent_pos[posa]==parent_pos[posr])
		parent_pos[posr]--;
	parent_pos[posa] = posr; 
}

void DSU::exchange(int uid,int vid){
	int posu = posofvtx[uid];
	int posv = posofvtx[vid];
	vtxatpos[posu] = vid;
	vtxatpos[posv] = uid;
	posofvtx[uid] = posv;
	posofvtx[vid] = posu;
}