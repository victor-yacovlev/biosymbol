#include "h_m_ovgraf.h"
#include "hhm_ovgraf.h"
#include "mmodel_prob.h"
#include "m_trtree.h"
#include "m_ovgraf.h"
#include "nd_hhm_prob.h"
#include "d_hhm_prob.h"
#include "maindata.h"



int H_M_OVGraf::NumAllStates = 0;
int H_M_OVGraf::MaxNLeafStates = 0;
double*** H_M_OVGraf::FarProbs = NULL;
double** H_M_OVGraf::BnpProbs = NULL;
double** H_M_OVGraf::TermProbs = NULL;
double* H_M_OVGraf::PHH = NULL;
int** H_M_OVGraf::ConsistStMatrix = NULL;
double** H_M_OVGraf::TransProbMatrix = NULL;
int* H_M_OVGraf::ConsistStNums = NULL;
double** H_M_OVGraf::TransStepProbMatrix = NULL;
double* H_M_OVGraf::TransStepProbList = NULL;
double** H_M_OVGraf::EProbsOne = NULL;

H_M_OVGraf* H_M_OVGraf::gHOVG = NULL; 

double** PsiH;
double** Psi2;

H_M_OVGraf::H_M_OVGraf(void)
{
	NStates = 0;
}


H_M_OVGraf::H_M_OVGraf(int num1)
:OV_Graf(num1)
{
	NStates = 0;
}


H_M_OVGraf::~H_M_OVGraf(void)
{
	if(NStates != 0){
		int i, j;
		for(i = 0; i < NStates; i++){
			if(iEnd != 0){
				delete[] States[i].FirstTemp;
			}
			if((iEnd == 0)&&(States[i].flag == 1)){
				for(j = 0; j < MainData::NOccur; j++){	
					delete[] States[i].ProbMark[j];
				}
				delete[] States[i].ProbMark;
				delete[] States[i].FirstTemp;
			}
			delete[] States[i].BackProbPointers;
		}
		delete[] States;
	}
}

//1, if there exists path prom q1 to q2 with the mark "word", else - 0 
int H_M_OVGraf::AssociatedCondFlag(int q1,std::string word,int q2){
	int res;
	if(MainData::order > 0){
		res = MModel_Prob::AssociatedCondFlag(q1,word,q2);
	}
	if(MainData::order == -1){
		res = D_HHM_Prob::AssociatedCondFlag(q1,word,q2);
	}
	if(MainData::order == -2){
		res = ND_HHM_Prob::AssociatedCondFlag(q1,word,q2);
	}
	return res;
}

//1,if there exists path from the initial state to q with the mark "word"
int H_M_OVGraf::AssociatedFlag(std::string word, int q){
	int res;
	if(MainData::order > 0){
		res = MModel_Prob::AssociatedFlag(word,q);
	}
	if(MainData::order == -1){
		res = D_HHM_Prob::AssociatedFlag(word,q);
	}
	if(MainData::order == -2){
		res = ND_HHM_Prob::AssociatedFlag(word,q);
	}
	return res;
}

vector<int> H_M_OVGraf::CalcAssociatedStates(std::string word){
	vector<int> vec;
	if(MainData::order > 0){
		vec = MModel_Prob::CalcAssociatedStates(word);
	}
	if(MainData::order == -1){
		vec = D_HHM_Prob::CalcAssociatedStates(word);
	}
	if(MainData::order == -2){
		vec = ND_HHM_Prob::CalcAssociatedStates(word);
	}
	return vec;

}		

//for a state q gets all states q' such that exisis a symbol 'a' for that Ï(q',a,q)>0
vector<int> H_M_OVGraf::ConsistStates(int q){
	vector<int> vec;
	if(MainData::order > 0){
		vec = MModel_Prob::ConsistStates(q);
	}
	if(MainData::order == -1){
		vec = D_HHM_Prob::ConsistStates(q);
	}
	if(MainData::order == -2){
		vec = ND_HHM_Prob::ConsistStates(q);
	}
	return vec;
}

void H_M_OVGraf::CrConsistStatesMatrix(void){
}


//for a states q1,q2 calculates Prob_q2(word|q1)
double H_M_OVGraf::TermCondProb(int q1, std::string word, int q2){
	double p;
	if(MainData::order > 0){
		p = MModel_Prob::TermCondProb(q1,word,q2);
	}
	if(MainData::order == -1){
		p = D_HHM_Prob::TermCondProb(q1,word,q2);
	}
	if(MainData::order == -2){
		p = ND_HHM_Prob::TermCondProb(q1,word,q2);
	}
	return p;
} 

