//#include "StdAfx.h"
#include "maindata.h"
#include "ac_trie.h"
#include "ov_graf.h"
#include "bprob_graf.h"
#include "m_ovgraf.h"
#include "hhm_ovgraf.h"
#include "m_trtree.h"

int OV_Graf::NumOVNodes = 1;			
int OV_Graf::NClasses   = 0;						
OV_Graf* OV_Graf::gOVG = NULL;
Clazz* OV_Graf::Classes;
std::string* Signs;

OV_Graf::OV_Graf(int num1)
{
	iEnd = 0; //false;
	RParent = NULL;
	LParent = NULL;
	Lnum = num1;
	len = 0;
	ForCopy = NULL;
	NRChilds = 0;
	NLChilds = 0;
	RChilds = NULL;
	LChilds = NULL;
}

OV_Graf::OV_Graf()
{
	iEnd = 0; //false;
	RParent = NULL;
	Lnum = 0;
	len = 0;
	ForCopy = NULL;
	NRChilds = 0;
	NLChilds = 0;
	RChilds = NULL;
	LChilds = NULL;
}


OV_Graf::~OV_Graf()
{
    int i;

	for(i = 0; i < NLChilds; i++){
		if(LChilds[i]!= NULL){
			delete LChilds[i];
			LChilds[i] = NULL;
		}
	}  	
	delete[] LChilds;
	delete[] RChilds;
	return;
}
//////////////////////Copy/////////////////////////////////////////////

void OV_Graf:: Copy(int m, OV_Graf* G){
    int i;

	Lsign = G->Lsign;
	Rsign = G->Rsign;
	iEnd = G->iEnd;
	Lnum = G->Lnum; 
	len   = G->len;
	for( i = 0; i < G->NLChilds; i++){
		if(G->LChilds[i]->ForCopy == NULL){
			if(m == 0){
				OV_Graf* G1 = new OV_Graf();
				G1->Copy(m,G->LChilds[i]);
				G->LChilds[i]->ForCopy = G1;
			}
			if(m == 2){
				if(G->LChilds[i]->iEnd != -1){
					BProb_Graf* G1 = new BProb_Graf();
					G1->Copy(m,G->LChilds[i]);
					G->LChilds[i]->ForCopy = G1;
				}
			}	
			if(m == 3){
				if(G->LChilds[i]->iEnd != -1){
					M_OVGraf* G1 = new M_OVGraf();
					G1->Copy(m,G->LChilds[i]);
					G->LChilds[i]->ForCopy = G1;
				}
			}
			if(m == 4){
				if(G->LChilds[i]->iEnd != -1){
					HHM_OVGraf* G1 = new HHM_OVGraf();
					G1->Copy(m,G->LChilds[i]);
					G->LChilds[i]->ForCopy = G1;
				}
			}
		}
		if(G->LChilds[i]->iEnd != -1){
			//LChilds.push_back(G->LChilds[i]->ForCopy);
			G->LChilds[i]->ForCopy->LParent = this; 
		}
	}
		
	for( i =0; i < G->NRChilds; i++){
		if(G->RChilds[i]->ForCopy ==NULL){
			if(m == 0){
				OV_Graf* G1 = new OV_Graf();
				G1->Copy(m,G->RChilds[i]);
				G->RChilds[i]->ForCopy = G1;
			}
			if(m == 2){
				if(G->RChilds[i]->iEnd != -1){
					BProb_Graf* G1 = new BProb_Graf();
					G1->Copy(m,G->RChilds[i]);
					G->RChilds[i]->ForCopy = G1;
				}
			}
			if(m == 3){
				if(G->RChilds[i]->iEnd != -1){
					M_OVGraf* G1 = new M_OVGraf();
					G1->Copy(m,G->RChilds[i]);
					G->RChilds[i]->ForCopy = G1;
				}
			}
			if(m == 4){
				if(G->RChilds[i]->iEnd != -1){
					HHM_OVGraf* G1 = new HHM_OVGraf();
					G1->Copy(m,G->RChilds[i]);
					G->RChilds[i]->ForCopy = G1;
				}
			}
		}
		if(G->RChilds[i]->iEnd != -1){
		//	RChilds.push_back(G->RChilds[i]->ForCopy);
			G->RChilds[i]->ForCopy->RParent = this; 
		}
	}
return;
}

