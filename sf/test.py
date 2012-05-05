#!/usr/bin/env python
##############################################
def read_alp_Bern(f):
    f_lines = f.read().split("\n")
    f_data = filter(lambda x: ( not x.startswith("!") ) and len(x)>0, f_lines)
    order = int(f_data[0])
    alpmas = f_data[1].replace(' ', '')
    bernprob = map(lambda x: float(x), f_data[2].split())
    return order,alpmas,bernprob

def read_alp_Mark(f):
    f_lines = f.read().split("\n")
    f_data = filter(lambda x: ( not x.startswith("!") ) and len(x)>0, f_lines)
    order = int(f_data[0])
    alpmas = f_data[1].replace(' ', '')
    markovprob = map(lambda i: map(lambda x: float(x), f_data[i].split()),range(2,len(f_data)))
    #markovprob = map(lambda i: map(lambda j: mas[j][i], range(len(mas))),range(len(mas[0])))
    return order,alpmas,markovprob

def read_alp_HHM(f):
    f_lines = f.read().split("\n")
    f_data = filter(lambda x: ( not x.startswith("!") )and (not x.startswith('#')) and len(x)>0, f_lines)
    order = int(f_data[0])
    alpmas = f_data[1].replace(' ', '')
    nstates = int(f_data[2])
    hhmprob = []
    ind = 3
    for i in range(nstates):
        mas = map(lambda j: map(lambda x: float(x), f_data[j].split()),range(ind,ind + nstates))
        hhmprob.append(mas)
        ind += nstates
    return order,alpmas,nstates,hhmprob

def read_alp_DHHM(f):
    f_lines = f.read().split("\n")
    f_data = filter(lambda x: ( not x.startswith("!") )and (not x.startswith('#')) and len(x)>0, f_lines)
    order = int(f_data[0])
    alpmas = f_data[1].replace(' ', '')
    nstates = int(f_data[2])
    dhhmprob = map(lambda i: map(lambda x: float(x), f_data[i].split()),range(3,3+nstates))    
    ind = 3 + nstates
    dhhmtrans = map(lambda i: map(lambda x: int(x), f_data[i].split()),range(ind,ind + nstates))
    return order,alpmas,nstates,dhhmprob,dhhmtrans
###########################################

def read_pat_0(f):
    f_lines = f.read().split("\n")
    wordslist = filter(lambda x: ( not x.startswith("!") ) and len(x)>0, f_lines)
    return wordslist

def read_pat_2(f):
    f_lines = f.read().split("\n")
    f_data = filter(lambda x: ( not x.startswith("!") ) and len(x)>0, f_lines)
    pssmmas = map(lambda i: map(lambda x: float(x), f_data[i].split()),range(1,len(f_data)))
    wordlen = len(pssmmas)
    return wordlen,pssmmas

def func_test(noccur,tlen,mode,patparam,order,alpfile):
    import sf
    sf.set_input_data(order,mode,tlen,noccur)
    f = open(alpfile,"r")
    if order == 0:
        order,alpmas,bernprob = read_alp_Bern(f)
        print bernprob
        sf.analis_alp_bern_data(alpmas, bernprob)
    elif order > 0:
        order,alpmas,markovprob = read_alp_Mark(f)
        print order
        print alpmas
        print "Probs"
        for i in range(len(markovprob)):
            print markovprob[i]
        sf.analis_alp_mark_data(alpmas,markovprob,[0.3,0.3,0.2,0.2])
    elif order == -1:
        order,alpmas,nstates,dhhmprob,dhhmtrans = read_alp_DHHM(f)
        print nstates
        print "Probs"
        for i in range(nstates):
            print dhhmprob[i]
        print "Trans"
        for i in range(nstates):
            print dhhmtrans[i]
        sf.analis_alp_dhhm_data(alpmas, nstates, dhhmprob, dhhmtrans)
    elif order == -2:
        order,alpmas,nstates,hhmprob = read_alp_HHM(f)
        print order
        print alpmas
        print nstates
        for i in range(nstates):
            print "State",i
            for j in range(nstates):
                print hhmprob[i][j]
        sf.analis_alp_hhm_data(alpmas,nstates,hhmprob)

    f.close()
    param = patparam.split()
    print param
    if mode == 0:
        f = open(patparam,"r")
        words = read_pat_0(f)
        f.close()
        sf.analis_pattern_data_0(words)
    elif mode == 1:
        nwords = int(param[0])
        wordlen = int(param[1])
        sf.analis_pattern_data_1(nwords, wordlen, [])
    elif mode == 2:
        f = open(param[0],"r")
        wordlen,pssmmas = read_pat_2(f)
        f.close()
        print wordlen
        for i in range(wordlen):
            print pssmmas[i]
        thr = float(param[1])
        sf. analis_pattern_data_2_3(wordlen,[],pssmmas,thr)
    elif mode == 4:
        motif = param[0]
        nreplace = int(param[1])
        constpos = map(lambda i: int(param[i]), range(2,len(param)))
        print "ConstPos ",constpos
        sf.analis_pattern_data_4(motif, nreplace, constpos)
    elif mode == 5:
        consensus = patparam
        consalp = [u'SATC', u'LMAGC', u'NACGT']#['NACTG','SAC','LTG','PATG']
        sf.analis_pattern_data_5(consensus, consalp)  
    err,prob,report,words = sf.main()
    print "Prob ",prob
    return prob

#pr = func_test(1,100,0,'Patterns/Words4_4.txt',0,'Alphabets/Bern/Alpb_4_0.txt')
#pr = func_test(1,100,0,'Patterns/Words4_1.txt',0,'Alphabets/Bern/Alpb_4_0.txt')
#pr = func_test(1,100,1,'1 7',0,'Alphabets/Bern/Alpb_4_0.txt')
#pr = func_test(1,100,2,'PSSM/pssm3.txt 5',0,'Alphabets/Bern/Alpb_4_0.txt')
#pr = func_test(1,100,4,'ACTGAT 2 0 5',0,'Alphabets/Bern/Alpb_4_0.txt')
#pr = func_test(1,100,5,'NASSLT',0,'Alphabets/Bern/Alpb_4_0.txt')

#pr = func_test(1,100,0,'Patterns/Words4_4.txt',0,'Alphabets/Bern/Alpb_20_0.txt')
#pr = func_test(1,100,0,'Patterns/Words4_4.txt',0,'Alphabets/Bern/Alpb_20_1.txt')

pr = func_test(1,100,0,'Patterns/Words4_1.txt',1,'Alphabets/Mark/Alpm_4_0_2_1.txt')
#pr = func_test(1,100,0,'Patterns/Words4_4.txt',-1,'Alphabets/DHHM/Alpd_4_0_2_5.txt')
#pr = func_test(1,100,0,'Patterns/Words4_4.txt',-2,'Alphabets/HHM/Alph_4_0_2_5.txt')