//calculates Prob_q(word)
double H_M_OVGraf::TermProb(std::string word, int q){
	double p;
	if(MainData::order > 0){
		p = MModel_Prob::TermProb(word,q); 
	}
	if(MainData::order == -1){
		p = D_HHM_Prob::TermProb(word,q);
	}
	if(MainData::order == -2){
		p = ND_HHM_Prob::TermProb(word,q);
	}
	return p;
}


//calculates probability of transition from q1 to q2
double H_M_OVGraf::TransitionProb(int q1, int q2){
	double p;
	if(MainData::order > 0){
		int s = q2/MainData::AlpSize;
		s = q2 - s*MainData::AlpSize;
		p = MainData::MarkovProbs[s][q1];
	}
	if(MainData::order == -1){
		p = D_HHM_Prob::TransitionProb(q1,q2);
	}
	if(MainData::order == -2){
		p = ND_HHM_Prob::TransitionProb(q1,q2);
	}
	return p;
}

void H_M_OVGraf::TransMatrixProduct(int n){
	int i, j;
	if(n == 0){
		for(i = 0; i < NumAllStates; i++){
			for(j = 0; j < ConsistStNums[i]; j++){
				int state = ConsistStMatrix[i][j];
				double p = TransProbMatrix[i][j];
				TransStepProbMatrix[state][i] = p;
			}
		}	
	}
	else{
		double** Matrix = new double*[NumAllStates];
		 for(i = 0; i < NumAllStates; i++){
			Matrix[i] = new double[NumAllStates];
			for(j = 0; j < NumAllStates; j++){
				Matrix[i][j] = 0;
			}
		}
		 int k;
		 for( i =0; i < NumAllStates; i++){
			 for(j = 0; j < ConsistStNums[i]; j++){
				 int state = ConsistStMatrix[i][j];
				 double p = TransProbMatrix[i][j];
				 for(k = 0; k < NumAllStates; k++){
					Matrix[k][i] += TransStepProbMatrix[k][state]*p;
				 }		 
			 
			 }
		 }
		 double** Matrix1 = TransStepProbMatrix; 
		 TransStepProbMatrix = Matrix;
		 
		 for(i = 0; i < NumAllStates; i++){
			delete[] Matrix1[i];
		 }
		 delete[] Matrix1;
	}
	return;
};

//for all ovelaps w and states q D-associated with w calculates Prob_q(Back(w))
void H_M_OVGraf::BackProb(void){
	int i,j;
	int k;
	double p = 0;
	OV_Graf* L1 = LParent;
	H_M_OVGraf* L2 = static_cast<H_M_OVGraf*>(L1);

	if(iEnd != 0){
		if(L2->Lnum != 0){
			for(i = 0; i < NStates; i++){
				int s = States[i].NumBackPointers;
				for(j = 0; j < s; j++){
					int pos = States[i].BackProbPointers[j].pos;
					int q1 = L2->States[pos].alpha;
					int q2 = States[i].alpha;
					p = 0;
					int s1 = (int)Classes[iEnd - 1].Words.size();
					for(k = 0; k < s1; k++){
						if( AssociatedCondFlag(q1,Classes[iEnd - 1].Lsigns[k],q2)==1){
							p = p + TermCondProb(q1,Classes[iEnd - 1].Lsigns[k],q2);
						}
					}
					States[i].BackProbPointers[j].prob = p;
				}
			}
		}
	}	
	else{
		if((Lnum != 0)&&(LParent->Lnum != 0)){
			for(i = 0; i < NStates; i++){
				int s = States[i].NumBackPointers;
				int q = States[i].alpha;
				for(j = 0; j < s; j++){
					int pos = States[i].BackProbPointers[j].pos;
					int q1 = L2->States[pos].alpha;
					p = TermCondProb(q1,Lsign,q);
					States[i].BackProbPointers[j].prob = p;
				}
			}
		}
	}

	for ( k = 0; k < NLChilds; k++){
		OV_Graf* G = LChilds[k];
		H_M_OVGraf* G1 = static_cast<H_M_OVGraf*>(G);
		G1->BackProb();
	}
	
	return;
}

