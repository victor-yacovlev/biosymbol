#include "nd_hhm_prob.h"
#include "h_m_ovgraf.h"
#include "maindata.h"

ND_HHM_Prob::ND_HHM_Prob(void)
{
}

ND_HHM_Prob::~ND_HHM_Prob(void)
{
}


int ND_HHM_Prob::AssociatedCondFlag(int q1,std::string word,int q2){
	size_t i;
	int pos = MainData::AToi(word.at(0));
	size_t s = MainData::ND_HHMTrans[q1][pos].size();
	if(word.size() == 1){
		for(i = 0; i < s; i++){
			if(MainData::ND_HHMTrans[q1][pos][i] == q2){
				return 1;
			}
		}
	}
	else{
		word.erase(0,1);
		for(i = 0; i < s; i++ ){
			int state = MainData::ND_HHMTrans[q1][pos][i];
			if(AssociatedCondFlag(state,word,q2)){
				return 1;
			}
		}
	}
	return 0;
};

int ND_HHM_Prob::AssociatedFlag(std::string word,int q){
	int i;
	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		if(AssociatedCondFlag(i,word,q)){
			return 1;
		}
	}
	return 0;
};
 
vector<int> ND_HHM_Prob::CalcAssociatedStates(std::string word){
	vector<int> vec;
	int i;
	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		if(AssociatedFlag(word,i) == 1){
			vec.push_back(i);
		}
	}
	return vec;
}


double ND_HHM_Prob::TermCondProb(int q1,std::string word,int q2){
	if(word.length() == 0){
		if(q1 == q2){
			return 1;
		}
		else{
			return 0;
		}
	}
	
	double prob = 0;
	size_t i;
	int pos = MainData::AToi(word.at(0));
	if(word.size() == 1){
		prob = MainData::ND_HHMProbs[q1][q2][pos];
	}
	else{
		size_t s = MainData::ND_HHMTrans[q1][pos].size();
		word.erase(0,1);
		for(i = 0; i < s; i++){
			int state = MainData::ND_HHMTrans[q1][pos][i];
			double p = MainData::ND_HHMProbs[q1][state][pos];
			prob += TermCondProb(state,word,q2)*p;
		}
	}
	return prob;
};

double ND_HHM_Prob::TermProb(std::string word, int q){
	double p = TermCondProb(0,word,q);
	return p;
};

double ND_HHM_Prob::TransitionProb(int q1, int q2){
	double p = 0;
	int i;
	for(i = 0; i < MainData::AlpSize; i++){
		p += MainData::ND_HHMProbs[q1][q2][i];
	}
	return p;
};

vector<int> ND_HHM_Prob::ConsistStates(int q){
	vector<int> vec;
	int i,j;
	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		for(j = 0; j < MainData::AlpSize; j++){
			if(MainData::ND_HHMProbs[i][q][j] != 0){
				vec.push_back(i);
				break;
			}
		}
	}
	return vec;
};

void ND_HHM_Prob::Debug(void){
	int flag = AssociatedCondFlag(0,"AC",1);
	double p = TermProb("",4);
	vector<int> vec = ConsistStates(1);
};
