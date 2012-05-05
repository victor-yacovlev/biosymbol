//#include "StdAfx.h"
#include "bprob_graf.h"
#include "maindata.h"

double*	BProb_Graf::PHCl;
double*	BProb_Graf::BSumProb;
BProb_Graf* BProb_Graf::gBOVG = NULL;

BProb_Graf::BProb_Graf(void)
	:OV_Graf()
{
	Fi = 0;
	FirstTemp = NULL;
	ProbMark = NULL;
}

BProb_Graf::BProb_Graf(int num1)
	:OV_Graf(num1)
{
	Fi = 0;
	FirstTemp = NULL;
	ProbMark = NULL;
}

BProb_Graf::~BProb_Graf(void)
{
	int i;
	if(FirstTemp != NULL)
		delete[] FirstTemp;
	if(ProbMark != NULL){
		for(i = 0; i < MainData::NOccur; i++){	
			delete[] ProbMark[i];
		}
		delete[] ProbMark;
	}
}

////////////////////////////////////


void BProb_Graf::PredStep(void){
	int i;
	PHCl = new double[NClasses];
	for(i = 0; i < NClasses; i++){
		PHCl[i] = 0;
	}
} 




//It calculates probability of a motif (Bernoulli model)
double BProb_Graf::BernProbMotif(const std::string& motif){
	int j;
	size_t i;
	double p = 1;
	
	for(i = 0; i<motif.length(); i++){
		j = MainData::AToi(motif[i]);
		p = p*MainData::BernProb[j];
	}
	return p;
}


//It calculates Prob(Back(w)) for all nodes w in OVG
void BProb_Graf::BernFi(void){
	int i;

	if(iEnd == 0){
		if(Lnum ==0){
			Fi = 1;
		}
		else{
			Fi = BernProbMotif(Lsign);
		}
	}

	for ( i = 0; i < NLChilds; i++){
		OV_Graf* G = LChilds[i];
		BProb_Graf* G1 = static_cast<BProb_Graf*>(G);
		G1->BernFi();
	}
	
	return;
}


//////////////////////////////////////
// preliminary steps for probability calculation

void BProb_Graf::IniProbs(void){
	if(iEnd == 0){
		int i,j, m;
	
		ProbMark = new double*[MainData::NOccur];
		m = MainData::WordLen - len;  
	
		for(i = 0; i < MainData::NOccur; i++){	
			ProbMark[i] = new double[m];

		}

		FirstTemp = new double[MainData::NOccur];
	
		for(i = 0; i < MainData::NOccur; i++){
			for(j = 0; j < m; j++){
				ProbMark[i][j] = 0;
			}
			FirstTemp[i] = 0;
		}
			int l;
			for ( l = 0; l < NLChilds; l++){
				OV_Graf* G = LChilds[l];
				BProb_Graf* G1 = static_cast<BProb_Graf*>(G);
				G1->IniProbs();
			}
	
		}
		return;
	}

//initialization of Firsttemp
void BProb_Graf::IniFTemp(void){
	
	if(iEnd == 0){
		int i;
		for(i = 0; i < MainData::NOccur; i++){
			FirstTemp[i] = 0;
		}
		for(i = 0; i< NLChilds; i++){
			OV_Graf* G = LChilds[i];
			BProb_Graf* G1 = static_cast<BProb_Graf*>(G);
			G1->IniFTemp();
		}
	}
	return;
}


//Initialization of ProbMark
void BProb_Graf::IniProbMark(void){
	int i,j;

	for(i = 0; i < MainData::NOccur; i++){
		for(j = 0; j < len; j++){	
			ProbMark[i][j] = 0;
		}
	}
	if(iEnd == 0){
		int l;
		for ( l = 0; l < NLChilds; l++){
			OV_Graf* G = LChilds[l];
			BProb_Graf* G1 = static_cast<BProb_Graf*>(G);
			G1->IniProbMark();
		}
	}
	return;
}

//calculates initial probabilities
void BProb_Graf::Stepm(void){
	int i;
	
	if(iEnd==0){

		for ( i = 0; i < NRChilds; i++){
			OV_Graf* G = RChilds[i];
			BProb_Graf* G1 = static_cast<BProb_Graf*>(G);
			G1->Stepm();
			if(G1->iEnd != 0){
				FirstTemp[0] = FirstTemp[0] + PHCl[G1->iEnd-1];
			}
			else{
				FirstTemp[0] = FirstTemp[0] + G1->FirstTemp[0];
			}
		}
		ProbMark[0][MainData::WordLen - len - 1]= FirstTemp[0];
		
		if((Lnum == 0)&&(MainData::NOccur == 1)){
			MainData::Pvalue = FirstTemp[0];
			MainData::ProbRes = FirstTemp[0];
		}
	}
}


double Psi[MaxOccur];
double Psi1[MaxOccur];
//It calculates Psi and probabilities of the patterns

