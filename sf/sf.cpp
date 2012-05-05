//#include "stdafx.h"
#include "maindata.h"
#include "ac_trie.h"
#include "ov_graf.h"
#include "bprob_graf.h"
#include "mmodel_prob.h"
#include "m_ovgraf.h" 
#include "m_trtree.h"
#include "nd_hhm_prob.h"
#include "d_hhm_prob.h"
#include "hhm_ovgraf.h"
#include "sf_main.h"

// POSIX-functions for time measure
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int i;
	time_t time0 = time(NULL);
	int ExitFlag = 0;
    int Error = MainData::ComLineParse(argc, argv);
    if(Error > 0)
        ExitFlag = 1;

	if(ExitFlag == 0){
		Error = MainData::GetInput();
		if(Error > 0)
			ExitFlag = 1;
	}
	if(ExitFlag == 0){
		Error = MainData::Check_Out_of_Range();
		if(Error > 0){
			ExitFlag = 1;
		}
	}

	ofstream out;
	ofstream outexp;
	outexp.open("GenInfo.txt", ios::app);
	if(MainData::Out_mode < 2){
		out.open(MainData::OutName.c_str(), ios::app);
	}
	else{
		out.open(MainData::OutName.c_str());
	}

	if((MainData::NOccur == 0)&&(ExitFlag == 0)){
		MainData::Pvalue =1;
		ExitFlag = 2;
	}
	
	if((MainData::TLen < MainData::WordLen)&&(ExitFlag == 0)){
		MainData::Pvalue = 0;
		ExitFlag = 2;
	}
	if((MainData::TLen == MainData::WordLen)&&(MainData::NOccur > 1)&&(ExitFlag == 0)){
		MainData::Pvalue = 0;
		ExitFlag = 2;
	}

	if((MainData::AlpSize == 1)&&(ExitFlag == 0)){
		if(MainData::TLen >= MainData::WordLen + MainData::NOccur - 1){
			MainData::Pvalue = 1;
			ExitFlag = 2;
		}
		else{
			MainData::Pvalue = 0;
			ExitFlag = 2;
		}
	}

    if((MainData::order > 0)&&(ExitFlag == 0)){
		if(MainData::order > MainData::WordLen){
			Error = 41;
			ExitFlag =  1;
		}
        M_TrTree::ForLeaves = new int[MainData::NWords];
    }

///////// 
	if(ExitFlag == 0){
		if(MainData::Out_mode == 3){
			MainData::ResWords.open(MainData::OutWordsName.c_str());
			    AC_Trie::gTrie->PrintTrie(&MainData::ResWords);
			    MainData::ResWords.close();
			}
	}