//////////To create overlap graf////////////////////////////	
int CurrentNumLOG = 1;
int CurrentNumROG = 1;
char* Let = NULL;
OV_Graf*** LeftChilds = NULL;
OV_Graf*** RightChilds = NULL;
OV_Graf** Nodes = NULL;
int* NumLChilds;
int* NLeaf = NULL;
int* NLOG = NULL;
int* NCl = NULL;
double* FiCl = NULL;
int* InLeaf = NULL;

void CreateLOG(OV_Graf* &G, AC_Trie* &T, AC_Trie* &T1, std::string word,double prob){
		
	int i;
	Let[0] = MainData::AlpMas[T->sign];
	word += Let;
	if((MainData::order == 0)&&(T->num != 0)){
		prob = prob* MainData::BernProb[T->sign];
	}
	if(T->lp == 0){
		T->toLOG = G;
	}
	if((T->main!=0)&(T->lp!=0)){
		OV_Graf* v;
		if(MainData::order < 0){ 
			v = new HHM_OVGraf(CurrentNumLOG);
		}
		if(MainData::order == 0){
			v = new BProb_Graf(CurrentNumLOG);
			BProb_Graf* v1 = static_cast<BProb_Graf*>(v);
			v1->Fi = prob;
		//	MainData::ResLOG<<word<<'\n'<<prob<<"\n\n";

			prob = 1;
		}
		if(MainData::order > 0){
			v = new M_OVGraf(CurrentNumLOG);
		}
		v->iEnd = T->iEnd;
		v->len = T->lp; 
		v->LParent = G;
		G->NLChilds ++;
		v->Lsign = word;
		Nodes[v->Lnum] = v;
		word = "";
		T->toLOG = v;
		CurrentNumLOG++;
		for( i = 0; i < MainData::AlpSize; i++){
			if(T->Childs[i]!=NULL){
				CreateLOG(v, T->Childs[i],T,word,prob);
		
			}
		}
	}
	else{
		for( i  =0; i < MainData::AlpSize; i++){
			if(T->Childs[i]!=NULL)
				CreateLOG(G, T->Childs[i],T1,word,prob);
		}
	}

}

//It Adds a node to ROG
void AddNodeROG(AC_Trie* &T){
	int i;
	if(T->lp!=0){
		AddNodeROG(T->nv);
	
		if(T->Rnum==0){
			T->toLOG->RParent = T->nv->toLOG;
			T->nv->toLOG->NRChilds ++;
			T->Rnum=1;
			CurrentNumROG++;
			AC_Trie *T1 = T;
			for(i = 0; i < T->nv->lp; i++){
				T1 = T1->Parent;	
			}
		}
	}

	return; 
}

//It creates ROG
void CreateROG(AC_Trie* &T){
	int i;

	if(T->iEnd != 0){
		 AddNodeROG(T);
	}	

	for( i = 0; i < MainData::AlpSize; i++){
		if(T->Childs[i]!=NULL){
			CreateROG(T->Childs[i]);
		}
	}
	return; 
} 
///////////////////////////////
int CrChilds(void){
	int i;
	int* CurNumLChilds = new int[OV_Graf::NumOVNodes];
	int* CurNumRChilds = new int[OV_Graf::NumOVNodes];
	
	for(i = 0; i < OV_Graf::NumOVNodes; i++){
		CurNumLChilds[i] = 0;
		CurNumRChilds[i] = 0;
	}
	
	for(i = 0; i < OV_Graf::NumOVNodes; i++){
		if(Nodes[i]->iEnd == 0){
			LeftChilds[Nodes[i]->Lnum] = new OV_Graf*[Nodes[i]->NLChilds];
			RightChilds[Nodes[i]->Lnum] = new OV_Graf*[Nodes[i]->NRChilds];
		}
		else{
			LeftChilds[Nodes[i]->Lnum] = NULL;
			RightChilds[Nodes[i]->Lnum] = NULL;
		}
	}
	
	for(i = 1; i < OV_Graf::NumOVNodes; i++){
	
		OV_Graf* G = Nodes[i]->LParent;
	
		int num = CurNumLChilds[G->Lnum];
	
		LeftChilds[G->Lnum][num] = Nodes[i];
	
		CurNumLChilds[G->Lnum]++;
	
		G = Nodes[i]->RParent;
	
		num = CurNumRChilds[G->Lnum];
	
		RightChilds[G->Lnum][num] = Nodes[i];
	
		CurNumRChilds[G->Lnum]++;
	
	}
	
	delete[] CurNumLChilds;
	delete[] CurNumRChilds;
	return 0;
}
///////////////////////////////
int CurrentNumCl = 0;

