#include "Python.h"
#include "sf_main.h"
#include <assert.h>

/* extern int func_set_input_data( int order, int mode, int TLen, int NOccur); */
/* function func_set_input_data(order: int, mode: int, TLen: int, NOccur: int): int */

extern PyObject* py_func_set_input_data(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"order", "mode", "TLen", "NOccur", NULL };
    int order = 0;
    int mode = 0;
    int TLen = 0;
    int NOccur = 0;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "iiii", kwlist, &order, &mode, &TLen, &NOccur))
        return NULL;
    int ret = func_set_input_data(order, mode, TLen, NOccur);
    PyObject* result = Py_BuildValue("i",ret);
    return result;
}

/* extern int func_analis_alp_bern_data(int AlpSize, char* AlpMas, int order, double* BernProb); */
/* function func_analis_alp_bern_data(AlpMas: string, order: int, BernProb: [float] ): int */
extern PyObject* py_func_analis_alp_bern_data(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"AlpMas", "BernProb", NULL };
    int AlpSize = 0;
    char *AlpMas = NULL;
    double *BernProb = NULL;
    PyObject * l = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sO", kwlist, &AlpMas, &l))
        return NULL;
    AlpSize = strlen(AlpMas);
    assert (AlpSize==PyList_Size(l));
    int i = 0;
    double val = 0.0;
    PyObject * current = 0;
    BernProb = (double*)calloc(AlpSize, sizeof(double));
    for (i=0; i<AlpSize; i++) {
        current = PyList_GetItem(l, i);
        val = PyFloat_AsDouble(current);
        BernProb[i] = val;
    }
    
    int ret = func_analis_alp_bern_data(AlpSize, AlpMas, BernProb);
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_analis_alp_mark_data(int AlpSize, char* AlpMas, double** MarkovProbs); */
/* function func_analis_alp_mark_data(AlpMas: string, MarkovProbs: [[float]]) : int */
extern PyObject* py_func_analis_alp_mark_data(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"AlpMas", "MarkovProbs", "IniProbs", NULL };
    int AlpSize = 0;
    char *AlpMas = NULL;
    double **MarkovProbs = NULL;
    double *IniProbs = NULL;
    PyObject * l = NULL;
    PyObject * l2 = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sOO", kwlist, &AlpMas, &l, &l2))
        return NULL;
    AlpSize = strlen(AlpMas);
    assert (AlpSize==PyList_Size(l));
    int i = 0;
    int j = 0;
	int l_size = 0;
    int ll_size = 0;
    double val = 0.0;
    PyObject * ll = 0;
    PyObject * current = 0;
    l_size = PyList_Size(l);
	
	
    MarkovProbs = (double**)calloc(l_size, sizeof(double*));
    for (i=0; i<l_size; i++) {
	ll = PyList_GetItem(l, i);
	ll_size = PyList_Size(ll);
	assert(ll_size==AlpSize);
	MarkovProbs[i] = (double*)calloc(ll_size, sizeof(double));
	for (j=0; j<ll_size; j++) {
	    current = PyList_GetItem(ll, j);
	    val = PyFloat_AsDouble(current);
	    MarkovProbs[i][j] = val;
	}
    }
	
    int l2_size = PyList_Size(l2);
    if (l2_size>0) {
	IniProbs = (double*)calloc(l2_size, sizeof(double));
	for (i=0; i<l2_size; i++) {
	    current = PyList_GetItem(l2, i);
	    val = PyFloat_AsDouble(current);
	    IniProbs[i] = val;
	}
    }
    //    printf("Alp size: %i, MarkovProbs size: %i, IniProbs size: %i\n", AlpSize, l_size, l2_size);
    int ret = func_analis_alp_mark_data(AlpSize, AlpMas, IniProbs, MarkovProbs);
    for (i=0; i<AlpSize; i++) {
        free(MarkovProbs[i]);
    }
    free(MarkovProbs);
    free(IniProbs);
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_analis_alp_dhhm_data(int AlpSize, char* AlpMas, int NumAllStates,
                              double** D_HHMProbs, int** D_HHMTrans); */
/* function func_analis_alp_dhhm_data(AlpMas: string, NumAllStates: int, 
                                      D_HHMProbs: [[float]], D_HHMTrans: [[float]]) : int */
