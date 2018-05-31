#ifndef CLUSTERING_HPP
#define CLUSTERING_HPP

#include <vector>
#include <ostream>
#include <algorithm>
#include <numeric>
#include <limits>
#include <set>
#include <map>
#include <cfloat> // DBL_MAX
#include <cmath> // std::nextafter

using namespace std;

//Lav input til algoritmerne tilfældigt
void createInput(vector<float> &input, int n, float k){
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0.0,nextafter(k,DBL_MAX));
	for(int i = 0; i < n; i++){
		input.push_back(dis(gen));
	}
}

//Print alle clusters
void printClusters(vector<vector<float>> V){
	for(int i = 0; i < V.size(); i++){
		cout << "{";
		for(int j = 0; j < V[i].size()-1; j++){
			cout << V[i][j] << ", ";
		}
		cout << V[i][V[i].size()-1] << "}" << endl;
	}
}

//Udregn afstanden mellem to punkter
float dist(float p, float endpoint){
	float dist = endpoint - p;
	return abs(dist);
}

int onlineBenefitGreedy(vector<float> input, float &length){
	//Vector af vector; inderste vector er en cluster, og yderste vector indeholder alle cluster vectors
	vector<vector<float>> V;
	vector<float> v;
	bool update;

	for(int i = 0; i < input.size(); i++){
		int index = 0;
		int ratio = 0;
		if(V.empty()){
			v.push_back(input[i]);
			V.push_back(v);
			v.clear();
		} else {
			for(int j = 0; j < V.size(); j++){
				sort(V[j].begin(), V[j].end());
				if(max(dist(input[i],V[j][0]),dist(input[i],V[j][V[j].size()-1])) <= 1){
					if(input[i] < V[j][0] && length-(V[j][0]-input[i]) >= 0 && (V[j][0]-input[i])/(V[j].size()) > ratio){
						update = true;
						ratio = (V[j][0]-input[i])/(V[j].size());
						index = j;
					} else if(input[i] > V[j][V[j].size()-1] && length-(input[i]-V[j][V[j].size()-1]) >= 0 && (input[i]-V[j][0])/(V[j].size()) > ratio){
						update = true;
						ratio = (input[i]-V[j][0])/(V[j].size());
						index = j;
					} else if(input[i] < V[j][V[j].size()-1] && input[i] > V[j][0]){
						update = true;
						index = j;
					}
				}
			}
			if(update){
				if(input[i] < V[index][0]){
					length -= V[index][0]-input[i];
				} else if(input[i] > V[index][V[index].size()-1]){
					length -= input[i]-V[index][V[index].size()-1];
				}
				V[index].push_back(input[i]);
				index = 0; 
				ratio = 0;
				update = false;
			} else {
				v.push_back(input[i]);
				V.push_back(v);
				v.clear();
			}
		}
	}
	//cout << "length: " << length << endl;
	cout << "Printing solution:" << endl;
	printClusters(V);
	return V.size();
}

int onlineCount(vector<float> input, float &length){
	//Vector af vector; inderste vector er en cluster, og yderste vector indeholder alle cluster vectors
	vector<vector<float>> V;
	vector<float> v;
	bool place;
	int index;

	for(int i = 0; i < input.size(); i++){
		place = false;
		index = 0;
		for(int j = 0; j < V.size(); j++){
			sort(V[j].begin(), V[j].end());
			if(max(dist(input[i],V[j][0]),dist(input[i],V[j][V[j].size()-1])) <= 1){
				if(min(dist(input[i],V[j][0]),dist(input[i],V[j][V[j].size()-1])) <= 0.5){
					if(input[i] < V[j][V[j].size()-1] && input[i] > V[j][0]){
						place = true;
						index = j;
					} else if(input[i] < V[j][0] && length-(V[j][0]-input[i]) >= 0){
						place = true;
						index = j;
					} else if(input[i] > V[j][V[j].size()-1] && length-(input[i]-V[j][V[j].size()-1]) >= 0){
						place = true;
						index = j;
					}
				}
			}
		}
		if(place){
			if(input[i] < V[index][0]){
				length -= V[index][0]-input[i];
			} else if(input[i] > V[index][V[index].size()-1]){
				length -= input[i]-V[index][V[index].size()-1];
			}
			V[index].push_back(input[i]);
		} else {
			v.push_back(input[i]);
			V.push_back(v);
			v.clear();
		}
	}
	//cout << "length: " << length << endl;
	printClusters(V);
	return V.size();
}