///////////PREPROCESSING/////////////////////////////
//preprocessing
void H_M_OVGraf::Preprocessing(void){
	IniStaticMas();
	ModelInit();
	SetStates();
	IniProbs("");
	if(MainData::order > 0){
		M_TrTree::Preprocessing();
	}
	int i;
	
	BackProb();

	for(i = 0; i < OV_Graf::NClasses; i++){
		Classes[i].Lsigns.clear();
		Classes[i].States.clear();
		Classes[i].Words.clear();
	}
	
	delete[] Classes;
	delete[] Signs;

	return;
}

/////////////////////////////////////////////////////////
void H_M_OVGraf::IniStaticMas(void){
	int i;
	int j,k;
	if(MainData::order > 0){
		NumAllStates = MModel_Prob::NumPower(MainData::AlpSize,MainData::order);
		MModel_Prob::Power = NumAllStates;
		MModel_Prob::CrTrMatrix();
		if(MainData::MarkovType == 0){
			MModel_Prob::IniProbs = new double[NumAllStates];
			MModel_Prob::SetIniProbs();
		}
	}

	//----------information about classes-----------
	for(i = 0; i < NClasses; i++){
		int s = (int)Classes[i].Words.size();
		if(s == 1){
			Classes[i].States = CalcAssociatedStates(Classes[i].Words[0]);
		}
		else{
			vector<int> vec1;
		
			int s1;
			for(j = 0; j < s; j++){
				vector<int> vec = CalcAssociatedStates(Classes[i].Words[j]);
				s1 = (int)vec.size();
				for(k = 0; k < s1; k++){
					vec1.push_back(vec[k]);
				}
			}
			vector<int> vec2;
			s = (int)vec1.size();
			for(j = 0; j < s; j++){
				if(vec1[j]>-1){
					int elem = vec1[j];
					vec2.push_back(elem);
					for(k = j+1; k < s; k++){
						if(vec1[k]== elem){
							vec1[k]= -1;
						}
					}
				}
			}	

			s = (int)vec2.size();
			for(j = 0; j < s; j++){
				int min = vec2[j];
				size_t minpos = j;
				for(k = j + 1; k < s; k++){
					if(min > vec2[k]){
						min = vec2[k];
						minpos = k;
					}
				}
				vec2[minpos] = vec2[j];
				vec2[j] = min;
			}	

			for(j = 0; j < s; j++){
				Classes[i].States.push_back(vec2[j]);
			}
		}
	}
	//---------Calculates Prob_q(H*)---------------
	TermProbs = new double*[NClasses];	
	for(i = 0; i < NClasses; i++){
		int s = (int)Classes[i].States.size();
		TermProbs[i] = new double[s];
		for(j = 0; j < s; j++){
			double prob = 0;
			int state = Classes[i].States[j];
			int s1 = (int)Classes[i].Words.size();
			for(k = 0; k < s1; k++){
				std::string word = Classes[i].Words[k];
				if(AssociatedFlag(word,state)){
					prob += TermProb(word,state);
				}		
			}
			TermProbs[i][j] = prob;
		}
	}
	//---------------------------------------
	PHH = new double[NumAllStates];
	for(i = 0; i < NumAllStates; i++){
		PHH[i] = 0;
	}
	for(i = 0; i < OV_Graf::NClasses; i++){
		int s = (int)Classes[i].States.size();
		for(j = 0; j < s; j++){
			int state = Classes[i].States[j];
			PHH[state] += TermProbs[i][j];
		}
	}

//-------------matrix for far probabilities-----------------------

	FarProbs = new double**[NClasses];

	int max = 0;
	for(i = 0; i < NClasses; i++){
		int size = (int)Classes[i].States.size();
		if(size > max){
			max = size;
		}
	}
	MaxNLeafStates = max;

	for(i = 0; i < OV_Graf::NClasses; i++){
		FarProbs[i] = new double*[MainData::NOccur];
		int l;
		for(l = 0; l < MainData::NOccur; l++){
			FarProbs[i][l] = new double[MaxNLeafStates];
			int x;
			for(x = 0; x < MaxNLeafStates; x++){
				FarProbs[i][l][x] = 0;
			}
		}
	}

	if(MainData::MarkovType > 0){
		EProbsOne = new double*[NClasses];
	
		for(i = 0; i < OV_Graf::NClasses; i++){
			EProbsOne[i] = new double[MaxNLeafStates];
			int x;
			for(x = 0; x < MaxNLeafStates; x++){
				EProbsOne[i][x] = 0;
			}
		}

		TransStepProbList = new double[NumAllStates];
		for(i = 0; i < NumAllStates; i++){
			TransStepProbList[i] = MModel_Prob::IniProbs[i];
		}
	}
	 
//----------------------------------------
	BnpProbs = new double*[NumAllStates];
	int l;
	for(i = 0; i < NumAllStates; i++){
		BnpProbs[i] = new double[MainData::NOccur];
		for(l = 0; l < MainData::NOccur; l++){
			BnpProbs[i][l] = 0;
		}
	}
	
	if(MainData::order < 0){
		TransStepProbMatrix = new double*[NumAllStates];
		for(i = 0; i < NumAllStates; i++){
			TransStepProbMatrix[i] = new double[NumAllStates];
			for(l = 0; l < NumAllStates; l++){
				TransStepProbMatrix[i][l] = 0;
			}
		}
	}
	return;
}



