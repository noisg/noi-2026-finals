#include "lemon.h"
#include <bits/stdc++.h>
using namespace std;
vector<pair<int,int> > all;
int ind[505],n;
int lem=-1,st=-1;
std::string init(int subtask, int N, std::vector<int> p){
	lem=-1; st=-1;
	n=N;
	assert(n==500);
	if(all.empty()){
		for(int i=0; i<33; i++){
			for(int j=i+1; j<33; j++){
				all.push_back({i,j});
			}
		}
	}
	//cout << p.size() << endl;
	for(int i=1; i<=n; i++){
		//cout << p[i] << endl;
		ind[p[i]]=i;
	}
	string ret="";
	for(int i=n-32; i<=n; i++){
		for(int j=0; j<9; j++){
			if(p[i]&(1<<j)) ret+='1';
			else ret+='0';
		}
	}
	return ret;
}
bool receive_fruit(int id, bool is_lemon){
	if(is_lemon){
		lem=id;
		if(ind[id]>=n-32) st=2;
		else st=1;
		return false;
	}
	if(ind[id]>=n-32){
		if(st==2){
			st=0;
			return true;
		}
		else if(st==1){
			if(ind[id]-(n-32)==all[lem].first||ind[id]-(n-32)==all[lem].second){
				return true;
			}
			else return false;
		}
	}
	return false;
}

int answer(int subtask, int N, std::string b, std::vector<int> uneaten){
	n=N;
	vector<int> bac;
	for(int i=0; i<33; i++){
		int num=0;
		for(int j=0; j<9; j++){
			if(b[i*9+j]=='1') num^=(1<<j);
		}
		bac.push_back(num);
	}
	int eat[505];
	memset(eat,0,sizeof(eat));
	for(int i:uneaten) eat[i]=1;
	pair<int,int> gg={-1,-1};
	for(int i=1; i<=500; i++){
		if(eat[i]==0){
			if(gg.first==-1) gg.first=i;
			else gg.second=i;
		}
	}
	if(gg.first==-1) return bac.back();
	if(gg.second==-1){
		for(int i=0; i<33; i++){
			if(bac[i]==gg.first) return bac[i-1];
		}
	}
	else{
		pair<int,int> hm={-1,-1};
		for(int i=0; i<33; i++){
			if(bac[i]==gg.first||bac[i]==gg.second){
				if(hm.first==-1) hm.first=i;
				else hm.second=i;
			}
		}
		return lower_bound(all.begin(),all.end(),hm)-all.begin();
	}
    return 0;
}