//Normal OPT
int leftUnit(vector<float> input, float &length){
	//Vector af vector; inderste vector er en cluster, og yderste vector indeholder alle cluster vectors
	vector<vector<float>> V;
	vector<float> v;
	bool placed = false;

	//sort(input.begin(),input.end());

	for(int i = 0; i < input.size(); i++){
		if(V.empty()){
			v.push_back(input[i]);
			V.push_back(v);
			v.clear();
		} else {
			for(int j = 0; j < V.size(); j++){
				if(placed == false){
					if(input[i] < V[j][V[j].size()-1] && input[i] > V[j][0]){
						V[j].push_back(input[i]);
						
						placed = true;
					} else if(max(dist(input[i],V[j][0]),dist(input[i],V[j][V[j].size()-1])) <= 1 && length-(min(abs(input[i]-V[j][V[j].size()-1]),abs(V[j][0]-input[i]))) >= 0){
						V[j].push_back(input[i]);
						length -= input[i]-V[j][V[j].size()-2];
						placed = true;
					}
				}
				sort(V[j].begin(),V[j].end());	
			}
			if(placed == false){
				v.push_back(input[i]);
				V.push_back(v);
				v.clear();
			} else {
				placed = false;
			}
		}
	}
	//cout << "length: " << length << endl;
	printClusters(V);
	return V.size(); 
}

int discretizedUnitGreedy(vector<float> input, float k, float &length){
	//Vector af vector; inderste vector er en cluster, og yderste vector indeholder alle cluster vectors
    vector<int> M;
    for(int i = 0.0; i < k; i++){
    	M.push_back(0);
    }

    sort(input.begin(),input.end(),std::less<>());

    for(int i = 0; i < input.size(); i++){
    	M[floor(input[i])]++;
    }

	stable_sort(input.begin(),input.end(),[&M](auto &left, auto &right) {
    	return M[floor(left)] > M[floor(right)];
	});
	
	for(int i = 0; i < input.size(); i++){
		cout << input[i] << ", ";
	}
	cout << endl;
	
	auto res = leftUnit(input,length);

	return res;
}

void updateRatios(vector<float> input, vector<pair<int, float>> &foo){
	foo.clear();

	/* Initialize the vector */
	for(int i = 0; i < input.size(); i++){
		foo.push_back(make_pair(0,0.0));
	}

	/* Compare every point to all other points in the input
	Ensures that the biggest ratios are found for all i */
	if(input.size() > 1){
		for(int i = 0; i < input.size(); i++){
			for(int j = i + 1; j < input.size(); j++){
				if(input[j]-input[i] <= 1){
					float diff = input[j]-input[i];
					float ratio = diff/(j-i);
					if(foo[i].second < ratio){
						foo[i].second = ratio;
						foo[i].first = j;
					}
				}
			}
		}
	} else {
		foo[0].first = 0;
		foo[0].second = 0;
	}
}

