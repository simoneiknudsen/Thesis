#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <vector>
#include <ostream>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>
#include <climits>
#include <limits>

using namespace std;


void printSchedule(vector<vector<pair<double,double>>> M){
	for(int i = 0; i < M.size(); i++){
		cout << i << ": ";
		for(int j = 0; j < M[i].size(); j++){
			cout << "(" << M[i][j].first << ", " << M[i][j].second << ")";
		}
		cout << endl;
	}
}

// Stuff for Resource Constraint 

pair<double,double> sum(vector<pair<double,double>> v){
	double p = 0;
	double r = 0;
	if(v.size() == 0){
		p = 0;
		r = 0;
	} else {
		for(int i = 0; i < v.size(); i++){
			p += v[i].first;
			r += v[i].second;
		}
	}
	return make_pair(p,r);
}

void makeCuts(double m, vector<vector<pair<double,double>>> &M){
	int q = (m/4)-1;
	vector<pair<double,double>> V;
	vector<double> v;
	vector<double> v2;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0,m);
	std::uniform_real_distribution<> dis2(0.0,1.0);
	for(int j = 0; j < m; j++){
		for(int i = 0; i < q; i++){
			v.push_back(dis(gen));
			v2.push_back(dis2(gen));
		}
		sort(v.begin(),v.end());
		sort(v2.begin(),v2.end());
		V.push_back(make_pair(v[0],v2[0]));
		for(int i = 1; i < v.size(); i++){
			V.push_back(make_pair(v[i]-v[i-1],v2[i]-v2[i-1]));
		}
		V.push_back(make_pair(m-v[v.size()-1],1.0-v2[v2.size()-1]));
		M[j] = V;
		v.clear();
		v2.clear();
		V.clear();
	}
}

pair<double,double> listScheduling(vector<pair<double,double>> input, double m){
	vector<vector<pair<double,double>>> M(m, vector<pair<double,double>>());
	vector<double> p(m);
	vector<double> r(m);
	bool placed = false;
	int index = 0;
	double alg = 0.0;
	int alg1 = 0;
	

	for(int k = 0; k < m; k++){
		M[k].push_back(input[k]);
		p[k] = input[k].first;
		r[k] = input[k].second;
	}
	double minimum = numeric_limits<double>::max();
	for(int k = m; k < input.size(); k++){
		for(int i = 0; i < M.size(); i++){
			if(minimum > p[i] && r[i]+input[k].second <= 1){
				index = i;
				minimum = p[i];
				placed = true;
			}
		}
		if(placed){
			M[index].push_back(input[k]);
			p[index] += input[k].first;
			r[index] += input[k].second;
		}
		index = 0;
		minimum = numeric_limits<double>::max();
		placed = false;
	}

	for(int i = 0; i < M.size(); i++){
		alg = max(sum(M[i]).first,alg);
		alg1 += M[i].size();
	}
	cout << "Printing solution: " << endl;
	printSchedule(M);
	return make_pair(alg,alg1);
}

pair<double,int> firstFitLS(vector<pair<double,double>> input, double m){
	vector<vector<pair<double,double>>> M(m, vector<pair<double,double>>());
	vector<double> p;
	vector<double> r;
	bool placed = false;
	double alg = 0.0;
	int alg1 = 0;

	for(int i = 0; i < m; i++){
		p.push_back(0.0);
		r.push_back(0.0);
	}
	
	for(int i = 0; i < input.size(); i++){
		for(int j = 0; j < M.size(); j++){
			if(!placed && p[j]+input[i].first <= 1.2*m && r[j]+input[i].second <= 1){
				M[j].push_back(input[i]);
				p[j] += input[i].first;
				r[j] += input[i].second;
				placed = true;
			}
		}
		if(!placed){
			double min = numeric_limits<double>::max();
			int index = 0;
			for(int j = 0; j < m; j++){
				if(p[j] < min && r[j]+input[i].second <= 1){
					min = p[j];
					index = j;
				}
			}
			if(min != numeric_limits<double>::max()){
				M[index].push_back(input[i]);
				p[index] += input[i].first;
				r[index] += input[i].second;
			}
		}
		placed = false;
	}

	for(int i = 0; i < M.size(); i++){
		alg = max(sum(M[i]).first,alg);
		alg1 += M[i].size();
	}
	cout << "Printing solution: " << endl;
	printSchedule(M);
	return make_pair(alg,alg1);
}

