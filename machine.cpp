#include <iostream>
#include <utility>
#include <random>
#include "machine.hpp"

int main(){
	double m = 8.0;

	//Resource Constraint
	vector<vector<pair<double,double>>> machinesRes(m, vector<pair<double,double>>());
	vector<pair<double,double>> inputRes;
	makeCuts(m,machinesRes);

	//Create input vector
	for(int i = 0; i < machinesRes.size(); i++){
		for(int j = 0; j < machinesRes[i].size(); j++){
			inputRes.push_back(machinesRes[i][j]);
		}
	}
	random_shuffle(inputRes.begin(),inputRes.end());

	listScheduling(inputRes,m);
	
	firstFitLS(inputRes,m);
	
	bestFitLS(inputRes,m);
	
	nonemptyWorstFitLS(inputRes,m);
	
	optimalRes(inputRes,m);


	//Cache Constraint
	vector<vector<pair<double,double>>> machinesCache(m, vector<pair<double,double>>());
	vector<pair<double,double>> inputCache;
	makeCutsCache(m,machinesCache);

	//Create input vector
	for(int i = 0; i < machinesCache.size(); i++){
		for(int j = 0; j < machinesCache[i].size(); j++){
			inputCache.push_back(machinesCache[i][j]);
		}
	}
	random_shuffle(inputCache.begin(),inputCache.end());

	listSchedulingCache(inputCache,m);

	optimalCache(inputCache,m);

	return 0;
}