int benefitGreedy(vector<float> input, float k, float &length){
	vector<vector<float>> V;
	/* Vector of pairs, indexed by the index in input
	Pair is made up of an index and a ratio */
	vector<pair<int,float>> foo;
	sort(input.begin(),input.end());

	/* Initialize the vector */
	for(int i = 0; i < input.size(); i++){
		foo.push_back(make_pair(0,0.0));
	}
	/* Compare every point to all other points in the input
	Ensures that the biggest ratios are found for all i */
	for(int i = 0; i < input.size(); i++){
		for(int j = i + 1; j < input.size(); j++){
			if(input[j]-input[i] <= 1){
				float diff = input[j]-input[i];
				float ratio = diff/(j-i);
				if(foo[i].second < ratio){
					foo[i].second = ratio;
					foo[i].first = j;
				}
			}
		}
	}

	while(input.size() > 0){
		/* Find index of the item with the largest ratio */

		auto index = distance(foo.begin(), max_element(foo.begin(), foo.end(), [](const std::pair<int,float>& a, const std::pair<int,float>& b) {
  			return a.second < b.second;
  		}));

		vector<float> v;

		if(input.size() == 1){
			bool placed = false;;
			pair<int,float> bar;
			bar.first = 0;
			bar.second = 0;
			for(int i = 0; i < V.size(); i++){
				sort(V[i].begin(),V[i].end());
				if(max(dist(input[0],V[i][0]),dist(input[0],V[i][V[i].size()-1])) <= 1){
					if(input[0] < V[i][0] && length-(V[i][0]-input[0]) >= 0){
						if(bar.second < (V[i][0]-input[0])/(V[i].size())){
							bar.first = i;
							bar.second = (V[i][0]-input[0])/(V[i].size());
							placed = true;
						}
					} else if(input[0] > V[i][0] && input[0] > V[i][V[i].size()-1] && length-(input[0]-V[i][V[i].size()-1]) >= 0){
						if(bar.second < (input[0]-V[i][0])/(V[i].size())){
							bar.first = i;
							bar.second = (input[0]-V[i][0])/(V[i].size());
							placed = true;
						}
					} else if(input[0] > V[i][0] && input[0] < V[i][V[i].size()-1]){
						if(bar.second < (V[i][V[i].size()-1]-V[i][0])/(V[i].size())){
							bar.first = i;
							bar.second = (V[i][V[i].size()-1]-V[i][0])/(V[i].size());
							placed = true;
						}
					}
				}
			}
			if(placed){
				if(input[0] < V[bar.first][0]){
					length -= V[bar.first][0]-input[0];
				} else if(input[0] > V[bar.first][0] && input[0] > V[bar.first][V[bar.first].size()-1]){
					length -= input[0]-V[bar.first][V[bar.first].size()-1];
				}
				V[bar.first].push_back(input[0]);
			} else {
				vector<float> v;
				v.push_back(input[0]);
				V.push_back(v);
				v.clear();
			}
			input.clear();
		} else if((length - (input[foo[index].first]-input[index])) >= 0){
			for(int i = index; i <= foo[index].first; i++){
				v.push_back(input[i]);
			}
			V.push_back(v);
			v.clear();
			length -= (input[foo[index].first]-input[index]);
			input.erase(input.begin()+index, input.begin()+foo[index].first+1);
			if(!input.empty()){
				updateRatios(input,foo);
			}
		} else if(input.size() >= 1 && (length - (input[index+1]-input[index])) >= 0){
			int count = 0;
			for(int i = index; i <= foo[index].first; i++){
				if(length-(input[i]-input[index]) >= 0){
					v.push_back(input[i]);
					if(v.size() > 1){
						length -= v[v.size()-1]-v[v.size()-2];
					}
					count++;
				}
			}

			V.push_back(v);
			v.clear();
			input.erase(input.begin()+index, input.begin()+index+count);
			if(!input.empty()){
				updateRatios(input,foo);
			}
		} else {
			for(int j = index; j <= foo[index].first; j++){
				bool placed = false;;
				pair<int,float> bar;
				bar.first = 0;
				bar.second = 0;
				for(int i = 0; i < V.size(); i++){
					sort(V[i].begin(),V[i].end());
					if(max(dist(input[j],V[i][0]),dist(input[j],V[i][V[i].size()-1])) <= 1){
						if(input[j] < V[i][0] && length-(V[i][0]-input[j]) >= 0){
							if(bar.second < (V[i][0]-input[j])/(V[i].size())){
								bar.first = i;
								bar.second = (V[i][0]-input[j])/(V[i].size());
								placed = true;
							}
						} else if(input[j] > V[i][0] && input[j] > V[i][V[i].size()-1] && length-(input[j]-V[i][V[i].size()-1]) >= 0){
							if(bar.second < (input[j]-V[i][0])/(V[i].size())){
								bar.first = i;
								bar.second = (input[j]-V[i][0])/(V[i].size());
								placed = true;
							}
						} else if(input[j] > V[i][0] && input[j] < V[i][V[i].size()-1]){
							if(bar.second < (V[i][V[i].size()-1]-V[i][0])/(V[i].size())){
								bar.first = i;
								bar.second = (V[i][V[i].size()-1]-V[i][0])/(V[i].size());
								placed = true;
							}
						}
					}
				}
				if(placed){
					if(input[j] < V[bar.first][0]){
						length -= V[bar.first][0]-input[j];
					} else if(input[j] > V[bar.first][0] && input[j] > V[bar.first][V[bar.first].size()-1]){
						length -= input[j]-V[bar.first][V[bar.first].size()-1];
					}
					V[bar.first].push_back(input[j]);
				} else {
					vector<float> v;
					v.push_back(input[j]);
					V.push_back(v);
					v.clear();
				}
			}
			input.erase(input.begin()+index, input.begin()+foo[index].first+1);
			if(!input.empty()){
				updateRatios(input,foo);
			}
		}
	}
	cout << "Printing solution: " << endl;
	printClusters(V);
	return V.size();
}