extern PyObject* py_func_analis_alp_dhhm_data(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"AlpMas", "NumAllStates", "D_HHMProbs", "H_HHMTrans", NULL };
    int AlpSize = 0;
    char *AlpMas = NULL;
    int NumAllStates = 0;
    double **D_HHMProbs = NULL;
    int **D_HHMTrans = NULL;
    PyObject * l1 = NULL;
    PyObject * l2 = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "siOO", kwlist, &AlpMas, &NumAllStates, &l1, &l2))
        return NULL;
    AlpSize = strlen(AlpMas);
    assert (NumAllStates==PyList_Size(l1));
    assert (NumAllStates==PyList_Size(l2));
    int i = 0;
    int j = 0;
    int ll1_size = 0;
    int ll2_size = 0;
    double val1 = 0.0;
    int val2 = 0;
    PyObject * ll1 = 0;
    PyObject * ll2 = 0;
    PyObject * current1 = 0;
    PyObject * current2 = 0;
    D_HHMProbs = (double**)calloc(NumAllStates, sizeof(double*));
    D_HHMTrans = (int**)calloc(NumAllStates, sizeof(int*));
    for (i=0; i<NumAllStates; i++) {
        ll1 = PyList_GetItem(l1, i);
        ll2 = PyList_GetItem(l2, i);
        ll1_size = PyList_Size(ll1);
        ll2_size = PyList_Size(ll2);
        assert (AlpSize==ll1_size);
        assert (AlpSize==ll2_size);
        D_HHMProbs[i] = (double*)calloc(ll1_size, sizeof(double));
        D_HHMTrans[i] = (int*)calloc(ll2_size, sizeof(int));
        for (j=0; j<ll1_size; j++) {
            current1 = PyList_GetItem(ll1, j);
            val1 = PyFloat_AsDouble(current1);
            D_HHMProbs[i][j] = val1;
            current2 = PyList_GetItem(ll2, j);
            val2 = PyInt_AsLong(current2);
            D_HHMTrans[i][j] = val2;
        }
    }

    int ret = func_analis_alp_dhhm_data(AlpSize, AlpMas, NumAllStates, D_HHMProbs, D_HHMTrans);
    for (i=0; i<NumAllStates; i++) {
        free(D_HHMProbs[i]);
        free(D_HHMTrans[i]);
    }
    free(D_HHMProbs);
    free(D_HHMTrans);
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_analis_alp_hhm_data(int AlpSize, char* AlpMas,
                        int NumAllStates, double*** ND_HHMProbs); */
/* function func_analis_alp_hhm_data(AlpMas: string, NumAllStates: int, 
                                     ND_HHMProbs: [[[float]]]): int */
