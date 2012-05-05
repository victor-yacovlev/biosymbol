#include "hhm_ovgraf.h"
#include "d_hhm_prob.h"
#include "nd_hhm_prob.h"
#include "maindata.h"

HHM_OVGraf* HHM_OVGraf::gHHM = NULL;

HHM_OVGraf::HHM_OVGraf(void)
{
}

HHM_OVGraf::HHM_OVGraf(int num1)
:H_M_OVGraf(num1)
{
}

HHM_OVGraf::~HHM_OVGraf(void)
{
}

using std::vector;

////////////////////////////////////////////////////
void HHM_OVGraf::SetSFK(void){
	int i;
	ForSK = new PrepSK[NumAllStates];
	int k;
	for(k = 0; k < NumAllStates; k++){
		ForSK[k].flag = 0;
		ForSK[k].s = 0;
		ForSK[k].NPredStates = 0;
		ForSK[k].PredStates = NULL;
	}
	if(iEnd != 0){
		int s = (int)Classes[iEnd - 1].States.size();
		for(i = 0; i <s; i++){
			int state = Classes[iEnd - 1].States[i];
			ForSK[state].s = 1;
			ForSK[state].flag = 1;
			GoodStPos.push_back(state);
			NStates ++;
		}
	}
	else{
		for(i = 0; i < NRChilds; i++){
			OV_Graf* G1 = RChilds[i];
			HHM_OVGraf* G2 = static_cast<HHM_OVGraf*>(G1);
			G2->SetSFK();
			size_t j;
			for(j = 0; j < G2->GoodStPos.size(); j++){
				int pos = G2->GoodStPos[j];
				if((G2->ForSK[pos].s == 1)&&(ForSK[pos].s != 1)){
					ForSK[pos].s = 1;
					ForSK[pos].flag = 1;
					GoodStPos.push_back(pos);
					NStates ++;
				}
			}
		}
	}
	return;
}



void HHM_OVGraf::SetSK(void){
	int i;
	int j;
	int k;
	int num = 0;
	BackPointer BackPoint;
	OV_Graf* L1 = LParent;
	HHM_OVGraf* L2 = static_cast<HHM_OVGraf*>(L1);

	if((Lnum != 0)&&(L2->Lnum != 0)){
		for(i = 0; i < NumAllStates; i++){
			for(j = 0; j < L2->NStates; j++){
				int state = L2->States[j].alpha;
				if(iEnd == 0){
					if(AssociatedCondFlag(state,Lsign,i)){
						if(ForSK[i].s == 0){
							ForSK[i].s = 1;
							GoodStPos.push_back(i);
						}
						if(ForSK[i].NPredStates == 0){
							ForSK[i].PredStates = new int[NumAllStates]; 
						}
						ForSK[i].PredStates[ForSK[i].NPredStates] = j;
						ForSK[i].NPredStates ++;
					}
				}
				else{
					int s = (int)Classes[iEnd -1].Lsigns.size();
					for(k = 0; k < s; k++){
						if(AssociatedCondFlag(state,Classes[iEnd -1].Lsigns[k],i)){
							if(ForSK[i].s == 0){
								ForSK[i].s = 1;
								GoodStPos.push_back(i);
							}
							if(ForSK[i].NPredStates == 0){
								ForSK[i].PredStates = new int[NumAllStates]; 
							}
							ForSK[i].PredStates[ForSK[i].NPredStates]=j;
							ForSK[i].NPredStates ++;
							
							break;
						};
					}
				}
			}	
		}
	}

	States = new SKVal[NStates];
	k = 0;
	for(i = 0; i < NStates; i++){
		int pos = GoodStPos[i];
		if(ForSK[pos].s == 1){
			SKVal Val;
			Val.alpha = pos;
			Val.flag = ForSK[pos].flag;
			Val.RParentPos = -1;
			Val.NumBackPointers = ForSK[pos].NPredStates;
			Val.BackProbPointers = new BackPointer[Val.NumBackPointers];
			if((Lnum != 0)&&(L2->Lnum != 0)){
				BackPoint.prob = 0;
				for(j = 0; j < ForSK[pos].NPredStates; j++){
					BackPoint.pos = ForSK[pos].PredStates[j];
					Val.BackProbPointers[j] = BackPoint;
				}
			}
			States[k] = Val;
			k++;
		}
	}
	if(iEnd == 0){
		for(k = 0; k < NLChilds; k++){
			OV_Graf* G1 = LChilds[k];
			HHM_OVGraf* G2 = static_cast<HHM_OVGraf*>(G1);
			G2->SetSK();
		}
	}
	for(i = 0; i < NumAllStates; i++){
		if((ForSK[i].s == 1)&&(ForSK[i].NPredStates != 0)){
			delete[] ForSK[i].PredStates;
		}
	}
	delete [] ForSK;
	GoodStPos.clear();
	return;
}