int minDistGreedy(vector<float> input, float &length){
	vector<vector<float>> V;
	vector<float> v;
	vector<tuple<float,int,int>> M;
	vector<pair<bool,int>> I;

	sort(input.begin(), input.end());
	for(int i = 0; i < input.size()-1; i++){
		if(input[i+1]-input[i] <= 1){
			M.push_back(make_tuple(input[i+1]-input[i],i,i+1));
		} else {
			M.push_back(make_tuple(numeric_limits<double>::max(),i,i+1));
		}
		I.push_back(make_pair(false,0));
	}
	I.push_back(make_pair(false,0));

	sort(M.begin(), M.end(),[](const std::tuple<float,int,int>& a, const std::tuple<float,int,int>& b) {
  		return get<0>(a) < get<0>(b);
  	});

  	for(int i = 0; i < M.size(); i++){
  		if(length - get<0>(M[i]) >= 0){
  			if(I[get<1>(M[i])].first == false && I[get<2>(M[i])].first == false){		//case 1: none of them are in an existing cluster
  				v.push_back(input[get<1>(M[i])]);
  				v.push_back(input[get<2>(M[i])]);
  				sort(v.begin(), v.end());
  				V.push_back(v);
  				v.clear();
  				length -= get<0>(M[i]);
  				I[get<1>(M[i])].first = true;
  				I[get<1>(M[i])].second = V.size()-1;
  				I[get<2>(M[i])].first = true;
  				I[get<2>(M[i])].second = V.size()-1;
  			} else if(I[get<1>(M[i])].first == true && I[get<2>(M[i])].first == true && max(dist(V[I[get<1>(M[i])].second][0],V[I[get<2>(M[i])].second][V[I[get<2>(M[i])].second].size()-1]),dist(V[I[get<2>(M[i])].second][0],V[I[get<1>(M[i])].second][V[I[get<1>(M[i])].second].size()-1])) <= 1){ //case 2: both of them are in existing cluster, i.e., they need to be combined
  				int index = min(I[get<1>(M[i])].second,I[get<2>(M[i])].second);
  				int indexSnd = max(I[get<1>(M[i])].second,I[get<2>(M[i])].second);
  				V[index].insert(V[index].end(), V[indexSnd].begin(), V[indexSnd].end());
  				sort(V[index].begin(), V[index].end());
  				V.erase(V.begin()+indexSnd);
  				length -= get<0>(M[i]);
  				for(int k = 0; k <= i; k++){
  					if(I[get<1>(M[k])].first == true){
  						if(I[get<1>(M[k])].second == indexSnd){
  							I[get<1>(M[k])].second = index;
  						} else if(I[get<1>(M[k])].second > indexSnd){
							I[get<1>(M[k])].second--;
  						}
  					}
  					if(I[get<2>(M[k])].first == true){
  						if(I[get<2>(M[k])].second == indexSnd){
  							I[get<2>(M[k])].second = index;
  						} else if(I[get<2>(M[k])].second > indexSnd){
							I[get<2>(M[k])].second--;
  						}
  					}
  				}
  			} else { //case 3: one of them are in an existing cluster
  				if(I[get<1>(M[i])].first == true && dist(V[I[get<1>(M[i])].second][0],input[get<2>(M[i])]) <= 1){
  					V[I[get<1>(M[i])].second].push_back(input[get<2>(M[i])]);
  					sort(V[I[get<1>(M[i])].second].begin(),V[I[get<1>(M[i])].second].end());
  					length -= get<0>(M[i]);
  					I[get<2>(M[i])].first = true;
  					I[get<2>(M[i])].second = I[get<1>(M[i])].second;
  				} else if(I[get<2>(M[i])].first == true && dist(V[I[get<2>(M[i])].second][0],input[get<1>(M[i])]) <= 1){
  					V[I[get<2>(M[i])].second].push_back(input[get<1>(M[i])]);
  					sort(V[I[get<2>(M[i])].second].begin(),V[I[get<2>(M[i])].second].end());
  					length -= get<0>(M[i]);
  					I[get<1>(M[i])].first = true;
  					I[get<1>(M[i])].second = I[get<2>(M[i])].second;
  				} else {
  					if(I[get<1>(M[i])].first == false){
  						v.push_back(input[get<1>(M[i])]);
  						V.push_back(v);
  						v.clear();
  						I[get<1>(M[i])].first = true;
  						I[get<1>(M[i])].second = V.size()-1;
  					}
  					if(I[get<2>(M[i])].first == false){
  						v.push_back(input[get<2>(M[i])]);
  						V.push_back(v);
  						v.clear();
  						I[get<2>(M[i])].first = true;
  						I[get<1>(M[i])].second = V.size()-1;
  					}
  				}
  			}
  		} else {
  			if(I[get<1>(M[i])].first == false){
  				v.push_back(input[get<1>(M[i])]);
  				V.push_back(v);
  				v.clear();
  				I[get<1>(M[i])].first = true;
  			}
  			if(I[get<2>(M[i])].first == false){
  				v.push_back(input[get<2>(M[i])]);
  				V.push_back(v);
  				v.clear();
  				I[get<2>(M[i])].first = true;
  			}
  		}
  	}
  	cout << "Printing solution: " << endl;
  	printClusters(V);
	return V.size();
}

