#include <vector>
#include <ostream>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>

using namespace std;

void printReservation(std::vector<std::vector<std::pair<int,int>>> &seatRes){
	for(int i = 0; i < seatRes.size(); i++){
		for(int j = 0; j < seatRes[i].size(); j++){
			if(seatRes[i].size() > 0){
				std::cout << "(" << seatRes[i][j].first << "," << seatRes[i][j].second << ")" << ' ';
			}
		}
		std::cout << "\n";
	}
}

std::vector<int> makeQs(std::vector<int> &q, int p, int m){
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1,p);

	for(int i = 0; i < m; i++){
		q.push_back(dis(gen));
	}
	return q;
}

std::vector<std::vector<int>> makeCutsVector(std::vector<std::vector<int>> &cuts, std::vector<int> &q, int k, int m){
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(2,k-1);
	int j;

	std::vector<int> aux;

	for(int w = 0; w < m; w++){
		for(int n = 0; n < q[w]-1; n++){
			j = dis(gen);
			aux.push_back(j);
		}
		std::sort(aux.begin(),aux.end());
		auto it = unique(aux.begin(),aux.end());

		aux.resize(distance(aux.begin(),it));
		cuts.push_back(aux);
		aux.clear();
	}

	return cuts;
}

void makeCuts(std::vector<std::vector<std::pair<int,int>>> &seatRes, std::vector<std::vector<int>> &cuts, int k){
	for(int i = 0; i < seatRes.size(); i++){
		if(cuts[i].size() != 0) {
			seatRes[i].push_back(std::make_pair(1,cuts[i][0]));
		}
	}
	for(int i = 0; i < seatRes.size(); i++){
		if(cuts[i].size() >= 1){
			for(int y = 1; y < cuts[i].size(); y++){
				seatRes[i].push_back(std::make_pair(cuts[i][y-1],cuts[i][y]));
			}
		}
	}
	for(int i = 0; i < seatRes.size(); i++){
		if(seatRes[i].size() > 0){
			int q = seatRes[i][seatRes[i].size()-1].second;
			seatRes[i].push_back(std::make_pair(q,k));
		} else {
			seatRes[i].push_back(std::make_pair(1,k));
		}
	}
}

void fixCuts(std::vector<std::vector<std::pair<int,int>>> &seatRes, int m, int stations) {
  int offset = m / 2;
  int maxCount = m / 4;

  // For det antal vogne vi har
  for(int i = 0; i < 2; i++) {
    std::map<int, int> startStations;
    std::map<int, int> endStations;

    // Initialiser histogram over start og slut stationer til 0
    for(int j = 0; j < m; j++) {
      startStations[j] = endStations[j] = 0;
    }

    for(int k = 0; k < offset; k++) {
      std::set<int> toRemove;
      auto& currentRow = seatRes[offset * i + k];

      // Tæl hvor mange gange gange hver start og slut station optræder
      // i nuværende vogn
      for(int j = 0; j < currentRow.size(); j++) {
        auto p = currentRow[j];
        // Og husk hvilke vi skal fjerne hvis der er for mange
        if (p.first != 1) {
          if(++startStations[p.first] > maxCount) {
            toRemove.insert(j);
          }
        }

        // --- || ---
        if (p.second != stations) {
          if(++endStations[p.second] > maxCount) {
            toRemove.insert(j);
          }
        }
      }

      // Lav en ny vektor der indeholder vores nye række hvis
      // vi altså gerne vil slette nogen pairs fra den nuværende
      auto newRow = std::vector<std::pair<int, int>>();

      // Kopier dem vi gerne vil beholde til den nye vector
      for(int j = 0; j < currentRow.size(); j++) {
        if(toRemove.find(j) == toRemove.end()) {
          newRow.push_back(currentRow[j]);
        }
      }
      if (newRow.empty()) {
        newRow.push_back(std::make_pair(1, stations));
      }

      // Erstat den nuværende række med den nye der kun indeholder dem vi gerne ville beholde
      currentRow = newRow;
    }
  }
}