void BProb_Graf::PsiCalc(int n){
	int i;
	int j;
	
	if(iEnd != 0){
		BProb_Graf* R = static_cast<BProb_Graf*>(RParent);
		int pos = n % (MainData::WordLen - R->len);
		for(j = 0; j < MainData::NOccur; j++){
			double prob = 0;
			if(j == 0){
				prob = (1 - BSumProb[0])* PHCl[iEnd-1] - Psi[0]*Fi;
			} 
			else{
				prob = (BSumProb[j-1] - BSumProb[j])*PHCl[iEnd-1] + (Psi[j-1] - Psi[j])*Fi;		
			}
			R->FirstTemp[j] += prob;
		}
	}
	else{
		int pos = n % (MainData::WordLen - len);
		double Psi1[MaxOccur];
	
		for(j = 0; j < MainData::NOccur; j++){
			Psi1[j] = Psi[j];
		}
	
		if(Lnum == 0){
			for(j = 0; j < MainData::NOccur; j++){
				Psi[j] = ProbMark[j][pos];
			}
		}
		else{
			for(j = 0; j < MainData::NOccur; j++){
				Psi[j] = Psi[j]*Fi + ProbMark[j][pos];
			}
		}
		
		for ( i = 0; i < NLChilds; i++){
			BProb_Graf* G1 = static_cast<BProb_Graf*>(LChilds[i]);
			G1->PsiCalc(n);
		}

		for(j = 0; j < MainData::NOccur; j++){
			Psi[j] = Psi1[j];
		}
		
	}
	return;
}

// It Updates ROG

void BProb_Graf::ROGUpdate(int n){
	if(iEnd == 0){
		int i;
		int j;
		int pos = n % (MainData::WordLen - len);

		if(Lnum == 0){
			for(j = 0; j < MainData::NOccur; j++){	
				BSumProb[j] = BSumProb[j] + ProbMark[j][pos];
			}
		}
		
		i = 0;
		while((i < NRChilds)&&(RChilds[i]->iEnd == 0)){
			BProb_Graf* G1 = static_cast<BProb_Graf*>(RChilds[i]);
			G1->ROGUpdate(n);

			for(j = 0; j < MainData::NOccur; j++){
				FirstTemp[j] = FirstTemp[j] + G1->FirstTemp[j];
				G1->FirstTemp[j] = 0;
			}	
			i++;

		}
		for(j = 0; j < MainData::NOccur; j++){
			ProbMark[j][pos] = FirstTemp[j];
		}

		if(Lnum == 0){
			MainData::ProbRes = ProbMark[MainData::NOccur - 1][pos];
			MainData::Pvalue += MainData::ProbRes;
			for(j = 0; j < MainData::NOccur; j++){
				FirstTemp[j] = 0;
			}	
		}
	}
	return;
}


//Main function for probabilities calculation


void BProb_Graf::ProbCalc(void){
	int i;
	gBOVG->BernFi();

	BSumProb  = new double[MainData::NOccur];
	for(i = 0; i< MainData::NOccur; i++){
		BSumProb[i] = 0;;
	}
	
	gBOVG->IniProbs();
	
	if(MainData::TLen > MainData::WordLen-1){
		gBOVG->Stepm();
		gBOVG->IniFTemp();
	}
	
	if(MainData::NOccur == 1){
		MainData::Pvalue = gBOVG->ProbMark[MainData::NOccur - 1][MainData::WordLen - 1];
	}

	if(MainData::TLen > MainData::WordLen){
		int m = 0;
		for(i = MainData::WordLen+1; i<= MainData::TLen; i++){	
			gBOVG->PsiCalc(m);
			gBOVG->ROGUpdate(m);
			m++;
		}
	}
	return;
}




///////////////////////////////////////////////////

void BProb_Graf::DebPrint2(std::string word, int step){
	int i,j;	
	word += Lsign;
	if(Lnum == 0){
		MainData::ResLOG<<"Step "<<step<<"\n-------\n--------\n\n";
	}
	if(iEnd != 0){
		MainData::ResLOG<<"Class "<<iEnd<<'\n';
	}
	MainData::ResLOG<<"Sign: "<<word<<"; Lsign: "<<Lsign<<'\n';
	if(iEnd == 0){
		for(i = 0; i< MainData::WordLen - len; i++){ 
			for(j = 0; j < MainData::NOccur; j++){
				MainData::ResLOG<<ProbMark[j][i]<<'\t';
			}
			MainData::ResLOG<<'\n';
		}
	
		for(i = 0; i < MainData::NOccur; i++){
			MainData::ResLOG<<FirstTemp[i]<<'\t';
		}
	}
	MainData::ResLOG<<"\n\n";

	for ( i = 0; i < NLChilds; i++){
		OV_Graf* G = LChilds[i];
		BProb_Graf* G1 = static_cast<BProb_Graf*>(G);
		G1->DebPrint2(word,step);
	}
}







