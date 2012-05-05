#include "m_ovgraf.h"
#include "mmodel_prob.h"
#include "m_trtree.h"
#include "bprob_graf.h"
#include "maindata.h"



M_OVGraf* M_OVGraf::gMOVG = NULL;

M_OVGraf::M_OVGraf()
{
}


M_OVGraf::M_OVGraf(int num1)
:H_M_OVGraf(num1)
{
}

M_OVGraf::~M_OVGraf()
{
}


///////////Functions to calculate states associated to an overlap w from OV(HH)///////////////////

void M_OVGraf::SetSFK(std::string word){
	word = Signs[Lnum];
	int i;
	if(iEnd != 0){
		NStates = (int)Classes[iEnd - 1].States.size();
		States = new SKVal[NStates];
		for(i = 0; i <NStates; i++){
			int state = Classes[iEnd - 1].States[i];
			States[i].alpha = state;
			States[i].flag = 1;
			States[i].RParentPos = -1;
			States[i].NumBackPointers = 0;
			States[i].BackProbPointers = NULL;
		}
	}
	else{
		int j; 
		
		if(len >= MainData::order){
			States = new SKVal[1];
			NStates = 1;
			States[0].alpha = MModel_Prob::SuffixN(MainData::order,word);
			States[0].flag = 1;
			States[0].RParentPos = -1;
			States[0].NumBackPointers = 0;
			States[0].BackProbPointers = NULL;
		}
		else{
			ForSK = new PrepSK[NumAllStates];
			for(j = 0; j < NumAllStates; j++){
				ForSK[j].flag = 0;
				ForSK[j].s = 0;
				ForSK[j].NPredStates = 0;
				ForSK[j].PredStates = NULL;
			}
		}
		int l;
		for(l = 0; l < NRChilds; l++){
			OV_Graf* G1 = RChilds[l];
			M_OVGraf* G2 = static_cast<M_OVGraf*>(G1);
			G2->SetSFK(word);
			if(len < MainData::order){
				for(j = 0; j < G2->NStates; j++){
					int pos;
					if(G2->len >= MainData::order){
						pos = G2->States[j].alpha;
					}
					else{
						pos = G2->GoodStPos[j];
					}
			
					if(ForSK[pos].s != 1){
						ForSK[pos].s = 1;
						ForSK[pos].flag = 1;
						GoodStPos.push_back(pos);
						NStates++;
					}
				}	
			}
		}
	}
	return;
}


void M_OVGraf::SetSK(void){
	int i,k;
	int j;
	BackPointer BackPoint;
	OV_Graf* L1 = LParent;
	M_OVGraf* L2 = static_cast<M_OVGraf*>(L1);
	BackPoint.prob = 0;
	if(len >= MainData::order){
		if(L2->Lnum > 0){
			for(i = 0; i < NStates; i++){
				States[i].NumBackPointers = L2->NStates;
				States[i].BackProbPointers = new BackPointer[L2->NStates];
				for(j = 0; j < L2->NStates; j++){
					BackPoint.pos = j;
					States[i].BackProbPointers[j] = BackPoint;
				}
			}
		}
	}
	else{
		if((Lnum != 0)&&(L2->Lnum != 0)){
			int cd1 = MModel_Prob::Code(Lsign);
			for(i = 0; i < L2->NStates; i++){
				int size = (int)Lsign.size();
				int pow = MModel_Prob::NumPower(MainData::AlpSize,MainData::order - size); 
				int cd = L2->States[i].alpha/pow;
				cd = L2->States[i].alpha - cd*pow;
				cd = cd*NumAllStates/pow;
				cd += cd1;
				if(ForSK[cd].s == 0){
					ForSK[cd].s = 1;
					NStates++;
					GoodStPos.push_back(cd);
				}
				if(ForSK[cd].NPredStates == 0){
					ForSK[cd].PredStates = new int[NumAllStates]; 
				}
				ForSK[cd].PredStates[ForSK[cd].NPredStates]= i;
				ForSK[cd].NPredStates ++;
			}
		}
		States = new SKVal[NStates];
		k = 0;
		for(i = 0; i < NumAllStates; i++){
			if(ForSK[i].s == 1){
				SKVal Val;
				Val.alpha = i;
				Val.flag = ForSK[i].flag;
				Val.RParentPos = -1;
				Val.NumBackPointers = ForSK[i].NPredStates;
				Val.BackProbPointers = new BackPointer[Val.NumBackPointers];
				if((Lnum != 0)&&(L2->Lnum != 0)){
					for(j = 0; j < ForSK[i].NPredStates; j++){
						BackPoint.pos = ForSK[i].PredStates[j];
						Val.BackProbPointers[j]= BackPoint;
					}
				}
				States[k] = Val;
				k++;
			}
		}
	}
	if(iEnd == 0){
		int l;
		for(l = 0; l < NLChilds; l++){
			OV_Graf* G1 = LChilds[l];
			M_OVGraf* G2 = static_cast<M_OVGraf*>(G1);
			G2->SetSK();
		}
		if(len < MainData::order){
			for(i = 0; i < NumAllStates; i++){
				if((ForSK[i].s == 1)&&(ForSK[i].NPredStates != 0)){
					delete[] ForSK[i].PredStates;
				}
			}
			delete [] ForSK;
			GoodStPos.clear();
		}
	}
	return;
}