// Tjek om to reservationer overlapper, og find ledigt interval
std::pair<bool,int> overlap(std::pair<int,int> one, std::vector<std::pair<int,int>> two, int k){
	bool b = false;
	int min = k-1;
	for(int i = 0; i < two.size(); i++){
		if(one.first < two[i].second && one.second > two[i].first){
			b = true;
		}
	}

	if(!b){
		if(two[0].first >= one.second){ // if first
			min = (two[0].first)-1;
		} else if(two[two.size()-1].second <= one.first){ // if last
			min = k-(two[two.size()-1].second);
		} else {
			for(int i = 0; i < two.size()-1; i++){
			 	if(two[i].second <= one.first && two[i+1].first >= one.second){ //If in the middle
					min = (two[i+1].first)-(two[i].second);
				}
			}
		}
	}
	return std::make_pair(b,min);
}

int bestFit(std::vector<std::pair<int,int>> perm, std::vector<std::vector<std::pair<int,int>>> &alg, int m, int k){
	int result = 0;
	bool placed = false;
	int startStations[2][k];
    int endStations[2][k];
    
    int seats = m/4;
    int cars = m/2;

    for(int j = 0; j < k; j++){
    	startStations[0][j] = startStations[1][j] = 0;
    	endStations[0][j] = endStations[1][j] = 0;
    }

    	alg[0].push_back(perm[0]);
    	if(perm[0].first != 1){
			startStations[0][perm[0].first]++;
		}
		if(perm[0].second != k){
			endStations[0][perm[0].second]++;
		}

    	for(int i = 1; i < perm.size(); i++){
    		int max = INT_MAX;
    		int index = 0;
    		for(int j = 0; j < m; j++){
    			std::sort(alg[j].begin(),alg[j].end());
    			bool lap;
    			int interval;
    			if(alg[j].size() == 0){
    				lap = false;
    				interval = k-1;
    			} else {
    				lap = (overlap(perm[i],alg[j],k)).first;
    				interval = (overlap(perm[i],alg[j],k)).second;
    			}
    			
    			if(!lap && max > interval && j < cars && startStations[0][perm[i].first] < seats && endStations[0][perm[i].second] < seats){
    				max = interval;
    				index = j;
    				placed = true;
    			} else if(!lap && max > interval && j >= cars && startStations[1][perm[i].first] < seats && endStations[1][perm[i].second] < seats){
    				max = interval;
    				index = j;
    				placed = true;
    			}
    		}
    		if(placed){
    			alg[index].push_back(perm[i]);
    			if(index < cars){
    				if(perm[i].first != 1){
						startStations[0][perm[i].first]++;
					}
					if(perm[i].second != k){
						endStations[0][perm[i].second]++;
					}
    			} else {
    				if(perm[i].first != 1){
						startStations[1][perm[i].first]++;
					}
					if(perm[i].second != k){
						endStations[1][perm[i].second]++;
					}
    			}
    		}
    		placed = false;
    	}

    for(int i = 0; i < alg.size(); i++){
		std::sort(alg[i].begin(),alg[i].end());
		
	}

	for(int i = 0; i < alg.size(); i++){
		//For Unit Price Problem
		//result += alg[i].size();
		//For Proportional Price Problem
		for(int j = 0; j < alg[i].size(); j++){
			result += alg[i][j].second-alg[i][j].first;
		}
	}
	return result;
}

