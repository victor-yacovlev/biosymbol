#include "mmodel_prob.h"
#include "m_trtree.h"
#include "maindata.h"
#include <iostream>

int MModel_Prob::Power = 1; //Alp^order
double* MModel_Prob::IniProbs;


MModel_Prob::MModel_Prob(void)
{
}

MModel_Prob::~MModel_Prob(void)
{
} 

////////////////////////////////////////////////
int MModel_Prob::NumPower(int num, int step){
	int i;
	int pow = 1;
	for(i = 0; i < step; i++){
		pow = pow*num;
	}
	return pow;
}

///////////1. functions to work with words//////////////////////

//for a word alpha gives number of the word in the lexicographic order.
int MModel_Prob::Code(std::string alpha){
	size_t i;
	int p;
	int cd = 0;
	size_t k = alpha.length();
	const char* st = alpha.c_str();
	if(k == 0){
		return -1;
	}
	
	p = 1;
	for(i = 0; i < k -1; i++){
		p = p*MainData::AlpSize;
	}
	
	for(i = 0; i < k; i++){
		cd = cd + p*(MainData::AToi(st[i]));
		p = p/MainData::AlpSize;
	}
	return cd; 
}

//gives words of length k with number cd in the lexicographic order
std::string MModel_Prob::deCode(int k, int cd){
	std::string alpha;
	if(cd == -1){
	   return alpha;
	}
	int p = 1;
	int s, i;
	for(i = 0; i < k -1; i++){
		p = p*MainData::AlpSize;
	}
	for(i = 0; i < k; i++){
		s = cd/p;
		cd = cd - s*p;
		p = p/MainData::AlpSize;
		alpha += MainData::IToa(s);
	}
	return alpha;
};


//gives prefix of length k
std::string MModel_Prob::Prefix(int k, std::string word){
	std::string pref = word;
	pref.erase(pref.begin()+k,pref.end());
return pref;
};

int MModel_Prob::PrefixN(int len, std::string word){
	int cd = 0;
	int pow = NumPower(MainData::AlpSize,len -1);
	int i;
	size_t m;
	const char* st = word.c_str();
	m = word.length();
	for(i = 0; i < len; i++){
		cd += MainData::AToi(st[i])*pow;
		pow = pow/MainData::AlpSize;
	}
	return cd;
};

//gives suffix of length k
std::string MModel_Prob::Suffix(int k, std::string word){
	std::string suf = word;
	suf.erase(suf.begin(),suf.end()-k);
	return suf;
};

//gives suffix of length k
int MModel_Prob::SuffixN(int len, std::string word){
	int cd = 0;
	int pow = NumPower(MainData::AlpSize,len -1);
	int i;
	size_t m;
	const char* st = word.c_str();
	m = word.length();
	for(i = 0; i < len; i++){
		cd += MainData::AToi(st[m - len + i])*pow;
		pow = pow/MainData::AlpSize;
	}
	return cd;
};
///////////////////////////////////
//1 - if q1 in Q(w,q2), 0- otherwise
int MModel_Prob::AssociatedCondFlag(int q1, std::string word, int q2){
	int cd,pow;
	size_t s1 = word.size();
	int s = (int)s1;
	int res = 0;
	pow = Power/MainData::AlpSize;
	if(s >= MainData::order){	
		cd = SuffixN(MainData::order,word);
		if(cd == q2){
			res = 1;
		}
	}
	else{
		int d = MainData::order - s;
		pow = NumPower(MainData::AlpSize,d);
		cd = q1/pow;
		cd = q1 - cd*pow;
		pow = Power/pow;
		cd = cd*pow + Code(word);
		if(cd == q2){
			res = 1;
		}
	}
	return res;
}

//1- if word is consistent with the state q
int MModel_Prob::AssociatedFlag(std::string word, int q){
	std::string suf;
	int cd;
	size_t s1 = word.size();
	int s = (int)s1;
	int res = 0;
	if(word.size() == 0){
		return 1;
	}
	if(s >= MainData::order){
		cd = SuffixN(MainData::order, word);
		if(cd == q){
			res = 1;
		}
	}
	else{
		int pow = NumPower(MainData::AlpSize,s);
		cd = q/pow;
		cd = q - cd*pow;
		int cd1;
		cd1 = Code(word);
		if(cd == cd1){
			res = 1;
		}
	}
	return res;
}

vector<int> MModel_Prob::CalcAssociatedStates(std::string word){
	vector<int> vec;
	size_t s1 = word.length();
	int s = int(s1);
	if(s > MainData::order){
		int cd = SuffixN(MainData::order,word);
		vec.push_back(cd);
	}
	else{
		int i;
		int d = MainData::order - s;
		int cd1,cd2, cd;
		cd1 = 0;
		cd2 = NumPower(MainData::AlpSize,d) - 1;
		cd = Code(word);
		int pow = NumPower(MainData::AlpSize,s);
		for( i = cd1; i <= cd2; i++){
			int state = i*pow;
			state += cd;
			vec.push_back(state);
		}
	}
	return vec;
};