void OV_Graf:: MinGraf(void){
    int i;

	if(iEnd == 0){
		for(i = 0; i< NLChilds; i++){
			OV_Graf* G1 = LeftChilds[Lnum][i];
			if(G1->iEnd!=0){
				int nw = G1->iEnd;
				OV_Graf *RP = G1->RParent;	
				if(NLOG[RP->Lnum] != Lnum){
					CurrentNumCl++;
					NCl[G1->iEnd - 1] = CurrentNumCl;
					NLeaf[RP->Lnum] = CurrentNumCl;
					NLOG[RP->Lnum] = Lnum;
					if(MainData::order == 0){
						BProb_Graf* G2 = static_cast<BProb_Graf*>(G1);
						FiCl[CurrentNumCl - 1] = G2->Fi;
					}
				}
				else{
					NCl[G1->iEnd - 1] = NLeaf[RP->Lnum];
					InLeaf[G1->iEnd - 1] = 0;
					if(MainData::order == 0){
						int NumCl = NCl[G1->iEnd - 1];
						BProb_Graf* G2 = static_cast<BProb_Graf*>(G1);
						FiCl[NumCl - 1] += G2->Fi;
					}
				}
				if(MainData::order > 0){
					M_TrTree::ForLeaves[nw - 1] = NCl[G1->iEnd - 1];
				}
			}
		}

		for (  i = 0; i < NLChilds; i++) {
			if(LeftChilds[Lnum][i]->iEnd == 0){	
				LeftChilds[Lnum][i]->MinGraf();
			}
		}
	}
	return;
}

////////////////////////////////////////////////
void OV_Graf::DebPrint(void){
	int i;
	if(iEnd != 0){
		MainData::ResLOG<<"leaf: "<<Lsign<<'\t'<<iEnd<<'\t'<<NCl[iEnd -1]<<'\t'<<InLeaf[iEnd -1]<<"\n\n";
	}
	else{
		MainData::ResLOG<<"node: "<<Lsign<<'\t'<<Lnum<<"\n\n";
		for (  i = 0; i < NLChilds; i++) {
			LeftChilds[Lnum][i]->DebPrint();
		}
	}
}

void OV_Graf::DebPrint1(void){
	int i;
	BProb_Graf* G = static_cast<BProb_Graf*>(this);
	if(iEnd != 0){
		
		MainData::ResLOG<<"leaf: "<<Lsign<<'\t'<<iEnd<<'\t'<<G->Fi<<'\n';
		int j;
		for(j = 0; j < MainData::NOccur; j++){
			//MainData::ResLOG<<G->FirstTemp[j]<<'\t';
		}
		MainData::ResLOG<<"\n\n";
	}
	else{
		MainData::ResLOG<<"node: "<<Lsign<<'\t'<<Lnum<<'\t'<<G->Fi<<'\n';
		int j;
		for(i = 0; i < MainData::WordLen - len; i++){
			for(j = 0; j < MainData::NOccur; j++){
				MainData::ResLOG<<G->ProbMark[j][i]<<'\t';
			}
		}
		MainData::ResLOG<<"\n\n";
		for (  i = 0; i < NLChilds; i++) {
			LChilds[i]->DebPrint1();
		}
	}
}

