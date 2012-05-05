#pragma once

#include <string>
#include <vector>

class MModel_Prob
{
public:
	static int Power; //Alp^order
	static double* IniProbs; //initial probabilities distribution on the states
public:
	static int NumPower(int num, int step);
	static int Code(std::string alpha);
	static std::string deCode(int k, int cd);
	static std::string Prefix(int k, std::string word);
	static int PrefixN(int len, std::string word);
	static std::string Suffix(int k, std::string word);
	static int SuffixN(int len, std::string word);
	static int AssociatedCondFlag(int q1,std::string word,int q2);
	static int AssociatedFlag(std::string word, int q);
	static std::vector<int> CalcAssociatedStates(std::string word);
	static double TermCondProb(int q1, std::string word, int q2);
	static double TermProb(std::string word, int q);
	static std::vector<int> ConsistStates(int q);
	static void SetIniProbs(void);
	static void	TransStepProbCalc(double* &Mass);
	static void CrTrMatrix(void);

	MModel_Prob(void);
	~MModel_Prob(void);
};