int worstFit(std::vector<std::pair<int,int>> perm, std::vector<std::vector<std::pair<int,int>>> &alg, int m, int k){
	int result = 0;
	bool placed = false;
	int startStations[2][k];
    int endStations[2][k];
    
    int seats = m/4;
    int cars = m/2;

    for(int j = 0; j < k; j++){
    	startStations[0][j] = startStations[1][j] = 0;
    	endStations[0][j] = endStations[1][j] = 0;
    }

    	alg[0].push_back(perm[0]);
    	if(perm[0].first != 1){
			startStations[0][perm[0].first]++;
		}
		if(perm[0].second != k){
			endStations[0][perm[0].second]++;
		}

    	for(int i = 1; i < perm.size(); i++){
    		int max = 0;
    		int index = 0;
    		for(int j = 0; j < m; j++){
    			std::sort(alg[j].begin(),alg[j].end());
    			bool lap;
    			int interval;
    			if(alg[j].size() == 0){
    				lap = false;
    				interval = k-1;
    			} else {
    				lap = (overlap(perm[i],alg[j],k)).first;
    				interval = (overlap(perm[i],alg[j],k)).second;
    			}
    			
    			if(!lap && max < interval && j < cars && startStations[0][perm[i].first] < seats && endStations[0][perm[i].second] < seats){
    				max = interval;
    				index = j;
    				placed = true;
    			} else if(!lap && max < interval && j >= cars && startStations[1][perm[i].first] < seats && endStations[1][perm[i].second] < seats){
    				max = interval;
    				index = j;
    				placed = true;
    			}
    		}
    		if(placed){
    			alg[index].push_back(perm[i]);
    			if(index < cars){
    				if(perm[i].first != 1){
						startStations[0][perm[i].first]++;
					}
					if(perm[i].second != k){
						endStations[0][perm[i].second]++;
					}
    			} else {
    				if(perm[i].first != 1){
						startStations[1][perm[i].first]++;
					}
					if(perm[i].second != k){
						endStations[1][perm[i].second]++;
					}
    			}
    		}
    		placed = false;
    	}

    for(int i = 0; i < alg.size(); i++){
		std::sort(alg[i].begin(),alg[i].end());
	}

	for(int i = 0; i < alg.size(); i++){
		//For Unit Price Problem
		//result += alg[i].size();
		//For Proportional Price Problem
		for(int j = 0; j < alg[i].size(); j++){
			result += alg[i][j].second-alg[i][j].first;
		}
	}
	return result;
}

int firstFit(std::vector<std::pair<int,int>> perm, std::vector<std::vector<std::pair<int,int>>> &alg, int m, int k){
	int result = 0;
	bool placed = false;
    int startStations[2][k];
    int endStations[2][k];

    // Initialiser histogram over start og slut stationer til 0
    for(int j = 0; j < k; j++){
    	startStations[0][j] = startStations[1][j] = 0;
    	endStations[0][j] = endStations[1][j] = 0;
    }
 
	for(int i = 0; i < perm.size(); i++){
		for(int j = 0; j < alg.size(); j++){
			bool lap;
			if(alg[j].size() == 0){
    			lap = false;
    		} else {
    			lap = (overlap(perm[i],alg[j],k)).first;
    		}
			if(!placed && !lap){
				if(j < m/2 && startStations[0][perm[i].first] < m/4 && endStations[0][perm[i].second] < m/4){
					alg[j].push_back(perm[i]);
					if(perm[i].first != 1){
						startStations[0][perm[i].first]++;
					}
					if(perm[i].second != k){
						endStations[0][perm[i].second]++;
					}
					placed = true;
				} else if(j >= m/2 && startStations[1][perm[i].first] < m/4 && endStations[1][perm[i].second] < m/4){
					alg[j].push_back(perm[i]);
					if(perm[i].first != 1){
						startStations[1][perm[i].first]++;
					}
					if(perm[i].second != k){
						endStations[1][perm[i].second]++;
					}
					placed = true;
				}
			}
		}
		placed = false;
	}
	
	for(int i = 0; i < alg.size(); i++){
		std::sort(alg[i].begin(),alg[i].end());
	}
	
	for(int i = 0; i < alg.size(); i++){
		//For Unit Price Problem
		//result += alg[i].size();
		//For Proportional Price Problem
		for(int j = 0; j < alg[i].size(); j++){
			result += alg[i][j].second-alg[i][j].first;
		}
	}
	return result;
}