pair<double,int> bestFitLS(vector<pair<double,double>> input, double m){
	vector<vector<pair<double,double>>> M(m, vector<pair<double,double>>());
	vector<double> p;
	vector<double> r;
	bool placed = false;
	double alg = 0.0;
	int alg1 = 0;

	for(int i = 0; i < m; i++){
		p.push_back(0.0);
		r.push_back(0.0);
	}
	
	for(int i = 0; i < input.size(); i++){
		int maxIndex = INT_MIN;
		double max = -1.0;
		for(int j = 0; j < M.size(); j++){
			if(p[j] > max && r[j]+input[i].second <= 1 && p[j]+input[i].first <= 1.2*m){
				maxIndex = j;
				max = p[j];
				placed = true;
			}
		}
		if(!placed){
			double min = numeric_limits<double>::max();
			int index = 0;
			for(int j = 0; j < m; j++){
				if(p[j] < min && r[j]+input[i].second <= 1){
					min = p[j];
					index = j;
				}
			}
			if(min != numeric_limits<double>::max()){
				M[index].push_back(input[i]);
				p[index] += input[i].first;
				r[index] += input[i].second;
			}
		} else {
			M[maxIndex].push_back(input[i]);
			p[maxIndex] += input[i].first;
			r[maxIndex] += input[i].second;
		}
		placed = false;
	}

	for(int i = 0; i < M.size(); i++){
		alg = max(sum(M[i]).first,alg);
		alg1 += M[i].size();
	}
	cout << "Printing solution: " << endl;
	printSchedule(M);
	return make_pair(alg,alg1);
}

pair<double,int> nonemptyWorstFitLS(vector<pair<double,double>> input, double m){
	vector<vector<pair<double,double>>> M(m, vector<pair<double,double>>());
	vector<double> p;
	vector<double> r;
	bool placed = false;
	double alg = 0.0;
	int alg1 = 0;

	for(int i = 0; i < m; i++){
		p.push_back(0.0);
		r.push_back(0.0);
	}
	
	for(int i = 0; i < input.size(); i++){
		int minIndex = INT_MAX;
		double min = numeric_limits<double>::max();
		for(int j = 0; j < M.size(); j++){
			if(p[j] != 0.0 && p[j] < min && r[j]+input[i].second <= 1 && p[j]+input[i].first <= 1*m){
				minIndex = j;
				min = p[j];
				placed = true;
			}
		}
		if(!placed){
			double min = numeric_limits<double>::max();
			int index = 0;
			for(int j = 0; j < m; j++){
				if(p[j] < min && r[j]+input[i].second <= 1){
					min = p[j];
					index = j;
				}
			}
			if(min != numeric_limits<double>::max()){
				M[index].push_back(input[i]);
				p[index] += input[i].first;
				r[index] += input[i].second;
			}
		} else {
			M[minIndex].push_back(input[i]);
			p[minIndex] += input[i].first;
			r[minIndex] += input[i].second;
		}
		placed = false;
	}

	for(int i = 0; i < M.size(); i++){
		alg = max(sum(M[i]).first,alg);
		alg1 += M[i].size();
	}
	cout << "Printing solution: " << endl;
	printSchedule(M);
	return make_pair(alg,alg1);
}

pair<double,double> optimalRes(vector<pair<double,double>> input, double m){
	sort(input.begin(), input.end(),[](const std::pair<double,double>& a, const std::pair<double,double>& b) {
  		return get<0>(a) < get<0>(b);
  	});

  	auto alg = listScheduling(input,m);

  	return alg;
}


//Stuff for Cache Resource

