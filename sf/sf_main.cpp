#include "maindata.h"
#include "ac_trie.h"
#include "ov_graf.h"
#include "bprob_graf.h"
#include "h_m_ovgraf.h"
#include "mmodel_prob.h"
#include "m_trtree.h"
#include "hhm_ovgraf.h"
#include "m_ovgraf.h"
#include "sf_main.h"
#include <cstring>
#include <sstream>

//////////////the functions for the server////////////////////////
extern "C" int func_set_input_data( int order, int mode, int TLen, int NOccur)
{
	std::string Error_line = "Error in the function 'set_input_data' \n";
	if(order < -2){
		cout<<Error_line;
		MainData::ErrorDetect(3);
		MainData::Error = 3;
		return 3;
	}
	
	if((mode < 0)||(mode > 5)){
		cout<<Error_line;
		MainData::Error = 10;
		MainData::ErrorDetect(10);
		return 10;
	}
	if((TLen < 0)||(NOccur < 0)){
		cout<<Error_line;
		MainData::Error = 20;
		MainData::ErrorDetect(20);
		return 20;
	}
	MainData::order = order;
	MainData::mode = mode;
	MainData::TLen = TLen;
	MainData::NOccur = NOccur;
	
	cout<<"Text length: "<<MainData::TLen<<'\n';
	cout<<"Number of occurences: "<<MainData::NOccur<<'\n';
	cout<<"Order of the probability model: "<<MainData::order<<'\n';
	cout<<"Type of pattern description: "<<MainData::mode<<'\n';

	return 0;
}


extern "C" int func_analis_alp_bern_data(int AlpSize, char* AlpMas, double* BernProb)
{
	std::string Error_line = "Error in the function 'analis_alp_bern_data' \n";
	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if(MainData::order != 0){
		cout<<Error_line;
		MainData::Error = 32;
		MainData::ErrorDetect(32);
		return 32;
	}
	if(AlpSize <= 0){
		cout<<Error_line;
		MainData::Error = 25;
		MainData::ErrorDetect(25);
		return 25;
	}
	
	int i;
	MainData::AlpSize = AlpSize;
	for(i = 0; i < AlpSize; i++){
		MainData::AlpMas[i] = AlpMas[i];
	}
	
	double sum = 0;
	for(i = 0; i < AlpSize; i++){
		MainData::BernProb[i] = BernProb[i];
		sum = sum + BernProb[i];
	}
	cout<<"Sum of the frequences: "<<sum<<'\n';
	if((sum < 0.999999)||(sum > 1.00001)){
		cout<<Error_line;
		MainData::Error = 34;
		MainData::ErrorDetect(34);
		return 34;
	}
	cout<<"Alphabet \n";
	for(i = 0; i < AlpSize; i++)
		cout<<MainData::AlpMas[i]<<'\t';
	cout<<"\n\n";
	cout<<"Probabilities"<<'\n';
	for(i = 0; i < AlpSize; i++)
		cout<<MainData::BernProb[i]<<'\t';
	cout<<'\n';
	return 0;
}

extern "C" int func_analis_alp_mark_data(int AlpSize, char* AlpMas, double* IniProbs, double** MarkovProbs)
{
	std::string Error_line = "Error in the function 'analis_alp_mark_data' \n";
	int i,j;

	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if(MainData::order <= 0){
		cout<<Error_line;
		MainData::Error = 32;
		MainData::ErrorDetect(32);
		return 32;
	}
	if(AlpSize <= 0){
		cout<<Error_line;
		MainData::Error = 25;
		MainData::ErrorDetect(25);
		return 25;
	}
	MainData::AlpSize = AlpSize;
	for(i = 0; i < AlpSize; i++){
		MainData::AlpMas[i] = AlpMas[i];
	}

	int power = MModel_Prob::NumPower(AlpSize,MainData::order);
	
	if(IniProbs != NULL){
		MainData::MarkovType = 1;
		MModel_Prob::IniProbs = new double[power];
		for(i = 0; i < power; i++){
			MModel_Prob::IniProbs[i] = IniProbs[i];
		}
	}

	MainData::MarkovProbs = new double*[MainData::AlpSize];
	for(i = 0; i < AlpSize; i++){
		MainData::MarkovProbs[i] = new double[power];
	}
	for(i = 0; i < AlpSize; i++){
		for(j = 0; j < power; j++){
			MainData::MarkovProbs[i][j] = MarkovProbs[j][i];
		}
	}
	MainData::CrDistribFlag = 1;
	
	cout<<"Alphabet \n";
	for(i = 0; i < AlpSize; i++)
		cout<<MainData::AlpMas[i]<<'\t';
	cout<<"\n\n";
	if(MainData::order <= 4){	
		cout<<"Probabilities distribution"<<'\n';
		for(j = 0; j < power; j++){
			for(i = 0; i < AlpSize; i++){
				cout<<MainData::MarkovProbs[i][j]<<'\t';
			}
			cout<<'\n';
		}
	}
	cout<<'\n';
	return 0;
}