void updateCount(vector<float> &input, vector<pair<int,int>> &indexCounts){
	int count = 0;
	indexCounts.clear();
	for(int i = 0; i < input.size(); i++){
		for(int j = i + 1; j < input.size(); j++){
			if(input[j]-input[i] <= 1){
				count++;
			}
		}
		indexCounts.push_back(make_pair(i,count));
		count = 0;
	}

	sort(indexCounts.begin(), indexCounts.end(), [](auto &left, auto &right) {
    	return left.second > right.second;
	});
}

int floatingUnitGreedy(vector<float> input, vector<vector<float>> &V, float &length){
	vector<pair<int,int>> indexCounts; //index it starts, followed by freq
	int count = 0;

	sort(input.begin(),input.end());

	for(int i = 0; i < input.size(); i++){
		for(int j = i+1; j < input.size(); j++){
			if(input[j]-input[i] <= 1){
				count++;
			}
		}
		indexCounts.push_back(make_pair(i,count));
		count = 0;
	}

	sort(indexCounts.begin(), indexCounts.end(), [](auto &left, auto &right) {
    		return left.second > right.second;
	});
	
	vector<float> v;
	
	while(input.size() > 0){
		int i = 0;
		
		if(indexCounts[i].second != 0 && input[indexCounts[i].first+indexCounts[i].second]-input[indexCounts[i].first] <= 1 && (length-(input[indexCounts[i].first+indexCounts[i].second]-input[indexCounts[i].first])) >= 0){
			for(int j = indexCounts[i].first; j <= (indexCounts[i].first+indexCounts[i].second); j++){
				v.push_back(input[j]);
			}
			V.push_back(v);
			length -= v[v.size()-1]-v[0];
			v.clear();
			input.erase(input.begin()+indexCounts[i].first, input.begin()+(indexCounts[i].first+indexCounts[i].second+1));
			if(!input.empty()){
				updateCount(input,indexCounts);
			}
		} else if(indexCounts[i].second != 0 && input[indexCounts[i].first+indexCounts[i].second]-input[indexCounts[i].first] <= 1 && input[indexCounts[i].first+1]-input[indexCounts[i].first] <= length){
			int c = 0; 
			for(int j = indexCounts[i].first; j <= (indexCounts[i].first+indexCounts[i].second); j++){	
				if(length >= 0 && input[j]-input[indexCounts[i].first] <= length){
					v.push_back(input[j]);
					c++;
				}
			}
			V.push_back(v);
			length -= v[v.size()-1]-v[0];
			v.clear();
			
			input.erase(input.begin()+indexCounts[i].first, input.begin()+(indexCounts[i].first+c));
			
			if(!input.empty()){
				updateCount(input,indexCounts);
			}
		} else {
			for(int k = 0; k < input.size(); k++){
				v.push_back(input[k]);
				V.push_back(v);
				v.clear();
			}
			input.erase(input.begin(),input.end());
		}
	}
	printClusters(V);
	return V.size();
}

#endif // CLUSTERING_HPP