void makeCutsCache(double m, vector<vector<pair<double,double>>> &M){
	int q = (m/4)-1;
	double C = 100.0;
	vector<pair<double,double>> V;
	vector<double> v;
	vector<double> v2; //to find minimum
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0,m);
	std::uniform_real_distribution<> dis2(0.0,C);
	for(int j = 0; j < m; j++){
		for(int i = 0; i < q; i++){
			v.push_back(dis(gen));
			//v2.push_back(dis2(gen));
		}
		sort(v.begin(),v.end());
		//sort(v2.begin(),v2.end());
		V.push_back(make_pair(v[0],0.0));
		v2.push_back(v[0]);
		for(int i = 1; i < v.size(); i++){
			V.push_back(make_pair(v[i]-v[i-1],0.0));
			//v2.push_back(v[i]-v[i-1]);
		}
		V.push_back(make_pair(m-v[v.size()-1],0.0));
		//v2.push_back(m-v[v.size()-1]);
		M[j] = V;
		v.clear();
		//v2.clear();
		V.clear();
	}
	for(int j = 0; j < m-1; j++){
		v.push_back(dis2(gen));
	}
	sort(v.begin(),v.end());
	/*cout << "The vector:" << endl;
	for(int i = 0; i < v.size(); i++){
		cout << v[i] << ", ";
	}
	cout << endl;*/
	
	M[0][0].second = v[0];
	for(int j = 1; j < v.size(); j++){
		M[j][0].second = v[j]-v[j-1];
	}
	double sum = 0.0;
	for(int i = 0; i < M.size(); i++){
		sum += M[i][0].second;
	}
	M[v.size()][0].second = C-sum;
	bool first = true;
	double remainder = 0;
	for(int i = 0; i < (m-1)*(q+1); i++){
		auto min = distance(v2.begin(),min_element(v2.begin(), v2.end()));
		//cout << "Min: " << min << endl;
		for(int j = 0; j < M[min].size(); j++){
			if(M[min][j].second == 0.0 && first){
				std::uniform_real_distribution<> dis3(0.0,remainder+M[min][j-1].second);
				//cout << "remainder: " << remainder+M[min][j-1].second << endl;
				M[min][j].second = dis3(gen);
				remainder = (remainder+M[min][j-1].second)-M[min][j].second;
				v2[min] += M[min][j].first;
				first = false;
			}
		}
		first = true;
	}
}

void updateCache(vector<pair<double,double>> &P, pair<double,double> input, double current, double C, bool newElem){
	// Finding the element with the makespan of the machine
	auto it = find_if(P.begin(),P.end(),[&current](const std::pair<double, double>& element){return element.first == current;});
	if(it != P.end()){
		// finding the last element within the new element
		auto bla = current+input.first;
		auto it2 = find_if(P.begin(),P.end(),[&bla](const std::pair<double, double>& element){return element.first == bla;});
		if(it2 != P.end()){
			auto index1 = distance(P.begin(),it); //start index
			auto index2 = distance(P.begin(),it2); //end index
			if(newElem){
				P[index2].second = P[index2-1].second;
			}
			for(int i = index1; i < index2; i++){
				P[i].second -= input.second;
			}
		}
	}
	P[P.size()-1].second = C;
}

pair<bool,int> emptyMachines(vector<vector<pair<double,double>>> M){
	bool b = false;
	int index = 0;
	for(int i = 0; i < M.size(); i++){
		if(M[i].empty()){
			b = true;
			index = i;
			return make_pair(b,index);
		}
	}
	return make_pair(b,index);
}

bool legal(vector<pair<double,double>> P, pair<double,double> j, int index){
	bool b = true;
	if(P.size() == 1){ //check om første række allerede er fyldt	
		return b;
	} else {
		for(int i = index+1; i < P.size(); i++){
			if(P[index].first+j.first > P[i].first){
				if(P[i].second < j.second){
					b = false;
				}
			}
		}
		return b;
	}
}

double sum2(vector<pair<double,double>> v, int index){
	double sum = 0.0;
	if(index != 0){
		for(int i = 0; i < index; i++){
			sum += v[i].first;
		}
	}

	return sum;
}

