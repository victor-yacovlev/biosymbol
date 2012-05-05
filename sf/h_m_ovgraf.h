#pragma once

#ifndef H_M_OVGraf_H
#define H_M_OVGraf_H

#include <vector>
#include <string>

#include "ov_graf.h"

typedef struct PrepSK
{
	int  s; //1, if corresponding alpha beint S(w), 0 otherwise
	int  flag; //is it in SF(w) or not
	int  NPredStates;
	int* PredStates;
}PrepSK;

//Let w be an overlap; q be a state D-associated with w; q’ be a state from Y(q,w);
typedef struct BackPointer
{
	int pos;				 // position of q’ according to the order in H_M_OVGraf[lpred(w)].States;
	double prob;			 //prob = Prob_q(Back(w)|q’)
}BackPointer;

//contains the data related to a overlap w and a state alpha that is D-associated with w.
typedef struct SKVal
{
	int alpha;		//state 
	int flag;					// flag = 1 iff alpha is R-associated to w, else flag = 0;  
	int RParentPos;				//position of the same state of rpred(w) 
	//std::vector<BackPointer> BackProbPointers; 
	BackPointer* BackProbPointers;
	int NumBackPointers;
	double*  FirstTemp;			// If the node corresponds to an equivalence class then for all   
								//p = 1,..,NOccur contains probabilities Probq(R(n, p, H)) 
								//calculated during a current step n; 
								//else the array is empty						
	double** ProbMark;			// If the node is an overlap then for all 
								//p = 1,..,NOccur; k = n-|w|,.., n at the end of step n it 
								//contains probabilities  
								//Prob_q(R(k, p, w)) for a current step n; 
								//else the matrix is empty.
}SKVal;


typedef struct CondProb
{
	int state;	//number of a state	
	double prob;			//Prob_q(H|state)
}CondProb;

// descriptor of a suffix-prefix w or an equivalence class H*  
//Let Q be a set of all states
class H_M_OVGraf :
	public OV_Graf
{
public:
						//1. Parameters
	int NStates;				//number of states D-associated with w
	SKVal* States;						//data for states D-associated with w
	PrepSK* ForSK; //for counting of SFK and SPK
	std::vector<int> GoodStPos;	//for counting of SFK and SPK
	std::vector< std::vector<CondProb> > TermCondProbs;	// (neded for HHM)the values Prob_q(H|q’) for all states q,q’ and equivalence classes
	
	static int NumAllStates;	//number of all states in HHM
	static double*** FarProbs;	// The 3-dimensional matrix of size|HH*|*NOccur*MaxNLeafStates
										// where FarProbs[H*][p][q]= Prob_q(F(n,p,H*)))
										//During the stage n is used to store the values Prob_q(F(n, p, H)) for all 
										//q from EndStates(H*) and p=1,..,NOccur.
	static int** ConsistStMatrix;	//The array of lists. 
											//For every state q from Q it contains list of all states q' such that ï(q',q)>0
	static double** TransProbMatrix;		//The array of lists.
											//for all q from Q and for all states q' consistent with q 
											//TransProbMatrix[q][q']=ï(q',q)
	static double** TransStepProbMatrix;	//(needed only for HHM) the [i,j]th element is ï(q',q) for all states q,q'.
	static double* TransStepProbList;			//(needed only for Markov model) At the step n for all states q it contains probabilities Prob_q(V^(n-m))
	static int* ConsistStNums;				// The array of size NumAllStates
											//ConsistStNums[q] = (number of states consistent with q)
	static int MaxNLeafStates;		//is max_(H* from HH*)(|EndStates(H*)|)
	static double** BnpProbs;			 // The 2-dimensional matrix of size Q*NOccur
										// BnpProbs[q][p]= Prob_q(B(n-m,p)))  
	static double**	TermProbs;			//The array of lists. 
										//TermProbs[H*][q]=Prob_q(H*), for all H* from HH* and q from EndStates(H*)
	static double* PHH;					//array of size NumAllStates. 
										//PHH[q]=Prob_q(HH)
	static double** EProbsOne;			//(needed only for Markov model) The 2-dimensional matrix of size|HH*|*MaxNLeafStates
										// where FarProbs[H*][q]= Prob_q(E(n,1,H*)))
										//During the stage n is used to store the values Prob_q(E(n, 1, H)) for all 
										//q from EndStates(H*).
	static H_M_OVGraf* gHOVG;			//root of the graph

								//2. Methods
	static int AssociatedCondFlag(int q1,std::string word,int q2);	//1, if there exists path from the state q1 to q2 with the mark "word"
																	// else - 0 
	static int AssociatedFlag(std::string word,int q);				//1,if there exists path from initial state to q with the mark "word"	
	static std::vector<int> CalcAssociatedStates(std::string word);		//calculates the states from EndState(word)
	static double TermCondProb(int q1,std::string word,int q2);		//calculates Prob_q1(word|q2)
	static double TermProb(std::string word, int q);				//calculates Prob_q(word)
	static double TransitionProb(int q1, int q2);					//calculates probability of transition from q1 to q2
	static  void TransMatrixProduct(int n);
	virtual void CrConsistStatesMatrix(void);
	virtual void ModelInit(void);									//sets main parameters for the model
	void ClearMas(void);				
	virtual void SetStates(void);									//sets states D-associated with w
	void BackProb(void);											//calculates Prob_q(Back(w))
	void IniProbs(std::string word);								//initialization of main arrays in the node					
	void Stepm(void);												//calculates initial probabilities
	virtual void FarPartProbs(void);								//calculates probabilities of the far sets (see statement 3.3.1 in [1])
																	//The procedure is different for HHM and Markov model of order K
	void ClosePartProbs(int n);					//calculates probabilities of the close sets (see statement 4.4.1 in [1])
	void ROGUpdate(int n);											//By bottom-up traversal of  ROG it recalculates probabilities in ProbMark
	void CalcBnp(int n);											// For all states q and p=1,..,NOccur calculates Probq(B(n, p)) (see statement 3.1.2 in [1]).

public:
	void Preprocessing(void);										//premilinary steps
	static void IniStaticMas(void);
	void ProbCalc(void);											//main algorithm
	static std::vector<int> ConsistStates(int q);						//gets all states consistent with q	
	static void DelMas(void);
	H_M_OVGraf(void);
	H_M_OVGraf(int num1);
	~H_M_OVGraf(void);
};

#endif /* H_M_Graf_H */