extern "C" int func_analis_alp_dhhm_data(int AlpSize, char* AlpMas, int NumAllStates,
							  double** D_HHMProbs, int** D_HHMTrans)
{
	std::string Error_line = "Error in the function 'analis_alp_dhhm_data' \n";
	
	int i,j;

	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if(MainData::order != -1){
		cout<<Error_line;
		MainData::Error = 32;
		MainData::ErrorDetect(32);
		return 32;
	}
	if(AlpSize <= 0){
		cout<<Error_line;
		MainData::Error = 25;
		MainData::ErrorDetect(25);
		return 25;
	}
	
	MainData::AlpSize = AlpSize;
	for(i = 0; i < AlpSize; i++){
		MainData::AlpMas[i] = AlpMas[i];
	}
	
	if(NumAllStates < 0){
		cout<<Error_line;
		MainData::Error = 27;
		MainData::ErrorDetect(27);
		return 27;
	}
	H_M_OVGraf::NumAllStates = NumAllStates;
	
	MainData::D_HHMProbs = new double*[H_M_OVGraf::NumAllStates];
	MainData::D_HHMTrans = new int*[H_M_OVGraf::NumAllStates];
	
	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		MainData::D_HHMProbs[i] = new double[MainData::AlpSize];
		MainData::D_HHMTrans[i] = new int[MainData::AlpSize];
		for(j = 0; j < MainData::AlpSize; j++){
			MainData::D_HHMProbs[i][j] = D_HHMProbs[i][j];
			MainData::D_HHMTrans[i][j] = D_HHMTrans[i][j];	
		}
	}	
	MainData::CrDistribFlag = 1;
	
	cout<<"Alphabet \n";
	for(i = 0; i < AlpSize; i++)
		cout<<MainData::AlpMas[i]<<'\t';
	cout<<"\n\n";
	cout<<"Probabilities distribution"<<'\n';
	for(i =0; i < H_M_OVGraf::NumAllStates; i++){
		for(j = 0; j < MainData::AlpSize; j++){
			cout<< MainData::D_HHMProbs[i][j]<<'\t';
		}
		cout<<'\n';
	}
	cout<<'\n';
	cout<<"Transitions"<<'\n';
	for(i =0; i < H_M_OVGraf::NumAllStates; i++){
		for(j = 0; j < MainData::AlpSize; j++){
			cout<< MainData::D_HHMTrans[i][j]<<'\t';
		}
		cout<<'\n';
	}
	cout<<'\n';
	return 0;
}