//////////////////////////////////////////////////
void OV_Graf::CreateChilds(void){
	if(iEnd == 0){
		int i;
		int k;
		NumLChilds[Lnum] = NLChilds;
		int s = NLChilds;
		for(i = 0; i < s; i++){
			OV_Graf* G = LeftChilds[Lnum][i];
			if((G->iEnd != 0)&&(InLeaf[G->iEnd -1] != 1)){
				NLChilds--;
			}
		}
		LChilds= new OV_Graf*[NLChilds]; 
		k = 0;
		for(i = 0; i < s; i++){
			OV_Graf* G = LeftChilds[Lnum][i];
			if((G->iEnd == 0)||(InLeaf[G->iEnd -1] == 1)){
				LChilds[k] = LeftChilds[Lnum][i];
				k++;
			}
		}
		s = NRChilds;
		for(i = 0; i < s; i++){
			OV_Graf* G = RightChilds[Lnum][i];
			if((G->iEnd != 0)&&(InLeaf[G->iEnd -1] != 1)){
				NRChilds--;
			}
		}
		RChilds = new OV_Graf*[NRChilds];
		k = 0;
		for(i = 0; i < s; i++){
			OV_Graf* G = RightChilds[Lnum][i];
			if(G->iEnd == 0){
				RChilds[k] = RightChilds[Lnum][i];
				k++;
			}
		}
		for(i = 0; i < s; i++){
			OV_Graf* G = RightChilds[Lnum][i];
			if((G->iEnd != 0)&&(InLeaf[G->iEnd -1] == 1)){
				RChilds[k] = RightChilds[Lnum][i];
				k++;
			}
		}
		
		for(i = 0;i < NLChilds; i++){
			LChilds[i]->CreateChilds();
		}
	}
	return;
}


void OV_Graf::DeleteLeaves(void){
	if(iEnd!= 0){
		if(InLeaf[iEnd -1] == 1){
			iEnd = NCl[iEnd - 1];
		}
		else{
			delete this;
		}
	}
	else{
		int i;
		int s = NumLChilds[Lnum];
		for(i = 0; i < s; i++){
			LeftChilds[Lnum][i]->DeleteLeaves();
		}	
	
	}
}



// It creates grafs
int OV_Graf::CreateGraf(OV_Graf* root){
	AC_Trie::gTrie->toLOG = gOVG;
	Let = new char[2];
	Let[1] = '\0';
	
	Nodes = new OV_Graf*[AC_Trie::NumACNodes];
	Nodes[0] = root;
	
	CreateLOG(root,AC_Trie::gTrie, AC_Trie::gTrie,"",1);
	CreateROG(AC_Trie::gTrie);
	
	NumOVNodes = CurrentNumLOG;
	LeftChilds = new OV_Graf**[NumOVNodes]; 
	RightChilds = new OV_Graf**[NumOVNodes];
	
	CrChilds(); // <-- HERE IS A BUG !!!
	
	delete[] Nodes;
	
	NLeaf = new int[NumOVNodes];
	NLOG = new int[NumOVNodes];
	NCl = new int[MainData::NWords];
	InLeaf = new int[MainData::NWords];
	int i;
	if(MainData::order == 0){
		FiCl = new double[MainData::NWords];
		for(i =0; i < MainData::NWords; i++){
			FiCl[i] = 0;
		}
	}

	for(i = 0; i < NumOVNodes; i++){
		NLeaf[i] = -1;
		NLOG[i] = -1;
	}
	for(i = 0; i < MainData::NWords; i++){
		NCl[i] = 0;
		InLeaf[i] = 1;
	}
	//root->DebPrint();
	
	root->MinGraf();
	NClasses = CurrentNumCl;

	if(MainData::order != 0){
		Signs = new std::string[NumOVNodes];
	}
	//root->DebPrint();

	if(MainData::order == 0){
		BProb_Graf::PHCl = new double[NClasses];
		for(i = 0; i < NClasses; i++){
			BProb_Graf::PHCl[i] = 0;
		}
		root->CalPH(1);
	}
	if(MainData::order != 0){
		Classes = new Clazz[NClasses];
		root->MCalClasses("");
	}

	NumLChilds = new int[NumOVNodes];
	for(i = 0; i < NumOVNodes; i++){
		NumLChilds[i] = 0;
	}
	root->CreateChilds(); 
	root->DeleteLeaves();
	//root->DebPrint1();
	//MainData::ResLOG.close();
	//gOVG->DeepNodes(1);
	//gOVG->DeepNodes(2);
	delete[] Let;
	
	for(i = 0; i < NumOVNodes; i++){
		if(LeftChilds[i] != NULL){
			delete[] LeftChilds[i];
		}
		if(RightChilds[i] != NULL){
			delete[] RightChilds[i];
		}
	}
	delete[] LeftChilds;
	delete[] RightChilds;
	delete[] NumLChilds;
	delete[] NLeaf;
	delete[] InLeaf;
	delete[] NLOG;
	delete[] NCl;
	if(MainData::order == 0){
		delete[] FiCl;
	}
	return 0; 
}




