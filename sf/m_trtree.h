#pragma once

#include <vector>
#include <string>
#include "maindata.h"

//Descriptor of a word w in Alp*
//A node corresponds to a word in Alpk (k=1,..,K) or a subset of words from  an equivalence class having a same prefix of length K.
class M_TrTree
{
public:
	int iEnd;					// If it is a leaf then iEnd is  the number of an equivalence class
							//  Else iEnd = 0;
	char sign;					// mark on the edge leading to the node
	int num;
	int len;					// |w|
	std::vector<M_TrTree*> Childs;	// links to the childes
	M_TrTree* Parent;			// links to the parent
	static int* ForLeaves; //to create the tree
	static M_TrTree* gMTr;	//root of the tree
	static int NumTrNodes;
	static M_TrTree** Prefixes; //contains all prefixes of words from HH of length order	
	static int NPrefixes;		//number of all prefixes of words from HH of length order	
	std::vector<double> Probs;	//if w is a leaf  then Probs contains Prob_q(w[K+1,|w|])
	std::vector<int> States;		// set of states associated with w
	std::vector<int> Positions;
private:
	void PrepLeaves(void);	//constructs leaves
	void SetStates(std::string word);	//sets states associated with w
	void CalChilds(void);
	void CalcPrefixes(void);
public:
	M_TrTree(void);
	~M_TrTree(void);
	static void Preprocessing(void);		//preprocessing
	void DebPrint(ofstream* dbf, std::string word);
	void FarPartProbs(double* MasPrProbs);	//calculates probabilities of the far sets
	static void CalEProbOne(void);
};