extern "C" int func_analis_alp_hhm_data(int AlpSize, char* AlpMas,
							 int NumAllStates, double*** ND_HHMProbs)
{
	std::string Error_line = "Error in the function 'analis_alp_hhm_data' \n";
	int i,j;

	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if(MainData::order != -2){
		cout<<Error_line;
		MainData::Error = 32;
		MainData::ErrorDetect(32);
		return 32;
	}
	if(AlpSize <= 0){
		cout<<Error_line;
		MainData::Error = 25;
		MainData::ErrorDetect(25);
		return 25;
	}

	MainData::AlpSize = AlpSize;
	for(i = 0; i < AlpSize; i++){
		MainData::AlpMas[i] = AlpMas[i];
	}
	
	if(NumAllStates < 0){
		cout<<Error_line;
		MainData::Error = 27;
		MainData::ErrorDetect(27);
		return 27;
	}
	H_M_OVGraf::NumAllStates = NumAllStates;
	MainData::ND_HHMProbs = new double**[NumAllStates];
	MainData::ND_HHMTrans = new vector<int>*[NumAllStates];
	
	for(i = 0; i < NumAllStates; i++){
		MainData::ND_HHMProbs[i] = new double*[NumAllStates];
		MainData::ND_HHMTrans[i] = new vector<int>[AlpSize];			
		for(j = 0; j < NumAllStates; j++){
			MainData::ND_HHMProbs[i][j] = new double[AlpSize];
			int k;
			for(k = 0; k < AlpSize; k++){
				MainData::ND_HHMProbs[i][j][k] = ND_HHMProbs[i][j][k];
				if(ND_HHMProbs[i][j][k] != 0){
					MainData::ND_HHMTrans[i][k].push_back(j);
				}
			}
		}
	}

	cout<<"Alphabet \n";
	for(i = 0; i < AlpSize; i++)
		cout<<MainData::AlpMas[i]<<'\t';
	cout<<"\n\n";
	cout<<"Probabilities distribution"<<'\n';
	for(i = 0; i < NumAllStates; i++){			
		cout<<"State "<<i<<'\n';
		for(j = 0; j < NumAllStates; j++){
			int k;
			for(k = 0; k < AlpSize; k++){
				cout<<MainData::ND_HHMProbs[i][j][k]<<' ';
			}
				cout<<'\n';
		}
	}

	MainData::CrDistribFlag = 1;

	return 0;
}

extern "C" int func_analis_pattern_data_0(int NWords, char **WordsList)
{
	std::string Error_line = "Error in the function 'analis_pattern_data_0' \n";

	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if(MainData::mode != 0){
		cout<<Error_line;
		MainData::Error = 33;
		MainData::ErrorDetect(33);
		return 33;
	}
	
	if(NWords == 0){
		cout<<Error_line;
		MainData::Error = 23;
		MainData::ErrorDetect(23);
		return 23;
	}

	int i,j,k;
	AC_Trie::gTrie = new AC_Trie;
	
	MainData::WordLen = (int)strlen(WordsList[0]);
	int* DigitLine = new int[MainData::WordLen + 1];
	DigitLine[MainData::WordLen] = -1;
	
	int n = 0;
	int ind;
	for(i = 0; i < NWords; i++){
		if(strlen(WordsList[i]) == MainData::WordLen){					
			for(j = 0; j< MainData::WordLen; j++){
				k = MainData::AToi(WordsList[i][j]);
				if(k != -1){
					DigitLine[j] = k;
				}
				else{
					delete[] DigitLine;
					cout<<Error_line;
					MainData::Error = 12;
					MainData::ErrorDetect(12);
					return 12;
				}
			}
			ind = AC_Trie::gTrie->InsertWord(DigitLine,n+1);
			if(ind == 0){
				n++;
			}
		}
		else{
			delete[] DigitLine;
			cout<<Error_line;
			MainData::Error = 13;
			MainData::ErrorDetect(13);
			return 13;
		}
	}
	delete[] DigitLine;

	MainData::NWords = n;
	cout<<"Number of words in the pattern: "<<MainData::NWords<<'\n';
	cout<<"Length of the pattern "<<MainData::WordLen<<'\n';
	return 0;
}

