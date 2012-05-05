#include "d_hhm_prob.h"
#include "h_m_ovgraf.h"
#include "maindata.h"

D_HHM_Prob::D_HHM_Prob(void)
{
}

D_HHM_Prob::~D_HHM_Prob(void)
{
}

//1, if there exists a path from a state q1 to a state q2 with the mark "word", else - 0 
int D_HHM_Prob::AssociatedCondFlag(int q1,std::string word,int q2){
	int state,q;
	size_t i;
	q = q1;
	for(i = 0; i < word.size(); i++){
		int pos = MainData::AToi(word.at(i));
		state = MainData::D_HHMTrans[q][pos];
		if(state == -1){
			return 0;
		}
		q = state;
	}
	if(state == q2){
		return 1;
	}
	return 0;
};

//1,if there exists path from initial state to q with a mark word
int D_HHM_Prob::AssociatedFlag(std::string word,int q){
	int i;
	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		if(AssociatedCondFlag(i,word,q)){
			return 1;
		}
	}
	return 0;
};

vector<int> D_HHM_Prob::CalcAssociatedStates(std::string word){
	vector<int> vec;
	int i;
	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		if(AssociatedFlag(word,i) == 1){
			vec.push_back(i);
		}
	}
	return vec;
}



//calculates Prob_q1(word|q2)
double D_HHM_Prob::TermCondProb(int q1,std::string word, int q2){
	if(word.length() == 0){
		if(q1 == q2){
			return 1;
		}
		else{
			return 0;
		}
	}

	int q;
	double p = 1;
	size_t i;
	q = q1;
	for(i = 0; i < word.size(); i++){
		int pos = MainData::AToi(word.at(i));
		p = p*MainData::D_HHMProbs[q][pos];
		q = MainData::D_HHMTrans[q][pos];
		if(q == -1){
			return 0;
		}
	}
	if(q == q2){
		return p;
	}else{
		return 0;
	}
};

//calculates Prob_q(word)
double D_HHM_Prob::TermProb(std::string word, int q){
	double p = TermCondProb(0,word,q);
	return p;
};

//calculates probability of transition from q1 to q2
double D_HHM_Prob::TransitionProb(int q1, int q2){
	double p = 0;
	int i;
	for(i = 0; i < MainData::AlpSize; i++){
		int state = MainData::D_HHMTrans[q1][i];
		if(state == q2){
			p += MainData::D_HHMProbs[q1][i];
		}
	}
	return p;
};

//gets all states q' such that exisis a symbol a for that q'=Ô(q,a)
vector<int> D_HHM_Prob::ConsistStates(int q){
	vector<int> vec;
	int i,j;
	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		for(j = 0; j < MainData::AlpSize; j++){
			if(MainData::D_HHMTrans[i][j] == q){
				vec.push_back(i);
				break;
			}	
		}
	}
	return vec;
};

void D_HHM_Prob::Debug(void){
	int flag = AssociatedCondFlag(0,"AC",1);
	double p = TermProb("A",1);
	vector<int> vec = ConsistStates(1);
};