/////////////////////////////////////////////
void H_M_OVGraf::ModelInit(void){

}


//sets states associated with w
void H_M_OVGraf::SetStates(void){

}

void H_M_OVGraf::IniProbs(std::string word){
	int k,i,j,m;
	word += Lsign;
	if(iEnd == 0){
		for(i = 0; i < NStates; i++){
			if(States[i].flag == 1){
				States[i].ProbMark = new double*[MainData::NOccur];
				States[i].FirstTemp = new double[MainData::NOccur];
				m = MainData::WordLen - len;  
	
				for(j = 0; j < MainData::NOccur; j++){	
					States[i].ProbMark[j] = new double[m];
				}

				for(j = 0; j < MainData::NOccur; j++){
					for(k = 0; k < m; k++){
						States[i].ProbMark[j][k] = 0;
					}
					States[i].FirstTemp[j] = 0; 
				}
			}
			else{
				States[i].ProbMark = NULL;
			}
		}
	}
	else{
		for(i = 0; i < NStates; i++){
			States[i].FirstTemp = new double[MainData::NOccur]; 
			States[i].ProbMark = NULL;
		}
	}

	if(iEnd == 0){
		int l;
		for ( l = 0; l < NLChilds; l++){
			OV_Graf* G = LChilds[l];
			H_M_OVGraf* G1 = static_cast<H_M_OVGraf*>(G);
			G1->IniProbs(word);
		}
	}
	return;
}


///////////////////////////////////////////////////

void H_M_OVGraf::ClearMas(void){
	int i,j,k;
	for(i = 0; i < OV_Graf::NClasses; i++){
		for(j = 0; j < MainData::NOccur; j++){
			for(k = 0; k < MaxNLeafStates; k++){
				FarProbs[i][j][k] = 0;
			}
		}
	}
	if(MainData::MarkovType > 0){
		for(i = 0; i < OV_Graf::NClasses; i++){
			for(j = 0; j < MaxNLeafStates; j++){
				EProbsOne[i][j] = 0;
			}
		}
	}
	return;
}

void H_M_OVGraf::DelMas(void){
	int i;
	int j;
	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		delete[] H_M_OVGraf::BnpProbs[i];
	}
	for(i = 0; i < H_M_OVGraf::NClasses; i++){
		delete[] H_M_OVGraf::TermProbs[i];
		for(j = 0; j < MainData::NOccur; j++){
			delete[] H_M_OVGraf::FarProbs[i][j];
		}
		delete[] H_M_OVGraf::FarProbs[i];
	}
	delete[] H_M_OVGraf::BnpProbs;
	delete[] H_M_OVGraf::FarProbs;
	delete[] H_M_OVGraf::TermProbs;

	for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
		delete[] H_M_OVGraf::ConsistStMatrix[i];
		delete[] H_M_OVGraf::TransProbMatrix[i];
	}
	delete[] H_M_OVGraf::ConsistStMatrix;
	delete[] H_M_OVGraf::TransProbMatrix;

	if(MainData::order > 0){
		delete[] MModel_Prob::IniProbs;
		if(MainData::MarkovType > 0){
			delete[] M_TrTree::Prefixes;
			delete[] H_M_OVGraf::TransStepProbList;
			for(i = 0; i < H_M_OVGraf::NClasses; i++){
				delete[] H_M_OVGraf::EProbsOne[i];
			}
			delete[] H_M_OVGraf::EProbsOne;
		}
	}
	if(MainData::order == -2){
		for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
			for(j = 0; j < H_M_OVGraf::NumAllStates; j++){
				delete[] MainData::ND_HHMProbs[i][j];
			}
			delete[] MainData::ND_HHMProbs[i];
			MainData::ND_HHMTrans[i]->clear();
			delete[] MainData::ND_HHMTrans[i];
			delete[] H_M_OVGraf::TransStepProbMatrix[i];
		}
		delete[] MainData::ND_HHMProbs;
		delete[] MainData::ND_HHMTrans;
		delete[] H_M_OVGraf::ConsistStNums;
		delete[] H_M_OVGraf::TransStepProbMatrix;
	}
	if(MainData::order == -1){
		for(i = 0; i < H_M_OVGraf::NumAllStates; i++){	
			delete[] MainData::D_HHMProbs[i];
			delete[] MainData::D_HHMTrans[i];
			delete[] H_M_OVGraf::TransStepProbMatrix[i];
		}
		delete[] MainData::D_HHMProbs;
		delete[] MainData::D_HHMTrans;
		delete[] H_M_OVGraf::ConsistStNums;
		delete[] H_M_OVGraf::TransStepProbMatrix;
	}
	if((MainData::mode==2)||(MainData::mode == 3)){
		for(i = 0; i < MainData::WordLen; i++){
			delete[] MainData::PssmMas[i];
		}
		delete[] MainData::PssmMas;
		MainData::PssmMas = NULL;
	}
	if(MainData::mode == 4){
		delete[] MainData::ConstPositions;
	}
	return;
}