extern "C" int func_analis_pattern_data_1(int NWords, int WordLen, double* RandPatProbs)
{
	std::string Error_line = "Error in the function 'analis_pattern_data_1' \n";
	
	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if(MainData::mode != 1){
		cout<<Error_line;
		MainData::Error = 33;
		MainData::ErrorDetect(33);
		return 33;
	}
	if(NWords <= 0){
		cout<<Error_line;
		MainData::Error = 23;
		MainData::ErrorDetect(23);
		return 23;
	}
	if(WordLen <= 0){
		cout<<Error_line;
		MainData::Error = 24;
		MainData::ErrorDetect(24);
		return 24;
	}

	int i;
	AC_Trie::gTrie = new AC_Trie;
	MainData::WordLen = WordLen;
	MainData::NWords = NWords;
	MainData::RandPatProbs = new double[MainData::AlpSize];
	if(RandPatProbs == NULL){
		for(i = 0; i < MainData::AlpSize; i++){
			MainData::RandPatProbs[i] = (double)1/MainData::AlpSize;
		}
	}
	else{
		double sum = 0;
		for(i = 0; i < MainData::AlpSize; i++){	
			MainData::RandPatProbs[i] = RandPatProbs[i];
			sum = sum + RandPatProbs[i];
		}
		if((sum < 0.999999)||(sum > 1.00001)){
			cout<<Error_line;
			MainData::Error = 10;
			MainData::ErrorDetect(10);
			return 10;
		}
	}
	MainData::GenRanWords();
	
	delete[] MainData::RandPatProbs;
	
	cout<<"Number of words in the pattern: "<<MainData::NWords<<'\n';
	cout<<"Length of the pattern "<<MainData::WordLen<<'\n';
	
	return 0;
}

extern "C" int func_analis_pattern_data_2_3(int WordLen, int NFootPrints, char **FootPrints, double** PssmMas, double Thr)
{
	std::string Error_line = "Error in the function 'analis_pattern_data_2_3' \n";
	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if((MainData::mode != 2)&&(MainData::mode != 3)){
		cout<<Error_line;
		MainData::Error = 33;
		MainData::ErrorDetect(33);
		return 33;
	}

	int i;
	AC_Trie::gTrie = new AC_Trie;

	MainData::WordLen = WordLen;
	MainData::PssmMas = new double*[MainData::WordLen];
	for(i = 0; i < MainData::WordLen; i++){
		MainData::PssmMas[i] = new double[MainData::AlpSize];
		int j;
		for(j = 0; j < MainData::AlpSize; j++){
			MainData::PssmMas[i][j] = PssmMas[i][j];
		}
	}
	if(MainData::mode == 2){
		MainData::Thr = Thr;
	}
	else{
		double t;
		MainData::Thr = 100;
		for(i = 0; i < NFootPrints; i++){
			t = MainData::CountThr(FootPrints[i]);
			 if((t < MainData::Thr) & (t!= -100))
				 MainData::Thr = t;
		}
	}
	int *word = new int[WordLen +1];
	word[WordLen] = -1;
	double* SMas = new double[MainData::WordLen];
	MainData::SetScorMas(SMas);
	MainData::GenPssmWords(word, 0, 0, SMas);
	delete[] word;
	delete[] SMas;
	SMas = NULL;
	
	cout<<"PSSM \n";
	for(i = 0; i < MainData::WordLen; i++){
		int j;
		for(j = 0; j < MainData::AlpSize; j++){
			cout<<MainData::PssmMas[i][j]<<'\t';
		}
		cout<<'\n';
	}
	cout<<'\n';
	if(MainData::mode == 3){
		cout<<"Cut-off: "<<MainData::Thr<<'\n';
	}
	cout<<"Number of words in the pattern: "<<MainData::NWords<<'\n';
	cout<<"Length of the pattern "<<MainData::WordLen<<'\n';

	return 0;
}

