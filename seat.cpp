#include <iostream>
#include <utility>
#include <random>
#include "seat.hpp"

using namespace std;

int main(){
	int m = 8;
	int k = 8;
	int p = k-1;

	vector<vector<pair<int,int>>> seatRes(m, vector<pair<int,int>>());
	vector<int> q;
	vector<vector<int>> cuts;
	vector<pair<int,int>> input;

	q = makeQs(q,p,m);

	cuts = makeCutsVector(cuts,q,k,m);

	makeCuts(seatRes,cuts,k);

	fixCuts(seatRes,m,k);

	//Create input vector
	for(int i = 0; i < seatRes.size(); i++){
		for(int j = 0; j < seatRes[i].size(); j++){
			input.push_back(seatRes[i][j]);
		}
	}

	random_shuffle(input.begin(),input.end());

	printInput(input);

	intervalLengthBestFit(input,m,k);

	intervalFreqBestFit(input,m,k);

	stationFreqBestFit(input,m,k);

	startstationBestFit(input,m,k);
	return 0;
}
