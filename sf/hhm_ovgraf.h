#pragma once
#ifndef HHM_OVGraf_H
#define HHM_OVGraf_H

#include <vector>
#include <string>
#include "h_m_ovgraf.h"
#include "maindata.h"


//for a state q and an equivalence class H

//descriptor of an overlap w or an equivalence class
class HHM_OVGraf :
	public H_M_OVGraf
{
public:
	static HHM_OVGraf* gHHM;				//root of the graph
	
public:
	void SetSFK(void);								//sets states from SF(w)
	void SetSK(void);								//sets states from S(w)
	void SameRPState(void);						//for all states associated with w finds number of the same states in RParent
	void Calc_TermCondProbs(void);				//if w is equivalence class that calculates it's terminal probabilities
	virtual void ModelInit(void);				//sets parameters for the model
	virtual void CrConsistStatesMatrix(void);
	virtual void SetStates(void);				//sets states associated with w
	virtual void FarPartProbs(void);			//calculates probabilities for far sets
	void DebPrint(ofstream* ForDeb1, std::string word, int ind, int step);
	HHM_OVGraf();
	HHM_OVGraf(int num1);
	~HHM_OVGraf(void);
};
#endif /* HHM_OVGraf_H */
