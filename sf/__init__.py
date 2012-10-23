#!/usr/bin/env python

import sf

"""

Pattern and description of probability distribution must be passed as a 
dictionary, containing at least one key - "type" (string). Another keys are
specific for pattern of description of probability distribution.

1. Pattern description

1.1 A list of words:
{
    "type": "list",
    "words": [ "word1", "word2", ..., "wordN" ]
}   

1.2 Random words
{
    "type": "random",
    "word_length": length_of_random_generated_words,
    "word_count": count_of_random_generated_words,
    "probs": [ prob1, prob2, ..., probN ]
        # prob1...probN - probabilities of letters in given alphabet of size N
        # Summ of probabilities must be 1.0
}

1.3 PSSM and cut-off
{
    "type": "pssm/cutoff",
    "pssm": [ [ value11, ..., value1N ], ..., [ valueM1, ..., valueMN ] ],
    "cutoff": cut_off_value
}

1.4 PSSM and footprints
{
    "type": "pssm/footprints",
    "pssm": [ [ value11, ..., value1N ], ..., [ valueM1, ..., valueMN ] ],
    "footprints": [ "word1", ..., "wordN" ]
}

1.5 Word and mismatches
{
    "type": "word/mismatches",
    "word": "aword",
    "mismatches": max_number_of_mismatches,
    "constpositions": [ pos1, ..., posN ]
}

1.6 Consensus
{
    "type": "consensus",
    "alphabet": {
        "aword1": [ "bword11", "bword12", ..., "bword1X" ],
        "aword2": [ "bword21", "bword22", ..., "bword2Y" ],
        ...,
        "awordN": [ "bwordN1", "bwordN2", ..., "bwordNZ" ]
    },
    "word": "cword"
}

------------------------

2. Probability description

2.1 Bernoulli model
{
    "type": "bernoulli",
    "probs": [ prob1, prob2, ..., probN ]
}

2.2 Markov model
{
    "type": "markov",
    "order": order_of_model,
    "probs": [ [prob11, ..., prob1N ], ..., [ probK1, ..., probKN ] ],
    "initial_probs": [ prob1, ..., probN ]
}

2.3 Hidden Markov model
{
    "type": "hmm",
    "states": number_of_states,
    "probs": [
        [ [ prob111, ..., prob11N ], ..., [prob1M1, ..., prob1MN ] ],
        ...,
        [ [ probS11, ..., probS1N ], ..., [probSM1, ..., probSMN ] ]
    ]
}

2.4 Determenistic Hidden Markov Model
{
    "type": "dhmm",
    "states": number_of_states,
    "probs": [ [prob11, ..., prob1N ], ..., [ probK1, ..., probKN ] ],
    "trans": [ [prob11, ..., prob1N ], ..., [ probK1, ..., probKN ] ]
}

"""

__ERRORS__ = {
    1: "Wrong number of parameters associated with the alphabet",
    2: "Error of opening of file with alphabet description",
    3: "Invalid value of model order",
    4: "Incorrect distribution of letters. Frequences of letters must be real numbers",
    5: "Invalid distribution. Number of frequences must be equal to the size of alphabet",
    6: "Invalid distribution. Incorrect size of matrix with frequences of letters",
    7: "Incorrect alphabet distribution",
    8: "Invalid distribution. Incorrect size of matrix with transition states",
    9: "Wrong number of parameters associated with the pattern",
    10: "Incorrect parameters associated with the pattern",
    11: "Error of opening of file with pattern description",
    12: "Incorrect words in the pattern. Letters of words must be in the given alphabet",
    13: "Incorrect length of the words in the pattern",
    14: "Error of opening of file with PSSM",
    15: "Error of opening of file with footprints",
    16: "Error of opening of file with consensus alphabet description",
    17: "Incorrect description of consensus alphabet",
    18: "Incorrect letters of the consensus",
    19: "Wrong number of parameters associated with text length or number of occurences",
    20: "Incorrect parameters associated with text length or number of occurences",
    21: "Wrong number of parameters associated with the name of output file",
    22: "Wrong number of probabilities associated with Bernoulli probabilities",
    23: "Incorrect number of words in the pattern",
    24: "Incorrect length of the pattern",
    25: "Incorrect size of the alphabet",
    26: "Incorrect order of the probability model",
    27: "Incorrect number of states of the HMM",
    28: "Incorrect size of the alphabet",
    29: "The number of words must be smaller then  alphabet size in power of the pattern length",
    30: "Incorrect PSSM description",
    31: "Some error was caused on the previous stages",
    32: "Incorrect using of the function. This function can not be used with this probability model",
    33: "Incorrect using of the function. This function can non be used for such pattern description",
    34: "Incorrect distribution of letters. Sum of the frequences of letters must be equal to 1",
    35: "Incorrect consensus alphabet. Some letters contain duplicate symbols",
    36: "Redefinition of a letter from consensus alphabet",
    37: "Incorrect consensus alphabet. Some letters contain symbols out of the alphabet",
    38: "Size of the alphabet must be less than 100",
    39: "Length of the pattern must be less than 2000",
    40: "Desired number of occurences must be less than 1000",
    41: "The pattern length has to be equal or bigger then order of the Markovian model",
    42: "Error in the pattern description. Number of mismatches can not be bigger than pattern length minus number of constant positions"
}

