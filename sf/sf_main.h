#ifdef __cplusplus
//set some input parameters
extern "C" int func_set_input_data( int order, int mode, int TLen, int NOccur);

//set alphabet and distribution letters for Bernoulli model
extern "C" int func_analis_alp_bern_data(int AlpSize, char* AlpMas, double* BernProb);

//set alphabet and distribution letters for Markov model
extern "C" int func_analis_alp_mark_data(int AlpSize, char* AlpMas, double* IniProbs, double** MarkovProbs);

//set alphabet and distribution letters for deterministic HHM
extern "C" int func_analis_alp_dhhm_data(int AlpSize, char* AlpMas, int NumAllStates,
							  double** D_HHMProbs, int** D_HHMTrans);

//set alphabet and distribution letters for HHM
extern "C" int func_analis_alp_hhm_data(int AlpSize, char* AlpMas,
							 int NumAllStates, double*** ND_HHMProbs);

//give pattern described by a list of words
extern "C" int func_analis_pattern_data_0(int NWords, char **WordsList);

//give random pattern
extern "C" int func_analis_pattern_data_1(int NWords, int WordLen, double* RandPatProbs);

//give pattern described by a PSSM and Cut-of (list of footprints)
extern "C" int func_analis_pattern_data_2_3(int WordLen, int NFootPrints, char **FootPrints, 
										double** PssmMas, double Thr);

//give pattern described by a word and number of mismatches
extern "C" int func_analis_pattern_data_4(char* motif, int Nreplace, int NConstPositions, 
									  int *ConstPositions);

//give pattern described by consensus
extern "C" int func_analis_pattern_data_5(char *consensus, int NSymbols, char **ConsAlp);

//main algorithm
extern "C" int func_main(double* pvalue, char** report, char* **ResWords, int *NWords);
#else
extern int func_set_input_data( int order, int mode, int TLen, int NOccur);
extern int func_analis_alp_bern_data(int AlpSize, char* AlpMas, double* BernProb);
extern int func_analis_alp_mark_data(int AlpSize, char* AlpMas, double* IniProbs, double** MarkovProbs);
extern int func_analis_alp_dhhm_data(int AlpSize, char* AlpMas, int NumAllStates,
							  double** D_HHMProbs, int** D_HHMTrans);
extern int func_analis_alp_hhm_data(int AlpSize, char* AlpMas,
							 int NumAllStates, double*** ND_HHMProbs);
extern int func_analis_pattern_data_0(int NWords, char **WordsList);
extern int func_analis_pattern_data_1(int NWords, int WordLen, double* RandPatProbs);
extern int func_analis_pattern_data_2_3(int WordLen, int NFootPrints, char **FootPrints, 
										double** PssmMas, double Thr);
extern int func_analis_pattern_data_4(char* motif, int Nreplace, int NConstPositions, 
									  int *ConstPositions);
extern int func_analis_pattern_data_5(char *consensus, int NSymbols, char **ConsAlp);
extern int func_main(double* pvalue, char** report, char* **ResWords, int *NWords);
#endif