void M_OVGraf::SameRPState(void){
	int i,j, k;
	if(Lnum != 0){
		OV_Graf* R1 = RParent;
		M_OVGraf* R2 = static_cast<M_OVGraf*>(R1);
		k = 0;
		for(i = 0; i < NStates; i++){
			for(j = k; j < R2->NStates; j++){
				if(States[i].alpha == R2->States[j].alpha){
					States[i].RParentPos = j;
					k = j;
					break;
				}	
			}
		}
	}
	if(iEnd == 0){
		int l;
		for(l = 0; l < NRChilds; l++){
			OV_Graf* G1 = RChilds[l];
			M_OVGraf* G2 = static_cast<M_OVGraf*>(G1);
			G2->SameRPState();
		}
	}
	return;
}


void M_OVGraf::CrConsistStatesMatrix(void){
	ConsistStMatrix = new int*[NumAllStates];
	TransProbMatrix = new double*[NumAllStates];
	int i;
	for(i = 0; i < NumAllStates; i++){
		ConsistStMatrix[i] = new int[MainData::AlpSize];
		TransProbMatrix[i] = new double[MainData::AlpSize];
		vector<int> vec = ConsistStates(i);
		size_t j;
		for(j = 0; j < vec.size(); j++){
			ConsistStMatrix[i][j] = vec[j];
			double p = TransitionProb(vec[j],i);
			TransProbMatrix[i][j] = p;
		}
	}
};



void M_OVGraf::ModelInit(void){
	MModel_Prob::Power = MModel_Prob::NumPower(MainData::AlpSize,MainData::order);
	CrConsistStatesMatrix();
	
	return;
}

void M_OVGraf::SetStates(void){
	NumAllStates = MModel_Prob::Power;
	std::string word;
	gMOVG->SetSFK(word);
	gMOVG->SetSK();
	gMOVG->SameRPState();
	return;
}

void M_OVGraf::FarPartProbs(void){
	M_TrTree::gMTr->FarPartProbs(NULL);
	return;
}

/////////////////////////////////////////////////////////////////

void M_OVGraf::DebPrint(ofstream* ForDeb, std::string word, int ind, int step){
	int i,j,k;
	std::string word1 = word;
	word += Lsign;
		
	if(Lnum == 0){
		*(ForDeb)<<"Step: "<<step<<'\n';
		*(ForDeb)<<"--------"<<'\n'<<"--------"<<"\n\n";
		if(ind == 1){
			for(i = 0; i < OV_Graf::NClasses; i++){
				*(ForDeb)<<"Class "<<i<<'\n';
				size_t l;
				for(l = 0; l < Classes[i].Words.size(); l++){
					*(ForDeb)<<Classes[i].Words[l]<<'\n'; //<<", "<<Classes[i].Lsigns[j]<<'\n';
				}
			}
			*(ForDeb)<<'\n';
		}
		for(i = 0; i < MainData::NOccur; i++){
			*(ForDeb)<<"Occur "<<i+2<<'\n';
			for(j = 0; j < OV_Graf::NClasses; j++){
				for(k = 0; k < H_M_OVGraf::MaxNLeafStates; k++){
					*(ForDeb)<<H_M_OVGraf::FarProbs[j][i][k]<<"  ";
				}
				*(ForDeb)<<'\n';
			}
			*(ForDeb)<<'\n';
		}
		
		*(ForDeb)<<"Bnp"<<'\n';
		for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
			for(j = 0; j < MainData::NOccur; j++){
				*(ForDeb)<<H_M_OVGraf::BnpProbs[i][j]<<' ';
			}
			*(ForDeb)<<'\n';
		}
		*(ForDeb)<<'\n';
		for(i = 0; i < NumAllStates; i++){
			if(H_M_OVGraf::BnpProbs[i][0]!= 0){
				*(ForDeb)<<MModel_Prob::deCode(MainData::order,i)<<' ';
			}
		}
		
		*(ForDeb)<<"\n\n";
	}
	
	if(iEnd != 0){
		*(ForDeb)<<"Class: "<<iEnd<<'\n';
	}
		*(ForDeb)<<"Sign: "<<word<<"; Lsign: "<<Lsign<<"; LParent: "<<word1<<'\n';
		for(i = 0; i< NStates; i++){
			*(ForDeb)<<"State"<<i<<". "<<MModel_Prob::deCode(MainData::order,States[i].alpha)<<'\n';
			for(j = 0; j < States[i].NumBackPointers; j++){
				*(ForDeb)<<'('<<States[i].BackProbPointers[j].pos<<','<<States[i].BackProbPointers[j].prob<<')'<<"; ";
			}
			*(ForDeb)<<'\n';
			if(States[i].flag == 1){
				*(ForDeb)<<"RParent same: "<<States[i].RParentPos<<"\n\n";
		
				for(j = 0; j < MainData::WordLen - len; j++){
					for(k = 0; k < MainData::NOccur; k++){
						*(ForDeb)<<States[i].ProbMark[k][j]<<' ';
					}
					*(ForDeb)<<"\n\n";
				}
			}
			*(ForDeb)<<"FirstTemp"<<'\n';
			for(j = 0; j < MainData::NOccur; j++){
				*(ForDeb)<<States[i].FirstTemp[j]<<' ';
			}
			*(ForDeb)<<"\n\n";
			if(iEnd != 0){
				*(ForDeb)<<"TermProb: "<<TermProbs[iEnd - 1][i]<<'\n'<<"-----------\n\n";
			}
		}
		*(ForDeb)<<"\n\n";
	
	if(iEnd == 0){
		int l;
		for(l = 0; l < NLChilds; l++){
			OV_Graf* G1 = LChilds[l];
			M_OVGraf* G2 = static_cast<M_OVGraf*>(G1);
			G2->DebPrint(ForDeb,word, 0,step);
		}
	}
}