////////////////////////////////////////////


//calculates initial probabilities
void H_M_OVGraf::Stepm(void){
	int i;
	double p = 0;
	if( iEnd == 0){
		int l;
		for( l = 0; l < NRChilds; l++){
			OV_Graf* G = RChilds[l];
			H_M_OVGraf* G1 = static_cast<H_M_OVGraf*>(G);
			G1->Stepm();
		}
	}

	if(Lnum != 0){
		for(i = 0; i < NStates; i++){
			if(States[i].flag == 1){
				if(iEnd != 0){		
					p = TermProbs[iEnd - 1][i];
					States[i].FirstTemp[0] = p;
				}
				else{
					p = States[i].ProbMark[0][MainData::WordLen - len - 1];		
				}
				int pos = States[i].RParentPos;	
				if(pos != -1){
					OV_Graf* R1 = RParent;
					H_M_OVGraf* R2 = static_cast<H_M_OVGraf*>(R1);	
					R2->States[pos].ProbMark[0][MainData::WordLen - R2->len - 1] +=p;
				}
			}
		}
	}
	return;
}

//calculate probabilities of the far sets
void H_M_OVGraf::FarPartProbs(void){

}

//By bottom-up traversal of  ROG it recalculates probabilities in ProbMark
void H_M_OVGraf::ROGUpdate(int n){
	int i, j;
	double p = 0;
	int pos;
	if( iEnd == 0){
		pos = n % (MainData::WordLen - len);
		i = 0;
		while((i < NRChilds)&&(RChilds[i]->iEnd == 0)){
			H_M_OVGraf* G1 = static_cast<H_M_OVGraf*>(RChilds[i]);
			G1->ROGUpdate(n);	
			i++;
		}
		for(i = 0; i < NStates; i++){
			if(States[i].flag == 1){
				for(j = 0; j < MainData::NOccur; j++){
					States[i].ProbMark[j][pos] = States[i].FirstTemp[j];
				} 
			}
		}

		if(Lnum != 0){
			OV_Graf* R1 = RParent;
			H_M_OVGraf* R2 = static_cast<H_M_OVGraf*>(R1);	
			for(j = 0; j < MainData::NOccur; j++){
				for(i = 0; i < NStates; i++){
					if(States[i].flag == 1){
						int state = States[i].RParentPos;	
						if(state != -1){
							R2->States[state].FirstTemp[j] += States[i].FirstTemp[j];
							States[i].FirstTemp[j] = 0;
						}
					}
				}
			}
		}
		if(Lnum == 0){
			for(i = 0; i < NStates; i++){
				for(j = 0; j < MainData::NOccur; j++){
					States[i].FirstTemp[j] = 0;
				}
			}
		}
	}
	return;
}



//calculates probabilities of the close sets (see statement 4.4.1 in [1])