extern "C" int func_analis_pattern_data_4(char* motif, int Nreplace, int NConstPositions, 
									  int *ConstPositions)
{
	std::string Error_line = "Error in the function 'analis_pattern_data_4' \n";
	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if(MainData::mode != 4){
		cout<<Error_line;
		MainData::Error = 33;
		MainData::ErrorDetect(33);
		return 33;
	}
	if((strlen(motif) <= 0)||(Nreplace < 0)){
		cout<<Error_line;
		MainData::Error = 10;
		MainData::ErrorDetect(10);
		return 10;
	}
	int leng = (int)strlen(motif);
	if(Nreplace > leng- NConstPositions){
		cout<<Error_line;
		MainData::Error = 42;
		MainData::ErrorDetect(42);
		return 42;
	}
	int i;
	
	AC_Trie::gTrie = new AC_Trie;
	
	MainData::WordLen = (int)strlen(motif);
	MainData::motif = new int[MainData::WordLen + 1];
	MainData::motif[MainData::WordLen] = -1;
	MainData::motifstr = motif;
	for(i = 0; i < MainData::WordLen; i++){
		int Let = MainData::AToi(motif[i]);
		if( Let != -1){
			MainData::motif[i] = Let;
		}
		else{
			delete[] MainData::motif;
			cout<<Error_line;
			MainData::Error = 12;
			MainData::ErrorDetect(12);
			return 12;
		}
	}
	MainData::Nreplace = Nreplace;
	MainData::NWords ++;
	AC_Trie::gTrie->InsertWord(MainData::motif, MainData::NWords);
		
	int s = NConstPositions;
	if(s > 0){
		MainData::ConstPositions = new int[MainData::WordLen];
		for(i = 0; i < MainData::WordLen; i++){
			MainData::ConstPositions[i] = 0;
		}
		for(i = 0; i < s; i++){
			int pos = ConstPositions[i];
			if((0 <= pos)&&(pos < MainData::WordLen)){
				MainData::ConstPositions[pos] = 1;
			}
			else{
				delete[] MainData::motif;
				delete[] MainData::ConstPositions;
				cout<<Error_line;
				MainData::Error = 10;
				MainData::ErrorDetect(10);
				return 10;
			}
		}
	}
		
	int* word = new int[MainData::WordLen + 1];
	word[MainData::WordLen]= -1;
	for(i = 0; i < MainData::WordLen; i++){
		word[i] = MainData::motif[i];
	}
		
	MainData::MotifVariations(Nreplace, 0, word);
	delete[] MainData::motif;
	delete[] word;
	cout<<"Motif: "<<motif<<'\n';
	if(NConstPositions > 0){
		cout<<"Constant positions: \n";
		for(i = 0; i < NConstPositions; i++){
			cout<<ConstPositions[i]<<'\t';
		}
		cout<<'\n';
	}
	cout<<"Number of replacements: "<<Nreplace<<'\n';
	cout<<"Number of words in the pattern: "<<MainData::NWords<<'\n';
	cout<<"Length of the pattern "<<MainData::WordLen<<'\n';
	return 0;
}

extern "C" int func_analis_pattern_data_5(char *consensus, int NSymbols, char **ConsAlp)
{
	std::string Error_line = "Error in the function 'analis_pattern_data_5' \n";

	if(MainData::Error != 0){
		cout<<Error_line;
		MainData::ErrorDetect(31);
		return 31;
	}
	if(MainData::mode != 5){
		cout<<Error_line;
		MainData::Error = 33;
		MainData::ErrorDetect(33);
		return 33;
	}

	int i;
	AC_Trie::gTrie = new AC_Trie;
	
	int j;
	for(i = 0; i <NSymbols; i++){
		for(j =i+1; j < NSymbols; j ++){
			if(ConsAlp[i][0]==ConsAlp[j][0]){
				cout<<Error_line;
				MainData::Error = 36;
				MainData::ErrorDetect(36);
				return 36;
			}
		}
	}

	for(i = 0; i < NSymbols; i++){
		int s1 = (int)strlen(ConsAlp[i]);
		for(j = 1; j < s1; j++){
			if(MainData::AToi(ConsAlp[i][j])== -1){
				cout<<Error_line;
				MainData::Error = 37;
				MainData::ErrorDetect(37);
				return 37;
			
			}
			int k;
			for(k = j+1; k < s1; k++){
				if(ConsAlp[i][j]==ConsAlp[i][k]){
					cout<<Error_line;
					MainData::Error = 35;
					MainData::ErrorDetect(35);
					return 35;
				}
			}
		}
	}

	for(i = 0; i < MainData::AlpSize; i++){
		vector<char> vec;
		vec.push_back(MainData::AlpMas[i]);
		vec.push_back(MainData::AlpMas[i]);
		MainData::ConsAlp.push_back(vec);
	}
	int s;
	for(i = 0; i < NSymbols; i++){
		s = (int)strlen(ConsAlp[i]);
		vector<char> vec;
		for(j = 0; j < s; j++){
			vec.push_back(ConsAlp[i][j]);
		}
		MainData::ConsAlp.push_back(vec);
	}	



	MainData::consensusstr = consensus;
	MainData::WordLen = (int)strlen(consensus);
	MainData::consensus = new int[MainData::WordLen + 1];
	MainData::consensus[MainData::WordLen] = -1;
	for(j = 0; j < MainData::WordLen; j++){
		int Let = MainData::Pos_In_Cons_Alp(consensus[j]);
		if( Let != -1){
			MainData::consensus[j] = Let;
		}
		else{
			delete[] MainData::consensus;
			cout<<Error_line;
			MainData::Error = 18;
			MainData::ErrorDetect(18);
			return 18;
		}
	}
	int* word = new int[MainData::WordLen + 1];
	word[MainData::WordLen] = -1;
	MainData::Error = MainData::ConsVariations(0, word);
	if(MainData::Error > 0){
		MainData::ErrorDetect(MainData::Error);
		return MainData::Error;
	}
	delete[] word;
	delete[] MainData::consensus;

	cout<<"Consensus: "<<consensus<<'\n';
	cout<<"Consensus alphabet \n";
	s = (int)MainData::ConsAlp.size();
	for(i = 0; i < s; i++){
		int s1 = (int)MainData::ConsAlp[i].size();
		cout<<MainData::ConsAlp[i][0]<<" = {";
		for(j = 1; j <s1 - 1; j++){
			cout<<MainData::ConsAlp[i][j]<<',';
		}
		cout<<MainData::ConsAlp[i][s1-1]<<"}\n";
	}
	cout<<'\n';
	cout<<"Number of words in the pattern: "<<MainData::NWords<<'\n';
	cout<<"Length of the pattern "<<MainData::WordLen<<'\n';
	return 0;
}


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