void HHM_OVGraf::SameRPState(void){
	int i,j, k;

	if(iEnd == 0){
		int l;
		for(l = 0; l < NRChilds; l++){
			OV_Graf* G1 = RChilds[l];
			HHM_OVGraf* G2 = static_cast<HHM_OVGraf*>(G1);
			G2->SameRPState();
		}
	}

	if(Lnum != 0){
		OV_Graf* R1 = RParent;
		HHM_OVGraf* R2 = static_cast<HHM_OVGraf*>(R1);
		k = 0;
		for(i = 0; i < NStates; i++){
			if(iEnd != 0){
				States[i].flag = 1;
			}
			if(States[i].flag == 1){
				for(j = k; j < R2->NStates; j++){
					if(States[i].alpha == R2->States[j].alpha){
						States[i].RParentPos = j;
						R2->States[j].flag = 1;
						k = j;
						break;
					}	
				}
			}
		}
	}
	return;
}


void HHM_OVGraf::Calc_TermCondProbs(void){
	int i,j;
	if(iEnd != 0){
		for(i = 0; i < NStates; i++){
			vector<CondProb> vec;
			int state = States[i].alpha;
			for(j = 0; j < NumAllStates; j++){
				int flag = 0;
				size_t k;
				for(k = 0; k < Classes[iEnd-1].Words.size(); k++){
					std::string word = Classes[iEnd - 1].Words[k];
					double p;
					if(MainData::order == -1){
						p = D_HHM_Prob::TermCondProb(j,word,state);
					}
					if(MainData::order == -2){
						p = ND_HHM_Prob::TermCondProb(j,word,state);
					}
					if(p != 0){
						if(flag == 0){
							CondProb val;
							val.state = j;
							val.prob = p;
							vec.push_back(val);
							flag = 1;
						}	
						else{
							size_t s = vec.size();
							vec[s-1].prob += p;
						}
					}
				}
			}
			TermCondProbs.push_back(vec);
		}	
	}
	if(iEnd == 0){
		int l;
		for(l = 0; l < NLChilds; l++){
			OV_Graf* G1 = LChilds[l];
			HHM_OVGraf* G2 = static_cast<HHM_OVGraf*>(G1);
			G2->Calc_TermCondProbs();
		}
	}
	return;
}

void HHM_OVGraf::SetStates(void){
	gHHM->SetSFK();
	gHHM->SetSK();
	gHHM->SameRPState();
	gHHM->Calc_TermCondProbs();
	return;
}

void HHM_OVGraf::CrConsistStatesMatrix(void){
	ConsistStMatrix = new int*[NumAllStates];
	TransProbMatrix = new double*[NumAllStates];
	ConsistStNums = new int[NumAllStates];
	int i,j;
	for(i = 0; i < NumAllStates; i++){
		vector<int> vec = ConsistStates(i);
		int s= (int)vec.size();
		ConsistStNums[i] = s;
		ConsistStMatrix[i] = new int[s];
		TransProbMatrix[i] = new double[s];
		for(j = 0; j < s; j++){
			ConsistStMatrix[i][j] = vec[j];
			TransProbMatrix[i][j]= TransitionProb(vec[j],i);
		}
	}
};


void HHM_OVGraf::ModelInit(void){
	CrConsistStatesMatrix();
	return;
}

///////////////////////////////////////////////

/////////////////////////////////////////////////

void HHM_OVGraf::FarPartProbs(void){
	int i;
	if(iEnd != 0){
		for(i = 0; i < MainData::NOccur; i++){
			int j;
			size_t k;
			for(j = 0; j < NStates; j++){
				for(k = 0; k < TermCondProbs[j].size(); k++){
					int state = TermCondProbs[j][k].state;
					FarProbs[iEnd -1][i][j] += BnpProbs[state][i]*TermCondProbs[j][k].prob;
				}
			}
		}
	}
	if(iEnd == 0){
		int l;
		for(l = 0; l < NLChilds; l++){
			OV_Graf* G1 = LChilds[l];
			HHM_OVGraf* G2 = static_cast<HHM_OVGraf*>(G1);
			G2->FarPartProbs();
		}
	}
	return;
}