//////////////////////////////////////////////////


void OV_Graf::CalPH(double prob){
	BProb_Graf* G = static_cast<BProb_Graf*>(this);
	if(Lnum != 0){
		prob = prob*G->Fi;
	}
	if(iEnd > 0){
		int NumCl = NCl[iEnd - 1];
		BProb_Graf::PHCl[NumCl - 1] += prob;
		G->Fi = FiCl[NumCl - 1];
	}
	if(iEnd == 0){
		int i;
		for ( i = 0; i < NLChilds; i++){
			OV_Graf* G = LeftChilds[Lnum][i];
			G->CalPH(prob);
		}
	}
}

void OV_Graf::MCalClasses(std::string word){
	std::string word1 = word;
	int i;
	if(Lnum != 0){
		word1 += Lsign;
	}
	if(MainData::order > 0){
		Signs[Lnum] = word1;
	}
	if(iEnd > 0){
		int NumCl = NCl[iEnd - 1];
		Classes[NumCl-1].Lsigns.push_back(Lsign);
		Classes[NumCl-1].Words.push_back(word1);
	}

	for ( i = 0; i < NLChilds; i++){
		OV_Graf* G = LeftChilds[Lnum][i];
		G->MCalClasses(word1);
	}
	return;
}

///////////////////////////////////////

//It prints LOG 
void OV_Graf:: PrintLOG(int z, ofstream *ff){
        int i;
		int j;

	if( Lnum == 0){	
		*(ff)<<"<!DOCTYPE wordsTree>"<<'\n';
		
		*(ff)<<"<Word value="<<'"'<<"LOG"<<'"'<<'>'<<'\n';
	
		for ( i = 0; i < NLChilds; i++)
				LChilds[i]->PrintLOG(z+1,ff);

		*(ff)<<"</Word>";
	}


	if(Lnum!= 0 ){
		
		for (j = 0; j < z; j++)
			*(ff)<<" "; 

		if (NLChilds!=0){
		
			*(ff)<<"<Word value="<<'"'<<Lsign<<'"'<<">\n";

			for ( i = 0; i < NLChilds; i++)
				LChilds[i]->PrintLOG(z+1, ff);
		
		
			for (j = 0; j < z; j++)
				*(ff)<<" "; 
		

			*(ff)<<"</Word>"<<'\n';
		} 
		else	
			*(ff)<<"<Word value="<<'"'<<iEnd<<'"'<<"/>\n";
	}	
			
return;
}


//It prints ROG 
void OV_Graf:: PrintROG(int z, ofstream *ff){
        int i;
		int j;

	if( Lnum == 0){
		*(ff)<<"<!DOCTYPE wordsTree>"<<'\n';
		
		*(ff)<<"<Word value="<<'"'<<"ROG"<<'"'<<'>'<<'\n';
	
		for ( i = 0; i < NRChilds; i++)
				RChilds[i]->PrintROG(z+1,ff);

		*(ff)<<"</Word>";
	}


	if(Lnum!= 0 ){
		
		for (j = 0; j < z; j++)
			*(ff)<<" "; 

		if (NRChilds!=0){
		
			*(ff)<<"<Word value="<<'"'<<Rsign<<'"'<<">\n";

			for ( i = 0; i < NRChilds; i++)
				RChilds[i]->PrintROG(z+1, ff);
		
		
			for (j = 0; j < z; j++)
				*(ff)<<" "; 
		

			*(ff)<<"</Word>"<<'\n';
		} 
		else	
			*(ff)<<"<Word value="<<'"'<<iEnd<<'"'<<"/>\n";
	}	
			
return;
}