extern "C" int func_main(double* pvalue, char** report, char* **ResWords, int *NWords){
	std::string Error_line = "Error in the function 'main' \n";

	int i;
	int ExitFlag = 0;
	
	if(MainData::Error != 0){
		ExitFlag = 1;
		MainData::Error = 31;
	}
	if(ExitFlag == 0){
		MainData::Error = MainData::Check_Out_of_Range();
		if(MainData::Error > 0){
			ExitFlag = 1;
		}
	}

	if((MainData::NWords == 0)&&(ExitFlag == 0)){
		MainData::Error = 23;
		ExitFlag = 1;
	}
	if((MainData::WordLen == 0)&&(ExitFlag == 0)){
		MainData::Error = 24;
		ExitFlag = 1;
	}

	if((MainData::NOccur == 0)&&(ExitFlag == 0)){
		*pvalue = 1;
		ExitFlag = 2;
	}
	
	if((MainData::TLen < MainData::WordLen)&&(ExitFlag == 0)){
		*pvalue = 0;
		ExitFlag = 2;
	}
	if((MainData::TLen == MainData::WordLen)&&(MainData::NOccur > 1)&&(ExitFlag == 0)){
		*pvalue = 0;
		ExitFlag = 2;
	}

	if((MainData::AlpSize == 1)&&(ExitFlag == 0)){
		if(MainData::TLen >= MainData::WordLen + MainData::NOccur - 1){
			*pvalue = 1;
			ExitFlag = 2;
		}
		else{
			*pvalue = 0;
			ExitFlag = 2;
		}
	}

    if((MainData::order > 0)&&(ExitFlag == 0)){
		if(MainData::order > MainData::WordLen){
			MainData::Error = 41;
			ExitFlag =  1;
		}
        M_TrTree::ForLeaves = new int[MainData::NWords];
    }


	std::ostringstream out;
	std::ostringstream outW;

	if(ExitFlag != 1){
		if(MainData::NWords < 10000){
		    AC_Trie::gTrie->PrintTrie(&outW);
		}
	}


	if(ExitFlag == 0){
		AC_Trie::CreateTrie();

	    if(MainData::order < 0){
			HHM_OVGraf::gHHM = new HHM_OVGraf();
			OV_Graf::CreateGraf(HHM_OVGraf::gHHM);
	     	HHM_OVGraf::gHHM->Preprocessing();
	       	delete AC_Trie::gTrie;
			HHM_OVGraf::gHHM->ProbCalc();
			MainData::PrintMain(&out,0);
			delete HHM_OVGraf::gHHM;
		}   
		if(MainData::order == 0){
			BProb_Graf::gBOVG = new BProb_Graf(0);;
			OV_Graf::CreateGraf(BProb_Graf::gBOVG);
			delete AC_Trie::gTrie;
			BProb_Graf::ProbCalc();
			MainData::PrintMain(&out,0);
			delete BProb_Graf::gBOVG;
			delete[] BProb_Graf::PHCl;
			delete[] BProb_Graf::BSumProb;
			if((MainData::mode == 2)||(MainData::mode == 3)){
				for(i = 0; i < MainData::WordLen; i++){
					delete[] MainData::PssmMas[i];
				}
				delete[] MainData::PssmMas;
				MainData::PssmMas = NULL;
			}
			if((MainData::mode == 4)&(MainData::ConstPositions != NULL)){
				delete[] MainData::ConstPositions;
			}
		}

	    if(MainData::order > 0){
			M_TrTree::gMTr = new M_TrTree();
			M_OVGraf::gMOVG = new M_OVGraf();
			OV_Graf::CreateGraf(M_OVGraf::gMOVG);
	        M_OVGraf::gMOVG->Preprocessing();
			delete AC_Trie::gTrie;
		    M_OVGraf::gMOVG->ProbCalc();
			MainData::PrintMain(&out,0);
	        delete M_OVGraf::gMOVG;
			delete M_TrTree::gMTr;
	    }
		if(MainData::order != 0){
			H_M_OVGraf::DelMas();
		}

	}
	if(MainData::mode == 1){
		delete[] MainData::RandPatProbs;
	}
	if((MainData::order > 0)&&(MainData::CrDistribFlag == 1)){
		for(i = 0; i < MainData::AlpSize; i++){
			delete[] MainData::MarkovProbs[i];
		}
		delete[] MainData::MarkovProbs;
	}
	if(ExitFlag > 0){
		if(ExitFlag ==2 ){
			MainData::PrintMain(&out,0);
		}
		int j;
		if(MainData::MarkovType == 1){
			delete[] MModel_Prob::IniProbs;
		}
		if((MainData::order == -2)&&(MainData::CrDistribFlag == 1)){
			for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
				for(j = 0; j < H_M_OVGraf::NumAllStates; j++){
					delete[] MainData::ND_HHMProbs[i][j];
				}
				delete[] MainData::ND_HHMProbs[i];
				MainData::ND_HHMTrans[i]->clear();
				delete[] MainData::ND_HHMTrans[i];
			}
			delete[] MainData::ND_HHMProbs;
			delete[] MainData::ND_HHMTrans;
		}
		if((MainData::order == -1)&&(MainData::CrDistribFlag == 1)){
			for(i = 0; i < H_M_OVGraf::NumAllStates; i++){	
				delete[] MainData::D_HHMProbs[i];
				delete[] MainData::D_HHMTrans[i];
			}
			delete[] MainData::D_HHMProbs;
			delete[] MainData::D_HHMTrans;
		}
		if(AC_Trie::gTrie != NULL){
			delete AC_Trie::gTrie;
		}
		cout<<Error_line;
		MainData::ErrorDetect(MainData::Error);
		int err = MainData::Error;
		return err;
	}
	
	*pvalue = MainData::Pvalue;

	std::string myreport = out.str();
	if(myreport.length() > 0){
		char* buffer = (char*) calloc(myreport.size()+1, sizeof(char));
		strcpy(buffer, myreport.c_str());
		*report = buffer;
	}
	else {
		*report = NULL;
	}
	
	*ResWords = NULL;
	
	std::string myreswords = outW.str();
	std::vector<std::string> reswords = split(myreswords, '\n');
	*NWords = (int)reswords.size();
	
	if(reswords.size() > 0){
		char ** Words = (char**) calloc(reswords.size(), sizeof(char*));
		int s = (int)reswords.size();
		for (i=0; i<s; i++) {
			char* buffer1 = (char*) calloc(reswords[i].size()+1, sizeof(char));
			strcpy(buffer1, reswords[i].c_str());
			Words[i] = buffer1;
		}
		*ResWords = Words;
	}
	else {
		*ResWords = NULL;
	}
	
	return 0;
}