////////////////////////////////////////////////////


void HHM_OVGraf::DebPrint(ofstream* ForDeb1, std::string word, int ind, int step){
	int i,j,k;
	std::string word1 = word;
	word += Lsign;
		
	if(Lnum == 0){
		*(ForDeb1)<<"Step: "<<step<<'\n';
		*(ForDeb1)<<"--------"<<'\n'<<"--------"<<"\n\n";
		if(ind == 1){
			for(i = 0; i < OV_Graf::NClasses; i++){
				*(ForDeb1)<<"Class "<<i<<'\n';
				size_t l;
				for(l = 0; l < Classes[i].Words.size(); l++){
					*(ForDeb1)<<Classes[i].Words[l]<<'\n'; //<<", "<<Classes[i].Lsigns[j]<<'\n';
				}
			}
			*(ForDeb1)<<'\n';
		}
		for(i = 0; i < MainData::NOccur; i++){
			*(ForDeb1)<<"Occur "<<i+2<<'\n';
			for(j = 0; j < OV_Graf::NClasses; j++){
				for(k = 0; k < H_M_OVGraf::MaxNLeafStates; k++){
					*(ForDeb1)<<H_M_OVGraf::FarProbs[j][i][k]<<"  ";
				}
				*(ForDeb1)<<'\n';
			}
			*(ForDeb1)<<'\n';
		}
		*(ForDeb1)<<"Bnp"<<'\n';
		for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
			for(j = 0; j < MainData::NOccur; j++){
				*(ForDeb1)<<H_M_OVGraf::BnpProbs[i][j]<<"  ";
			}
			*(ForDeb1)<<'\n';
		}
		*(ForDeb1)<<'\n';
	
		*(ForDeb1)<<"\n\n";
	}

	//if(Lnum != 0){

	if(iEnd != 0){
		*(ForDeb1)<<"Class: "<<iEnd<<'\n';
	}
	*(ForDeb1)<<"Sign: "<<word<<"; Lsign: "<<Lsign<<"; LParent: "<<word1<<'\n';
	for(i = 0; i< NStates; i++){
		*(ForDeb1)<<"State "<<States[i].alpha<<'\n';
		for(j = 0; j < States[i].NumBackPointers; j++){
			*(ForDeb1)<<'('<<States[i].BackProbPointers[j].pos<<','<<States[i].BackProbPointers[j].prob<<')'<<"; ";
		}
		*(ForDeb1)<<'\n';
		
		if(States[i].flag == 1){
			*(ForDeb1)<<"RParent same: "<<States[i].RParentPos<<"\n\n";
		
			for(j = 0; j < MainData::WordLen - len; j++){
				for(k = 0; k < MainData::NOccur; k++){
					*(ForDeb1)<<States[i].ProbMark[k][j]<<' ';
				}
				*(ForDeb1)<<"\n\n";
			}
			if(iEnd!= 0){
				*(ForDeb1)<<"FirstTemp"<<'\n';
				for(j = 0; j < MainData::NOccur; j++){
					*(ForDeb1)<<States[i].FirstTemp[j]<<' ';
				}
			}
			*(ForDeb1)<<"\n\n";
			if(iEnd != 0){
				*(ForDeb1)<<"TermProb: "<<TermProbs[iEnd - 1][i]<<'\n'<<"-----------\n\n";
			}
		}
		*(ForDeb1)<<"\n\n";
/*
		if(iEnd != 0){
			ForDeb1<<"TermCondProbs: \n";
			for(j = 0; j < TermCondProbs[i].size(); j++){
				ForDeb1<<'('<<TermCondProbs[i][j].state<<','<<TermCondProbs[i][j].prob<<");  ";
			}
			ForDeb1<<"\n\n";
		}
		*/
	}

	//ForDeb1<<"\n\n";
	if(iEnd == 0){
		int l;
		for(l = 0; l < NLChilds; l++){
			OV_Graf* G1 = LChilds[l];
			HHM_OVGraf* G2 = static_cast<HHM_OVGraf*>(G1);
			G2->DebPrint(ForDeb1, word, 0,step);
		}
	}
}
