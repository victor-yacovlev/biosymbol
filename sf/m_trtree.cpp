#include "m_trtree.h"
#include "ac_trie.h"
#include "mmodel_prob.h"
#include "h_m_ovgraf.h"
#include "ov_graf.h"

int* M_TrTree::ForLeaves;
M_TrTree* M_TrTree::gMTr;
M_TrTree** M_TrTree::Prefixes = NULL;
int M_TrTree::NPrefixes = 0;

M_TrTree::M_TrTree(void)
{
	iEnd = 0;
	sign = '\0';
	len = 0;
	Parent = NULL;
}

M_TrTree::~M_TrTree(void)
{
	States.clear();
	Probs.clear();
	Positions.clear();
	size_t i;
	if(iEnd == 0){
		for(i = 0; i < Childs.size(); i++){
			if(Childs[i]!= NULL){
				delete Childs[i];
				Childs[i] = NULL;
			}
		}  			
	}
}


void CreateTrTree(M_TrTree* &G, AC_Trie* &T){
	int ind = 0;
	int i;
	if((T->lp != 0)&&(T->lp <= MainData::order)&&(T->iEnd == 0)){
		M_TrTree* v = new M_TrTree;
		v->iEnd = 0;
		v->len = T->lp;
		v->sign = MainData::AlpMas[T->sign];
		v->Parent = G;
		G->Childs.push_back(v);
		ind = 1;
	}
	if(T->iEnd != 0){
		std::string word, pref, suf;
		int cd, cd1;
		M_TrTree* v = new M_TrTree;
		v->iEnd = M_TrTree::ForLeaves[T->iEnd - 1];
		v->len = T->lp;
		v->Parent = G;
		if(v->len == MainData::order){
			v->sign = MainData::AlpMas[T->sign];
		}
		else{	
			T->Path(AC_Trie::gTrie,&word);
			pref = MModel_Prob::Prefix(MainData::order,word);
			cd = MModel_Prob::Code(pref);
			suf = MModel_Prob::Suffix(MainData::order,word);
			cd1 = MModel_Prob::Code(suf);
			word.erase(0,MainData::order);
			double p = MModel_Prob::TermCondProb(cd,word,cd1);
			v->sign = '\0';	
			v->States.push_back(cd1);
			v->Probs.push_back(p);
		}
		G->Childs.push_back(v);
	}
	if(T->iEnd == 0){
		for( i = 0; i < MainData::AlpSize; i++){
			if(T->Childs[i]!=NULL){
				if(ind == 1){
					size_t m = G->Childs.size();
					CreateTrTree(G->Childs[m - 1], T->Childs[i]);
				}
				else{
					CreateTrTree(G,T->Childs[i]);
				}
			}
		}
	}
	return;
}

M_TrTree** Mas;

void M_TrTree::PrepLeaves(void){
	size_t i;
	if((len == MainData::order)&&(iEnd == 0)){
		int k;
		for(k = 0; k < OV_Graf::NClasses; k++){
			Mas[k]= NULL;
		}
		for(i = 0; i < Childs.size(); i++){
			M_TrTree* T = Childs[i];
			if(Mas[T->iEnd - 1] == NULL){
				Mas[T->iEnd - 1] = T;
			}
			else{
				M_TrTree* T1 = Mas[T->iEnd - 1];
				size_t j;
				int flag = 0;
				for(j = 0; j < T1->States.size(); j++){
					if(T1->States[j] == T->States[0]){
						T1->Probs[j] += T->Probs[0];
						flag = 1;
						break;
					}
				}
				if(flag == 0){
					T1->States.push_back(T->States[0]);
					T1->Probs.push_back(T->Probs[0]);
				}
				T->iEnd = -1;
			}
		}
	
		vector<M_TrTree*>::iterator iter;
		vector<M_TrTree*>::iterator iter1;
		iter1 = Childs.begin();
		for(i = 0;  i < Childs.size(); i++){
			iter = iter1;
			if(Childs[i]->iEnd == -1){
				M_TrTree* T = Childs[i];
				iter1--;
				i--;
				Childs.erase(iter);
				delete T;
				T = NULL;
			}
			if(i < Childs.size() -1){
				iter1++;
			}
		}
	}
	if(len < MainData::order){
		for(i = 0; i < Childs.size(); i++){
			Childs[i]->PrepLeaves();
		}
	}
	return;
}


void M_TrTree::SetStates(std::string word){
	size_t i, j;
	int state;
	char* st = &sign;
	st[1]='\0';
	if(len <=MainData::order){
		word += st;
	}
	if((len != 0)&&(len < MainData::order)){
		int d = MainData::order - len;
		int pow = MModel_Prob::NumPower(MainData::AlpSize, d);
		int pow1 = MModel_Prob::Power/pow;
		state = MModel_Prob::Code(word);
		int state1;
		int k;
		for(k = 0; k < pow; k++){
			state1 = state;
			state1 += k*pow1; 
			States.push_back(state1);
		}
	}
	if(len == MainData::order){
		state = MModel_Prob::Code(word);
		States.push_back(state);
	}
	if(iEnd != 0){
		for(i = 0; i < States.size(); i++){
			for(j = 0; j < H_M_OVGraf::Classes[iEnd - 1].States.size(); j++){
				if(States[i] == H_M_OVGraf::Classes[iEnd - 1].States[j]){
					int pos = (int)j;
					Positions.push_back(pos);
					break;
				}
			}
		}
	}
	if(iEnd == 0){
		for(i = 0; i < Childs.size(); i++){
			Childs[i]->SetStates(word);
		}
	}
	return;
}