void H_M_OVGraf::ClosePartProbs(int n){
	int i,j, k;
	if(iEnd != 0){		
		H_M_OVGraf* R = static_cast<H_M_OVGraf*>(RParent);
		int pos = n % (MainData::WordLen - R->len);
		for(j = 0; j < MainData::NOccur; j++){
			for(i = 0; i < NStates; i++){
				double Clprob = 0;
				if(LParent->Lnum != 0){
					for(k = 0; k < States[i].NumBackPointers; k++){
						int pos1 = States[i].BackProbPointers[k].pos;
						double p = States[i].BackProbPointers[k].prob;
						if(j == 0){
							Clprob = Clprob - PsiH[pos1][0]*p;
						} 
						else{
							Clprob = Clprob+(PsiH[pos1][j-1] - PsiH[pos1][j])*p;
						}
					}
				}
				if(j == 0){
					if(MainData::order > 0){
						if(MainData::MarkovType == 0){
							Clprob += TermProbs[iEnd - 1][i] - FarProbs[iEnd - 1][0][i];
						}
						else{
							Clprob += EProbsOne[iEnd - 1][i] - FarProbs[iEnd - 1][0][i];
						}
					}
					if(MainData::order < 0){
						int x;
						int s = (int)TermCondProbs[i].size();
						double sum = 0;
						for(x = 0; x < s; x++){
							int state = TermCondProbs[i][x].state;
							sum += TransStepProbMatrix[0][state]*TermCondProbs[i][x].prob;
						}
					
						Clprob += sum - FarProbs[iEnd - 1][0][i];
					}
				}
				else{
					Clprob += FarProbs[iEnd - 1][j-1][i] - FarProbs[iEnd - 1][j][i];
				}

				int st = States[i].RParentPos;
				R->States[st].FirstTemp[j] += Clprob;
				States[i].FirstTemp[j] = Clprob;
			}
		}
	}
	else{
		
		if(Lnum != 0){
			
			int pos = n % (MainData::WordLen - len);
		

			if(LParent->Lnum == 0){
				for(i = 0; i < NStates; i++){
					for(j = 0; j < MainData::NOccur; j++){
						Psi2[i][j] = States[i].ProbMark[j][pos];
					}
				}
			}
			else{
				for(j = 0; j < MainData::NOccur; j++){
					for(i = 0; i < NStates; i++){
						double Sum = 0;
						for(k = 0; k < States[i].NumBackPointers; k++){
							int state = States[i].BackProbPointers[k].pos;
							double p = States[i].BackProbPointers[k].prob;
							Sum = Sum + PsiH[state][j]*p;
						}
						Psi2[i][j] = Sum;
						if(States[i].flag == 1){
							Psi2[i][j] += States[i].ProbMark[j][pos];
						}
					}
				}
			}
		}

		if((MainData::NOccur < 20)&&(NumAllStates < 300)){
			double Psi1[300][20];
			for(i = 0; i < NStates; i++){
				for(j = 0; j < MainData::NOccur; j++){
					Psi1[i][j] = PsiH[i][j];
					PsiH[i][j] = Psi2[i][j];
				}
			}
			for ( i = 0; i < NLChilds; i++){
				OV_Graf* G = LChilds[i];
				H_M_OVGraf* G1 = static_cast<H_M_OVGraf*>(G);
				G1->ClosePartProbs(n);
			}	
			for(i = 0; i < NStates; i++){
				for(j = 0; j < MainData::NOccur; j++){
					PsiH[i][j] = Psi1[i][j];
				}
			}
		}
		else{
			double* Psi1 = new double[NumAllStates*MainData::NOccur];
			for(i = 0; i < NStates; i++){
				for(j = 0; j < MainData::NOccur; j++){
					Psi1[i*MainData::NOccur + j] = PsiH[i][j];
					PsiH[i][j] = Psi2[i][j];
				}
			}
			for ( i = 0; i < NLChilds; i++){
				OV_Graf* G = LChilds[i];
				H_M_OVGraf* G1 = static_cast<H_M_OVGraf*>(G);
				G1->ClosePartProbs(n);
			}	
			for(i = 0; i < NStates; i++){
				for(j = 0; j < MainData::NOccur; j++){
					PsiH[i][j] = Psi1[i*MainData::NOccur + j];
				}
			}
			delete[] Psi1;
		}
	}
	return;
}


// For all states q and p=1,..,NOccur calculates Probq(B(n, p)) (see statement 3.1.2 in [1]).

