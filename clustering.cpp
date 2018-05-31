#include <iostream>
#include <utility>
#include <random>
#include "clustering.hpp"

using namespace std;

int main(){
	float k = 6.0;
	int alg = 0;
	int opt = 0;

	vector<float> input;
	int n = 4*k;

	float length = k/3;

	createInput(input,n,k);

	printInput(input);

	onlineBenefitGreedy(input,length);

	onlineCount(input,length);

	discretizedUnitGreedy(input,k,length);

	benefitGreedy(input,k,length);

	minDistGreedy(input,length);

	floatingUnitGreedy(input,length);
	return 0;

}