double listSchedulingCache(vector<pair<double,double>> input, double m){
	double C = 100.0;
	vector<vector<pair<double,double>>> M(m, vector<pair<double,double>>());
	vector<pair<double,double>> P;
	vector<pair<double,double>> fragmentation;
	P.push_back(make_pair(0.0,C));
	vector<double> p(m);
	bool placed = false;
	double alg = 0.0;
	bool flag = false;

	for(int i = 0; i < m; i++){
		p[i] = 0.0;
	}

	for(int i = 0; i < input.size(); i++){
		flag = false;
		for(int j = 0; j < P.size(); j++){
			if(!placed && P[j].second >= input[i].second && legal(P,input[i],j)){
				auto it = find(p.begin(),p.end(),P[j].first);
				if(it != p.end()){		//the makespan exists
					auto index = distance(p.begin(), it);
					M[index].push_back(input[i]);
					auto bla = P[j].first+input[i].first;
					if(find_if(P.begin(),P.end(),[&bla](const std::pair<double, double>& element){return element.first == bla;}) == P.end()){
						P.push_back(make_pair(P[j].first+input[i].first,P[j].second-input[i].second));
						sort(P.begin(), P.end(), [](const std::pair<double,double>& a, const std::pair<double,double>& b) {
  							return get<0>(a) < get<0>(b);
  						});
  						flag = true;
					}
  					updateCache(P,input[i],P[j].first,C,flag);
  					p[index] += input[i].first;
					placed = true;
				} else { //the makespan doesn't exist
					if(emptyMachines(M).first){
						auto index = emptyMachines(M).second;
						M[index].push_back(make_pair(P[j].first,0.0));
						M[index].push_back(input[i]);
						auto bla = P[j].first+input[i].first;
						if(find_if(P.begin(),P.end(),[&bla](const std::pair<double, double>& element){return element.first == bla;}) == P.end()){
							P.push_back(make_pair(P[j].first+input[i].first,P[j].second-input[i].second));
							sort(P.begin(), P.end(), [](const std::pair<double,double>& a, const std::pair<double,double>& b) {
  								return get<0>(a) < get<0>(b);
  							});
  							flag = true;
						}
  						updateCache(P,input[i],P[j].first,C,flag);
						p[index] = P[j].first+input[i].first;
						placed = true;
					} else {
						for(int k = 0; k < M.size(); k++){
							if(!placed && p[k] <= P[j].first){
								M[k].push_back(make_pair(P[j].first-p[k],0.0));
								M[k].push_back(input[i]);
								auto bla = P[j].first+input[i].first;
								if(find_if(P.begin(),P.end(),[&bla](const std::pair<double, double>& element){return element.first == bla;}) == P.end()){
									P.push_back(make_pair(P[j].first+input[i].first,P[j].second-input[i].second));
									sort(P.begin(), P.end(), [](const std::pair<double,double>& a, const std::pair<double,double>& b) {
  										return get<0>(a) < get<0>(b);
  									});
  									flag = true;
								}
  								updateCache(P,input[i],P[j].first,C,flag);
  								p[k] = P[j].first+input[i].first;
								placed = true;
							}
						}
						if(!placed){
							for(int k = 0; k < M.size(); k++){
								for(int l = 0; l < M[k].size(); l++){
									if(!placed && M[k][l].second == 0.0 && M[k][l].first >= input[i].first){
										if(sum2(M[k],l) <= P[j].first && sum2(M[k],l+1) > P[j].first){
											if(sum2(M[k],l) == P[j].first){
												auto it = M[k].begin()+l;
												auto n = M[k][l].first-input[i].first;
												if(n != 0){
													M[k].insert(it,make_pair(n,0.0));
  													M[k][l+1].first = n;
												}
												M[k][l].first = input[i].first;
												M[k][l].second = input[i].second;
											} else {
												if(P[j].first+input[i].first == sum2(M[k],l+1)){
													auto it = M[k].begin()+l+1;
													M[k].insert(it,make_pair(input[i].first,input[i].second));
													M[k][l].first -= input[i].first;
												} else {
													auto end = sum2(M[k],l+1)-(P[j].first+input[i].first);
													auto start = P[j].first-sum2(M[k],l);
													M[k][l].first = start;
													auto it1 = M[k].begin()+l+1;
													M[k].insert(it1,make_pair(input[i].first,input[i].second));
													auto it2 = M[k].begin()+l+2;
													M[k].insert(it2,make_pair(end,0.0));
												}
											}
											auto bla = P[j].first+input[i].first;
											if(find_if(P.begin(),P.end(),[&bla](const std::pair<double, double>& element){return element.first == bla;}) == P.end()){
												P.push_back(make_pair(P[j].first+input[i].first,P[j].second-input[i].second));
												sort(P.begin(), P.end(), [](const std::pair<double,double>& a, const std::pair<double,double>& b) {
  													return get<0>(a) < get<0>(b);
  												});
  												flag = true;
											}
  											updateCache(P,input[i],P[j].first,C,flag);
  											placed = true;	
										}
									}
								}
							}
						}
					}
				}
			}
		}
		placed = false;
	}
	for(int i = 0; i < M.size(); i++){
		alg = max(sum(M[i]).first,alg);
	}
	cout << "Printing solution: " << endl;
	printSchedule(M);
	return alg;
}

double optimalCache(vector<pair<double,double>> input, double m){
	sort(input.begin(), input.end(),[](const std::pair<double,double>& a, const std::pair<double,double>& b) {
  		return get<0>(a) < get<0>(b);
  	});

  	double alg = listSchedulingCache(input,m);

  	return alg;
}

#endif //MACHINE_HPP
