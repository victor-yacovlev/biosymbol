#pragma once
#ifndef BProb_Graf_H
#define BProb_Graf_H

#include "ov_graf.h"


class BProb_Graf;

//descriptor of a node w
class BProb_Graf :
	public OV_Graf
{
public:
	double Fi;					//Prob(Back(w))	
	double* FirstTemp;			// If w is an equivalence class then for all   
								//p = 1,..,NOccur contains probabilities Prob(R(n, p, H)) 
								//calculated in a current step n; 
								//else the array is empty.
	double** ProbMark;
								// If w is an overlap then for all 
								//p = 1,..,NOccur; k = n-|w|,..,n contains probabilities  
								//Prob(R(k, p, w)) for a current step n; 
								//else the matrix is empty.

	static double*	PHCl;		//Contains probability P(F) for every overlap class F
	static double*	BSumProb;	
	static BProb_Graf *gBOVG;	//root of the graph
private:
	virtual void BernFi(void);	//It calculates Prob(Back(w)) for all nodes w in OVG
	virtual void IniProbs(void);	// preliminary steps for probability calculation
	virtual void IniFTemp(void);	
	virtual	void IniProbMark(void);
	virtual	void Stepm(void);		//calculates initial probabilities
	virtual void PsiCalc(int n);	//It calculates Prob(R(n,p,HH)) and probabilities of the pattern words
	virtual void ROGUpdate(int n);	//it updates ROG
public:
	BProb_Graf();
	BProb_Graf(int num1);
	virtual ~BProb_Graf();
	static  double BernProbMotif(const std::string& motif);	//calculates probability of a motif
	static  void ProbCalc(void); //main algorithm 
	static void PredStep(void);	//preliminary steps

	void DebPrint2(std::string word, int step);
};



#endif