void M_TrTree::CalcPrefixes(void){
	if(len < MainData::order){
		int i;
		int s = (int)Childs.size();
		for(i = 0; i < s; i++){
			Childs[i]->CalcPrefixes();
		}  			
	}
	if(len == MainData::order){
		Prefixes[NPrefixes] = this;
		NPrefixes ++;
	}
}

void M_TrTree::Preprocessing(void){
	Mas = new M_TrTree*[OV_Graf::NClasses];
	CreateTrTree(gMTr, AC_Trie::gTrie);
	gMTr->PrepLeaves();
	gMTr->SetStates("");
	if(MainData::MarkovType > 0){
		Prefixes = new M_TrTree*[H_M_OVGraf::NumAllStates];
		gMTr->CalcPrefixes();
	}
	
	delete[] Mas;
	delete[] M_TrTree::ForLeaves;
	return;
}



void M_TrTree::CalEProbOne(void){
	int i,j, k;
	MModel_Prob::TransStepProbCalc(H_M_OVGraf::TransStepProbList);
	if(MainData::order != MainData::WordLen){
		for( i = 0; i < NPrefixes; i++){
			int state = Prefixes[i]->States[0];
			int s = (int)Prefixes[i]->Childs.size();
			for(j = 0; j < s; j++){
				M_TrTree* G = Prefixes[i]->Childs[j];
				int s1 = (int)G->States.size();
				for(k = 0; k < s1; k++){
					int pos = G->Positions[k];
					double p = G->Probs[k];
					H_M_OVGraf::EProbsOne[G->iEnd - 1][G->Positions[k]] += H_M_OVGraf::TransStepProbList[state]*G->Probs[k];
					double prob = H_M_OVGraf::EProbsOne[G->iEnd - 1][G->Positions[k]];
				}
			}
		}
	}
	else{
		for( i = 0; i < NPrefixes; i++){
			M_TrTree* G = Prefixes[i];
			int state = G->States[0];
			H_M_OVGraf::EProbsOne[G->iEnd - 1][0] = H_M_OVGraf::TransStepProbList[state];
		}
	}
	return;
}



void M_TrTree::FarPartProbs(double* MasPrProbs){
	double* MaseProbs;
	int i,j, k;
	double p;
	int NStates = (int)States.size();
	int pos = MainData::AToi(sign);
	if(len == 0){
		MaseProbs =NULL;
	}
	if((iEnd == 0)&&(len > 0)){
		MaseProbs = new double[NStates*MainData::NOccur]; 
		for(i = 0; i < NStates; i++){
			for(j = 0; j < MainData::NOccur; j++){
				MaseProbs[i*MainData::NOccur + j] = 0;
			}
		}

		for( i = 0; i < MainData::NOccur; i++){
			for(j = 0; j < NStates; j++){
				p = 0;
				int cd = 0;
				p = 0;
				cd = States[j]/MainData::AlpSize;
				for(k = 0; k < MainData::AlpSize; k++){
					int cd1 = cd + (k*MModel_Prob::Power)/MainData::AlpSize;
					double mp = MainData::MarkovProbs[pos][cd1];
					if(len == 1){
						p += H_M_OVGraf::BnpProbs[cd1][i]*mp;
					}
					else{
						int pos = cd1/MModel_Prob::NumPower(MainData::AlpSize, len -1);
						p += MasPrProbs[pos*MainData::NOccur + i]*mp;
					}
				}
				MaseProbs[j*MainData::NOccur + i] = p;
			}
		}
	}
	if(iEnd != 0){
		if(len > MainData::order){
			for(i = 0; i < MainData::NOccur; i++){
				for(j = 0; j < NStates; j++){
					H_M_OVGraf::FarProbs[iEnd - 1][i][Positions[j]] += MasPrProbs[i]*Probs[j];
				}
			}
		}
		else{
			size_t s = Parent->States.size();
			for(i = 0; i < MainData::NOccur; i++){
				size_t l;
				int cd;
				if(len == 1){
					for(k = 0; k < MainData::AlpSize; k++){
						H_M_OVGraf::FarProbs[iEnd - 1][i][0] +=H_M_OVGraf::BnpProbs[k][i]*MainData::MarkovProbs[pos][k];
					}
				}
				for(l = 0; l < s; l++){
				    cd = Parent->States[l];
					H_M_OVGraf::FarProbs[iEnd - 1][i][0] += MasPrProbs[l*MainData::NOccur+i]*MainData::MarkovProbs[pos][cd];
				}
			}
		}
	}

	if(iEnd == 0){
		int l;
		int s = (int)Childs.size();
		for(l = 0; l < s; l++){
			Childs[l]->FarPartProbs(MaseProbs);
		}
		if(len > 0){
			delete[] MaseProbs;
		}
	}
	return;
}



void M_TrTree::DebPrint(ofstream* dbf, std::string word){
	size_t i;
	if(len <= MainData::order){
		word += sign;
		*(dbf)<<word<<'\n';
	}
	if(iEnd > 0){
		*(dbf)<<"Leaf "<<iEnd<<'\n';
	}
	*(dbf)<<"States: "<<States.size()<< '\n';
	for(i = 0; i < States.size(); i++){
		*(dbf)<<MModel_Prob::deCode(MainData::order, States[i])<<'\n';
		if((iEnd > 0)&&(len > MainData::order)){
			*(dbf)<<"prob: "<<Probs[i]<<'\n';
		}
	}
	*(dbf)<<"\n\n";
	if(iEnd == 0){
		for(i = 0; i < Childs.size(); i++){
			Childs[i]->DebPrint(dbf,word);
		}
	}
}