//gives probability Prob_q2(word|q1)
double MModel_Prob::TermCondProb(int q1, std::string word, int q2){
	if(word.length() != 0){
		size_t i = 0;
		int cd = q1;
		int pow = Power/MainData::AlpSize;
		double p = 1;
		size_t s = word.length();
		const char* st = word.c_str();	
		for(i = 0; i < s; i++){
			p = p*MainData::MarkovProbs[MainData::AToi(st[i])][cd];
			int cd1 = cd/pow;
			cd = cd - cd1*pow;
			cd = cd*MainData::AlpSize;
			cd += MainData::AToi(st[i]);
		}
		return p;
	}
	else{
		return 1;
	}
}

//gives Prob_q(word)
double MModel_Prob::TermProb(std::string word, int q){
	size_t s1 = word.length();
	int s = int(s1);
	if( s!= 0){
		double p = 0;
		if(s < MainData::order){
			p = IniProbs[q];
		}
		else{
			int cd = PrefixN(MainData::order, word);
			std::string word1 = word;
			word1.erase(0, MainData::order);
			p = IniProbs[cd]*TermCondProb(cd,word1,q);
		}
		return p;
	}
	else{
		return 1;
	}
}

//gives list of states consistent with q
vector<int> MModel_Prob::ConsistStates(int q){
	int i;
	vector<int> vec;
	int pow = Power/MainData::AlpSize;
	for(i = 0; i < MainData::AlpSize; i++){
		int cd = q/MainData::AlpSize;
		cd += i*pow;
		vec.push_back(cd);
	}
	return vec;
} 

/////////////////////Initial distribution////////////////////////////

int** TrMatrix = NULL;

void MModel_Prob::CrTrMatrix(void){
	int i,j,cd;
	int pow = MModel_Prob::Power/MainData::AlpSize;
	TrMatrix = new int*[MModel_Prob::Power];
	for(i = 0; i < MModel_Prob::Power; i++){
		TrMatrix[i] = new int[MainData::AlpSize];
		for(j = 0; j < MainData::AlpSize; j++){
			cd = i/MainData::AlpSize;
			cd += j*pow;
			TrMatrix[i][j] = cd;
		}
	}
	return;
}



double VecProduct(void){
	int i,j,pos;
	int s;
	double* vec = new double[MModel_Prob::Power];
	int pow = MModel_Prob::Power/MainData::AlpSize;

	for(i = 0; i < MModel_Prob::Power; i++){
		vec[i] = 0;
		s = i/MainData::AlpSize;
		s = i - s*MainData::AlpSize;
		for(j = 0; j < MainData::AlpSize; j++){
			pos = TrMatrix[i][j];
			vec[i] += MModel_Prob::IniProbs[pos]*MainData::MarkovProbs[s][pos];
		}
	}
	double norm = 0;
	double p;
	for(i = 0; i < MModel_Prob::Power; i++){
		p = MModel_Prob::IniProbs[i] - vec[i];
		if(p < 0){
			p = -1*p;
		}
		norm += p;
	}

	for(i = 0; i < MModel_Prob::Power; i++){
		MModel_Prob::IniProbs[i] = vec[i];
	}
	delete[] vec;
	return norm;
}

void MModel_Prob::SetIniProbs(void){
	int i;
	for( i = 1; i < Power; i++){
		IniProbs[i] = 0;
	}
	IniProbs[0] = 1;

	double norm = 1;
	i = 0;
	
	while((i < 1000)&&(norm > 0.0000000001)){
		norm = VecProduct();
		i++;
	}
	

	for(i = 0; i < Power; i++){
		delete[] TrMatrix[i];
	}
	delete[] TrMatrix;
	return;
}

void MModel_Prob::TransStepProbCalc(double* &Mass){
int i,j,pos,s;
	if(MainData::order <= 5){
		double vec[NMaxStates];
	
		for(i = 0; i < MModel_Prob::Power; i++){
			vec[i] = 0;
			s = i/MainData::AlpSize;
			s = i - s*MainData::AlpSize;
			for(j = 0; j < MainData::AlpSize; j++){
				pos = TrMatrix[i][j];
				vec[i] += Mass[pos]*MainData::MarkovProbs[s][pos];
			}
		}

		for(i = 0; i < MModel_Prob::Power; i++){
			Mass[i] = vec[i];
		}
	}
	else{
		double* vec = new double[MModel_Prob::Power];
	
		for(i = 0; i < MModel_Prob::Power; i++){
			vec[i] = 0;
			s = i/MainData::AlpSize;
			s = i - s*MainData::AlpSize;
			for(j = 0; j < MainData::AlpSize; j++){
				pos = TrMatrix[i][j];
				vec[i] += Mass[pos]*MainData::MarkovProbs[s][pos];
			}
		}

		for(i = 0; i < MModel_Prob::Power; i++){
			Mass[i] = vec[i];
		}	
		delete[] vec;
	}
	return;
}