/////////    

	if(ExitFlag == 0){
		AC_Trie::CreateTrie();
	    if(MainData::order < 0){
			HHM_OVGraf::gHHM = new HHM_OVGraf();
			OV_Graf::CreateGraf(HHM_OVGraf::gHHM);
	        HHM_OVGraf::gHHM->Preprocessing();
	        delete AC_Trie::gTrie;
			 HHM_OVGraf::gHHM->ProbCalc();
			 if(MainData::Out_mode > 1){
				 MainData::PrintMain(&out,0);
			 }
			delete HHM_OVGraf::gHHM;
		}   
		if(MainData::order == 0){
			BProb_Graf::gBOVG = new BProb_Graf(0);
			OV_Graf::CreateGraf(BProb_Graf::gBOVG);
			delete AC_Trie::gTrie;
			BProb_Graf::ProbCalc();
			if(MainData::Out_mode > 1){
				 MainData::PrintMain(&out,0);
			 }
			delete BProb_Graf::gBOVG;
			delete[] BProb_Graf::PHCl;
			delete[] BProb_Graf::BSumProb;
			if((MainData::mode == 2)||(MainData::mode == 3)){
				for(i = 0; i < MainData::WordLen; i++){
					delete[] MainData::PssmMas[i];
				}
				delete[] MainData::PssmMas;
				MainData::PssmMas = NULL;
			}
			if((MainData::mode == 4)&(MainData::ConstPositions != NULL)){
				delete[] MainData::ConstPositions;
			}
		}

	    if(MainData::order > 0){
			M_TrTree::gMTr = new M_TrTree();
			M_OVGraf::gMOVG = new M_OVGraf();
			OV_Graf::CreateGraf(M_OVGraf::gMOVG);
	        M_OVGraf::gMOVG->Preprocessing();
			delete AC_Trie::gTrie;
		    M_OVGraf::gMOVG->ProbCalc();
			if(MainData::Out_mode > 1){
				 MainData::PrintMain(&out,0);
			 }
	        delete M_OVGraf::gMOVG;
			delete M_TrTree::gMTr;
	    }
		if(MainData::order != 0){
			H_M_OVGraf::DelMas();
		}
	}
	
	if(MainData::mode == 1){
		delete[] MainData::RandPatProbs;
	}
	if((MainData::order > 0)&&(MainData::CrDistribFlag == 1)){
		for(i = 0; i < MainData::AlpSize; i++){
			delete[] MainData::MarkovProbs[i];
		}
		delete[] MainData::MarkovProbs;
	}
	if((MainData::Out_mode < 2)&&(ExitFlag != 1)){
		//out<<MainData::Pvalue<<'\n';
	}
	if(ExitFlag > 0){
		if((MainData::Out_mode > 1)&&(ExitFlag == 2)){
			 MainData::PrintMain(&out,2);
		}
		int j;
		if((MainData::order == -2)&&(MainData::CrDistribFlag == 1)){
			for(i = 0; i < H_M_OVGraf::NumAllStates; i++){
				for(j = 0; j < H_M_OVGraf::NumAllStates; j++){
					delete[] MainData::ND_HHMProbs[i][j];
				}
				delete[] MainData::ND_HHMProbs[i];
				MainData::ND_HHMTrans[i]->clear();
				delete[] MainData::ND_HHMTrans[i];
			}
			delete[] MainData::ND_HHMProbs;
			delete[] MainData::ND_HHMTrans;
		}
		if((MainData::order == -1)&&(MainData::CrDistribFlag == 1)){
			for(i = 0; i < H_M_OVGraf::NumAllStates; i++){	
				delete[] MainData::D_HHMProbs[i];
				delete[] MainData::D_HHMTrans[i];
			}
			delete[] MainData::D_HHMProbs;
			delete[] MainData::D_HHMTrans;
		}
		if(AC_Trie::gTrie != NULL){
			delete AC_Trie::gTrie;
		}
		MainData::ErrorDetect(Error);
		return 1;
	}
	time0 = time(NULL) - time0;
	//out<<'\t'<<time0<<'\n';
	
	
	// Print resources used
	int who = RUSAGE_SELF;
	struct rusage usage;
	int ret;
	ret = getrusage(who, &usage);
	if (ret==0) {
	    std::cout << "Used resources: \n";
	    struct timeval utime = usage.ru_utime;
	    struct timeval stime = usage.ru_stime;
	    long maxrss = usage.ru_maxrss;
	    long u_millsecs = (utime.tv_sec * 1000000 + utime.tv_usec) / 1000;
	    long s_millsecs = (stime.tv_sec * 1000000 + stime.tv_usec) / 1000;
	    long total_millsecs = u_millsecs + s_millsecs;
	    std::cout << "user time: " << u_millsecs << "ms; system time: " << s_millsecs << "ms" << std::endl;
	    std::cout << "total time: " << total_millsecs << "ms" << std::endl;
	    long pagesize = sysconf(_SC_PAGE_SIZE);
	    long memused = maxrss * pagesize;
	    std::cout << "Used " << maxrss << " pages of size " << pagesize << "; total: " << memused << std::endl;
	    out<<MainData::Pvalue<<'\t'<<total_millsecs<<'\t'<<memused<<'\n';
	   outexp<<MainData::WordLen<<'\t'<<MainData::NWords<<'\t'<<AC_Trie::NumACNodes<<'\t'<<OV_Graf::NumOVNodes-MainData::NWords<<'\t'<<OV_Graf::NClasses<<'\n';
	}
	else {
	    std::cout << "Can't use getrusage!\n";
	}
	out.close();
	return 0;
}
