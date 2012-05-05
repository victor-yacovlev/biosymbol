#include "maindata.h"
#include "minov_graf.h"
#include "m_trtree.h"



MinOV_Graf* MinOV_Graf::gMinOVG = NULL;


MinOV_Graf::MinOV_Graf():
	OV_Graf()
{	
	NLeaf = -1;
	NLOG =  -1;
	NCl = 0;
}

MinOV_Graf::~MinOV_Graf()

{
}


//int CurrentNumCl = 0;

void MinOV_Graf:: MinGraf(void){
   /* size_t i;

	if(LDeep == 1){

		for(i = 0; i< NLChilds(); i++){
			OV_Graf* G2 = LChilds[i];
			MinOV_Graf* G1 = static_cast<MinOV_Graf*>(G2);
			if(G1->iEnd!=0){
				int nw = G1->iEnd;
				MinOV_Graf *G = static_cast<MinOV_Graf*>(G1->RParent);	
				if(G->NLOG != Lnum){
					CurrentNumCl++;
					G1->NCl = CurrentNumCl;
					G1->iEnd = CurrentNumCl;
					G->NLeaf = CurrentNumCl;
					G->NLOG = Lnum;	
				}
				else{
					G1->NCl = G->NLeaf;
					G1->iEnd = -1;
				}
				if(MainData::order > 0){
					M_TrTree::ForLeaves[nw - 1] = G1->NCl;
				}
			}
		}
	}

	for (  i = 0; i < NLChilds(); i++) {
		if(LChilds[i]->iEnd == 0){	
			OV_Graf* G = LChilds[i];
			MinOV_Graf* G1 = static_cast<MinOV_Graf*>(G);
			G1->MinGraf();
		}
	}
	return;
	*/
}


void MinOV_Graf::MinimizeGraf(void){
	gMinOVG->MinGraf();
	//NClasses = CurrentNumCl;
}