def __error_text__(no):
    if no==0: return ""
    elif __ERRORS__.has_key(no): return __ERRORS__[no]
    else: return "Error: "+str(no)
        
import sys

def calculate(A, n, p, H, dist):
    """
    A: Alphabet (unicode string)
    n: length of random text (int)
    p: desired number of occurences of pattern words (int)
    H: pattern (dict - see above)
    dist: description of probability distribution (dict - see above)
    """

    if H["type"]=="list": mode = 0
    elif H["type"]=="random": mode = 1
    elif H["type"]=="pssm/cutoff": mode = 2
    elif H["type"]=="pssm/footprints": mode = 3
    elif H["type"]=="word/mismatches": mode = 4
    elif H["type"]=="consensus": mode = 5
    
    if dist["type"]=="markov": order = int(dist["order"])
    elif dist["type"]=="bernoulli": order = 0
    elif dist["type"]=="dhmm": order = -1
    elif dist["type"]=="hmm": order = -2
    #sys.stderr.write("Order: "+str(order)+"\n"+\
    #    "Mode: "+str(mode)+"\n"+\
    #    "n: "+str(n)+"\n"+\
    #    "p: "+str(p)+"\n")
    ret = sf.set_input_data(order, mode, n, p)
    if ret!=0:
        return __error_text__(ret), 0.0, "", []
    
    
    if dist["type"]=="markov":
        ret = sf.analis_alp_mark_data(A, dist["probs"], dist["initial_probs"])
    elif dist["type"]=="bernoulli":
        ret = sf.analis_alp_bern_data(A, dist["probs"])
    elif dist["type"]=="dhmm":
        ret = sf.analis_alp_dhhm_data(A, len(dist["probs"]), dist["probs"], dist["trans"])
    elif dist["type"]=="hmm":
        ret = sf.analis_alp_hhm_data(A, len(dist["probs"]), dist["probs"])
    
    if ret!=0:
        return __error_text__(ret), 0.0, "", []
        
    if H["type"]=="list":
        ret = sf.analis_pattern_data_0(H["words"])
    elif H["type"]=="random":
        ret = sf.analis_pattern_data_1(H["word_count"], H["word_length"], H["probs"])
    elif H["type"].startswith("pssm"):
        if H["type"]=="pssm/footprints":
            footprints = H["footprints"]
            cutoff = 0.0
        else:
            footprints = []
            cutoff = H["cutoff"]
        ret = sf.analis_pattern_data_2_3(len(H["pssm"]), footprints, H["pssm"], cutoff)
    elif H["type"]=="word/mismatches":
        ret = sf.analis_pattern_data_4(H["word"], int(H["mismatches"]), H["constpositions"])
    elif H["type"]=="consensus":
        ret = sf.analis_pattern_data_5(H["word"], H["alphabet"])
        
    if ret!=0:
        return __error_text__(ret), 0.0, "", []
    
    err_no, result, report, res_words = sf.main()
    return __error_text__(err_no), result, report, res_words
    