void H_M_OVGraf::CalcBnp(int n){
	int j;
	int i,s;

	if(n == MainData::WordLen - 1){
		for(i = 0; i < NumAllStates; i++){
			BnpProbs[i][0] = PHH[i];
		}
		delete[] PHH;
	}
	if(n > MainData::WordLen - 1){
		if((MainData::NOccur <20)&&(NumAllStates < 300)){
			int ocr = MainData::NOccur;
			double Mas[300][20];
			double sum;
			
			s = MainData::AlpSize;
			for(i = 0; i < NumAllStates; i++){
				if(MainData::order < 0){
					s = ConsistStNums[i];
				}

				for(j = 0; j < MainData::NOccur; j++){
					sum = 0;
					int k;
					for(k = 0; k < s; k++){
						int state = ConsistStMatrix[i][k];
						sum += BnpProbs[state][j]*TransProbMatrix[i][k];  
					}
				
					Mas[i][j]= sum;
				}
			}

			int pos = n % MainData::WordLen;
			for(i = 0; i < NStates; i++){
				if(States[i].flag == 1){
					int cd = States[i].alpha;
					for( j= 0; j < MainData::NOccur; j++){
						Mas[cd][j] += States[i].ProbMark[j][pos];
					}
				}
			}
	
		
			for(i = 0; i < NumAllStates; i++){
				for(j = 0; j < MainData::NOccur; j++){
					BnpProbs[i][j] = Mas[i][j];
				}
			}
		}
		else{
			
			int ocr = MainData::NOccur;
			double* Mas = new double[NumAllStates*ocr];
			double sum;
			
			s = MainData::AlpSize;
			for(i = 0; i < NumAllStates; i++){
				if(MainData::order < 0){
					s = ConsistStNums[i];
				}

				for(j = 0; j < MainData::NOccur; j++){
					sum = 0;
					int k;
					for(k = 0; k < s; k++){
						int state = ConsistStMatrix[i][k];
						sum += BnpProbs[state][j]*TransProbMatrix[i][k];  
					}
				
					Mas[i*ocr + j]= sum;
				}
			}

			int pos = n % MainData::WordLen;
			for(i = 0; i < NStates; i++){
				if(States[i].flag == 1){
					int cd = States[i].alpha;
					for( j= 0; j < MainData::NOccur; j++){
						Mas[cd*ocr + j] += States[i].ProbMark[j][pos];
					}
				}
			}
	
		
			for(i = 0; i < NumAllStates; i++){
				for(j = 0; j < MainData::NOccur; j++){
					BnpProbs[i][j] = Mas[i*ocr +j];
				}
			}
		
			delete[] Mas;
		
		}
	}
	return;
}
//main algorithm 
void H_M_OVGraf::ProbCalc(void){
	int i,j;
	PsiH  = new double*[NumAllStates];
	Psi2 = new double*[NumAllStates];
	for(i = 0; i < NumAllStates; i++){
		PsiH[i] =new double[MainData::NOccur];
		Psi2[i] = new double[MainData::NOccur];
	}
	for(i = 0; i < NumAllStates; i++){
		for(j = 0; j < MainData::NOccur; j++){
			PsiH[i][j] = 0;
			Psi2[i][j] = 0;
		}
	}


	if(MainData::TLen > MainData::WordLen-1){
		Stepm();
	}
	if(MainData::TLen == MainData::WordLen){
		for(i = 0; i < NumAllStates; i++){
			MainData::Pvalue += PHH[i];
		}
		delete[] PHH;
	}
	if(MainData::TLen > MainData::WordLen){
		int n = 0;
		
		for(i = MainData::WordLen+1; i<= MainData::TLen; i++){
			if(n >= MainData::WordLen - 1){
				CalcBnp(n);
			}
			if(MainData::order < 0){
				TransMatrixProduct(n);
			}
			FarPartProbs();
			if(MainData::MarkovType > 0){
				M_TrTree::CalEProbOne();
			}
			ClosePartProbs(n);
			ROGUpdate(n);
			ClearMas();
			n++;
		}
		for(i = n; i < MainData::TLen; i++){
			CalcBnp(i);
		}
		double p = 0;
		for(i = 0; i < NumAllStates; i++){
			p += BnpProbs[i][MainData::NOccur - 1];
		}
		MainData::Pvalue = p;
	}
	for(i = 0; i < NumAllStates; i++){
		delete[] PsiH[i];
		delete[] Psi2[i];
	}
	delete[] PsiH;
	delete[] Psi2;
	return;
}


