#pragma once
#include "h_m_ovgraf.h"
#include "maindata.h"
#include <string>

//Descriptor of a suffix-prefix w or an equivalence class H
//Contains additional methods and parameters 

class M_OVGraf :
	public H_M_OVGraf
{
private:
public:
	static M_OVGraf* gMOVG;	//root of the graph
private:
	void SetSFK(std::string word);		//sets states from SF(w)
	void SetSK(void);		//sets states from S(w)
	void SameRPState(void);	//for all states associated with w finds number of the same states in RParent
public:
	M_OVGraf();
	M_OVGraf(int num1);
	~M_OVGraf();
	virtual void ModelInit(void);		//sets parameters for the model
	virtual void CrConsistStatesMatrix(void);
	virtual void SetStates(void);		//sets states associated with w
	virtual void FarPartProbs(void);	//calculates probabilities of the far sets 
	void DebPrint(ofstream* ForDeb, std::string word,int ind, int step);
};
