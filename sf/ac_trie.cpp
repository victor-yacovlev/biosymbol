#include "ac_trie.h"
#include "maindata.h"

int AC_Trie::NumACNodes  = 0;	
AC_Trie* AC_Trie::gTrie = NULL;	//root of the trie

AC_Trie::AC_Trie()
{
	int i;
	sign = -1;	
	iEnd = 0; //if T corresponds to a final state;
	lp = 0;
	num = AC_Trie::NumACNodes;
	AC_Trie::NumACNodes++;
	main = 0;
	Rnum = 0;
	Childs = new AC_Trie*[MainData::AlpSize];  
	for( i = 0; i < MainData::AlpSize; i++){
		Childs[i] = NULL;
	}
	Parent = NULL;	
	nv = NULL;
	toLOG = NULL;
	
}

AC_Trie::~AC_Trie(){
	int i;
	for(i=0; i < MainData::AlpSize; i++){
		if(Childs[i]!=NULL){
			delete Childs[i];
			Childs[i] = 0;
		} 
	}
	delete[] Childs;
}


int AC_Trie:: InsertWord(int* Word, int Number)
{
	if (Word[0] == -1)
	{
		if(iEnd == 0){
			iEnd = Number;
			return 0;
		}
		else{
			return 1;
		}
	}
	else
	{ 
		int k = Word[0];
		if (Childs[k] == NULL)
		{
			AC_Trie* T = new AC_Trie;
			Childs[k]= T;
			Childs[k]->Parent = this;
			Childs[k]->lp = lp + 1;
			Childs[k]->sign = Word[0];
		}

		int ind = Childs[k]->InsertWord(Word+1, Number);
		if(ind == 1){
			return 1;
		}
	}
	return 0;
}



//It creates a suffix links

void preLink()
{
	AC_Trie** queue = new AC_Trie*[AC_Trie::NumACNodes + 1];
	int i;	
	for(i = 0; i < AC_Trie::NumACNodes; i++){
		queue[i] = NULL;
	}
	int first = 0;
	int last = -1;
	int size = 0;

	AC_Trie::gTrie->nv = AC_Trie::gTrie;
	AC_Trie::gTrie->main = 1;  

	for ( i=0; i<MainData::AlpSize; i++)
		if (AC_Trie::gTrie->Childs[i] != NULL) 
		{
			AC_Trie::gTrie->Childs[i]->nv = AC_Trie::gTrie;
		}	

		for ( i = 0; i < MainData::AlpSize; i++){
			if (AC_Trie::gTrie->Childs[i] != NULL){
				last ++;
				size ++;
				queue[last] = AC_Trie::gTrie->Childs[i];
			}
		}

	while (size > 0) //queue is not empty
	{
		AC_Trie *v1= queue[first]; //pop				
		first++;
		size--;
		int x;
		for (x=0; x < MainData::AlpSize; x++)		
			if (v1->Childs[x]  != NULL)
			{
				AC_Trie *v = v1->Childs[x];
				last ++;
				size ++;
				queue[last] = v; //push
				AC_Trie *w = v1->nv;

				while ((w->Childs[x] == NULL) && (w != AC_Trie::gTrie))
				{
					w = w->nv;			
				}				

				if (w->Childs[x] != NULL)
					v->nv = w->Childs[x];
				else
					v->nv = AC_Trie::gTrie;

			}
	}
	delete[] queue;
	return;
}


////////////////////////////////////////////////
////////////////////////////////////////////////

void AC_Trie:: Path(AC_Trie* &T, std::string* word){	
	
	if((lp!=0)&&(num!=T->num)){
		Parent->Path(T,word);
		*(word)+=MainData::AlpMas[sign];
	}
	return;
}


//It prints way root-vertex
void AC_Trie:: PrintWay(ostream *ff){
	if(lp!=0){
		Parent->PrintWay(ff);
		*(ff)<<MainData::AlpMas[sign];
	}
	return;
}

//It Prints a subtrie
void AC_Trie:: PrintTrie(ostream *ff){
	int i;
	if(iEnd!=0){
		PrintWay(ff);
		*(ff)<<'\n';

	}	
	for( i=0; i<MainData::AlpSize; i++){
		if(Childs[i]!=NULL){
			Childs[i]->PrintTrie(ff);
		}  			
	}
}



void AC_Trie::Deb_Print(ofstream* ff){
	int i;
	if((main == 1)&&(iEnd == 0)){
		PrintWay(ff);
		*(ff)<<'\n';
	}	
	for( i=0; i<MainData::AlpSize; i++){
		if(Childs[i]!=NULL){
			Childs[i]->Deb_Print(ff);
		}  			
	}
}
///////////////////////////////////////
// It markes main node
void AC_Trie:: MainNode(){
	AC_Trie *v = this;

	while(v->lp!=0){
		v->main = 1;
		v = v->nv;
	}
	return; 
} 

void AC_Trie:: MarkMainNodes(){
	int i;
	
	if(iEnd!=0)
		MainNode();

	for(i = 0; i < MainData::AlpSize; i++){
		if(Childs[i]!=NULL){
			Childs[i]->MarkMainNodes();
		}
	}
}

//////////////////////////////////////////

//////////////////////////////////////////
// It creates Trie

void AC_Trie::CreateTrie(){
	preLink();
	gTrie->MarkMainNodes();
	return; 
}
////////////////////////