extern PyObject* py_func_analis_alp_hhm_data(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"AlpMas", "NumAllStates", "ND_HHMProbs", NULL };
    int AlpSize = 0;
    char *AlpMas = NULL;
    int NumAllStates = 0;
    double ***ND_HHMProbs = NULL;
    PyObject * l = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "siO", kwlist, &AlpMas, &NumAllStates, &l))
        return NULL;
    AlpSize = strlen(AlpMas);
    assert (NumAllStates==PyList_Size(l));
    int i = 0;
    int j = 0;
    int k = 0;
    int ll_size = 0;
    int lll_size = 0;
    double val = 0.0;
    PyObject * ll = 0;
    PyObject * lll = 0;
    PyObject * current = 0;
    ND_HHMProbs = (double***)calloc(NumAllStates, sizeof(double**));
    for (i=0; i<NumAllStates; i++) {
        ll = PyList_GetItem(l, i);
        ll_size = PyList_Size(ll);
        assert(NumAllStates==ll_size);
        ND_HHMProbs[i] = (double**)calloc(ll_size, sizeof(double*));
        for (j=0; j<ll_size; j++) {
            lll = PyList_GetItem(ll, j);
            lll_size = PyList_Size(lll);
            assert(AlpSize==lll_size);
            ND_HHMProbs[i][j] = (double*)calloc(lll_size, sizeof(double));
            for (k=0; k<lll_size; k++) {
                current = PyList_GetItem(lll, k);
                val = PyFloat_AsDouble(current);
                ND_HHMProbs[i][j][k] = val;
            }
        }
    }
    int ret = func_analis_alp_hhm_data(AlpSize, AlpMas, NumAllStates, ND_HHMProbs);
    for (i=0; i<NumAllStates; i++) {
        for (j=0; j<NumAllStates; j++) {
            free(ND_HHMProbs[i][j]);
        }
        free(ND_HHMProbs[i]);
    }
    free(ND_HHMProbs);
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_analis_pattern_data_0(int NWords, char **WordsList); */
/* function func_analis_pattern_data_0(WordsList: [string]): int */
extern PyObject* py_func_analis_pattern_data_0(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"WordsList", NULL };
    int NWords = 0;
    PyObject * l = 0;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "O", kwlist, &l))
        return NULL;
    NWords = PyList_Size(l);
    int i = 0;
    char **WordsList = (char**)calloc(NWords, sizeof(char*));
    char *val = 0;
    PyObject * current = 0;
    for (i=0; i<NWords; i++) {
        current = PyList_GetItem(l, i);
        val = PyString_AsString(current);
        WordsList[i] = val;
    }
    int ret = func_analis_pattern_data_0(NWords, WordsList);
    free(WordsList);
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_analis_pattern_data_1(int NWords, int WordLen, double* RandPatProbs); */
/* function func_analis_pattern_data_1(NWords: int, WordLen: int, RandPatProbs: [float]): int */ 
extern PyObject* py_func_analis_pattern_data_1(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"NWords", "WordLen", "RandPatProbs", NULL };
    int NWords = 0;
    int WordLen = 0;
    double *RandPatProbs = NULL;
    PyObject * l = 0;
    int l_size = 0;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "iiO", kwlist, &NWords, &WordLen, &l))
        return NULL;
    l_size = PyList_Size(l);
    int i = 0;
    if (l_size>0) {
        RandPatProbs = (double*)calloc(l_size, sizeof(double));
    }
    double val = 0.0;
    PyObject * current = 0;
    for (i=0; i<l_size; i++) {
        current = PyList_GetItem(l, i);
        val = PyFloat_AsDouble(current);
        RandPatProbs[i] = val;
    }
    int ret = func_analis_pattern_data_1(NWords, WordLen, RandPatProbs);
    if (RandPatProbs) {
        free(RandPatProbs);
    }
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_analis_pattern_data_2_3(int WordLen, 
                                           int NFootPrints, char **FootPrints,
                                           double** PssmMas, double Thr); */
/* function func_analis_pattern_data_2_3(WordLen: int,
                                         FootPrints: [string],
                                         PssmMas: [[float]],
                                         Thr: float) : int */
extern PyObject* py_func_analis_pattern_data_2_3(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"WordLen", "FootPrints", "PssmMas", "Thr", NULL };
    int WordLen = 0;
    int NFootPrints = 0;
    char **FootPrints = 0;
    double **PssmMas = 0;
    double Thr = 0.0;
    PyObject * l1 = 0;
    PyObject * l2 = 0;
    PyObject * l2l = 0;
    int l1_size = 0;
    int l2_size = 0;
    int l2l_size = 0;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "iOOd", kwlist, &WordLen, &l1, &l2, &Thr))
        return NULL;
    NFootPrints = l1_size = PyList_Size(l1);
    FootPrints = (char**)calloc(l1_size, sizeof(char*));
    l2_size = PyList_Size(l2);
    assert(WordLen==l2_size);
    PssmMas = (double**)calloc(l2_size, sizeof(double**));
    int i = 0;
    int j = 0;
    PyObject * current = 0;
    char *val1 = 0;
    double val2 = 0.0;
    for (i=0; i<l1_size; i++) {
        current = PyList_GetItem(l1, i);
        val1 = PyString_AsString(current);
        FootPrints[i] = val1;
    }
    for (i=0; i<l2_size; i++) {
        l2l = PyList_GetItem(l2, i);
        l2l_size = PyList_Size(l2l);
        PssmMas[i] = (double*)calloc(l2l_size, sizeof(double));
        for (j=0; j<l2l_size; j++) {
            current = PyList_GetItem(l2l, j);
            val2 = PyFloat_AsDouble(current);
            PssmMas[i][j] = val2;
        }
    }
    int ret = func_analis_pattern_data_2_3(WordLen, NFootPrints, FootPrints, PssmMas, Thr);
    for (i=0; i<l2_size; i++) {
        free(PssmMas[i]);
    }
    free(PssmMas);
    free(FootPrints);
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_analis_pattern_data_4(char* motif, int Nreplace, 
                                         int NConstPositions, 
                                         int *ConstPositions); */
/* function func_analis_pattern_data_4(motif: string, 
                                       Nreplace: int, 
                                       ConstPositions: [int]): int */
extern PyObject* py_func_analis_pattern_data_4(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"motif", "Nreplace", "ConstPositions", NULL };
    char * motif = 0;
    int Nreplace = 0;
    int NConstPositions = 0;
    int * ConstPositions = 0;
    PyObject * l = 0;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "siO", kwlist, &motif, &Nreplace, &l))
        return NULL;
    NConstPositions = PyList_Size(l);
    ConstPositions = (int*)calloc(NConstPositions, sizeof(int));
    int i = 0;
    PyObject * current = 0;
    int val = 0;
    for (i=0; i<NConstPositions; i++) {
        current = PyList_GetItem(l, i);
        val = PyInt_AsLong(current);
        ConstPositions[i] = val;
    }
    int ret = func_analis_pattern_data_4(motif, Nreplace, NConstPositions, ConstPositions);
    free(ConstPositions);
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_analis_pattern_data_5(char *consensus, 
                                         int NSymbols, char **ConsAlp); */
/* function func_analis_pattern_data_5(consensus: string, ConsAlp: [string]): int */
extern PyObject* py_func_analis_pattern_data_5(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    static char *kwlist[] = {"consensus", "ConsAlp", NULL };
    char * consensus = 0;
    int NSymbols = 0;
    char **ConsAlp = 0;
    PyObject * l = 0;
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sO", kwlist, &consensus, &l))
        return NULL;
    NSymbols = PyList_Size(l);
    ConsAlp = (char**)calloc(NSymbols, sizeof(char*));
    int i = 0;
    PyObject * current = 0;
    char * val = 0;
    for (i=0; i<NSymbols; i++) {
        current = PyList_GetItem(l, i);
        val = PyString_AsString(current);
        ConsAlp[i] = val;
    }
    int ret = func_analis_pattern_data_5(consensus, NSymbols, ConsAlp);
    free(ConsAlp);
    PyObject* result = Py_BuildValue("i", ret);
    return result;
}

/* extern int func_main(double* pvalue); */
/* function func_main(): int, float */
extern PyObject* py_func_main(PyObject *self, PyObject *args, PyObject *keywds)
{
    self = self;
    args = args;
    keywds = keywds;
    double pvalue = 0.0;
    char * report = 0;
    char ** resWords = 0;
    int nWords = 0;
    int ret = func_main(&pvalue, &report, &resWords, &nWords);
    PyObject* l = PyList_New(0);
    PyObject* current = 0;
    int i = 0;
    for (i=0; i<nWords; i++) {
	current = Py_BuildValue("s", resWords[i]);
	//printf("aaa: %s\n", resWords[i]);
	PyList_Append(l, current);
	free(resWords[i]);
    }
    free(resWords);
    PyObject* result = Py_BuildValue("idsO", ret, pvalue, report, l);
    return result;
}

static PyMethodDef sf_methods[] = {
    {"set_input_data",         (PyCFunction)py_func_set_input_data,        METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_alp_bern_data",   (PyCFunction)py_func_analis_alp_bern_data,  METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_alp_mark_data",   (PyCFunction)py_func_analis_alp_mark_data,  METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_alp_dhhm_data",   (PyCFunction)py_func_analis_alp_dhhm_data,  METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_alp_hhm_data",    (PyCFunction)py_func_analis_alp_hhm_data,   METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_pattern_data_0",  (PyCFunction)py_func_analis_pattern_data_0, METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_pattern_data_1",  (PyCFunction)py_func_analis_pattern_data_1, METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_pattern_data_2_3",(PyCFunction)py_func_analis_pattern_data_2_3,METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_pattern_data_4",  (PyCFunction)py_func_analis_pattern_data_4, METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"analis_pattern_data_5",  (PyCFunction)py_func_analis_pattern_data_5, METH_VARARGS|METH_KEYWORDS, "!!! Comment !!!"},
    {"main",  (PyCFunction)py_func_main, 0, "!!! Comment !!!"},
    {NULL, NULL, 0, NULL}
};

extern void initsf() {
    Py_InitModule("sf", sf_methods);
}
