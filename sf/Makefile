CC=gcc 
CFLAGS=-Werror -g  -fPIC
PYTHON_CFLAGS=$(shell python-config --cflags)
PYTHON_LDFLAGS=$(shell python-config --ldflags)
CXX=g++
CXXFLAGS=${CFLAGS}
LINKER=g++
SO_LDFLAGS=-shared

all: first pymodule

first: sf.bin

pymodule: sf.so

sf.bin: ac_trie.o bprob_graf.o d_hhm_prob.o hhm_ovgraf.o h_m_ovgraf.o maindata.o minov_graf.o mmodel_prob.o m_ovgraf.o m_trtree.o nd_hhm_prob.o ov_graf.o sf.o
	${LINKER} -o sf.bin ac_trie.o bprob_graf.o d_hhm_prob.o hhm_ovgraf.o h_m_ovgraf.o maindata.o minov_graf.o mmodel_prob.o m_ovgraf.o m_trtree.o nd_hhm_prob.o ov_graf.o sf.o
	
clean:
	rm -rf *.o
	
ac_trie.o: ac_trie.cpp ac_trie.h maindata.h
	${CXX} -c ${CXXFLAGS} ac_trie.cpp

bprob_graf.o: bprob_graf.cpp bprob_graf.h minov_graf.h
	${CXX} -c ${CXXFLAGS} bprob_graf.cpp

d_hhm_prob.o: d_hhm_prob.cpp d_hhm_prob.h h_m_ovgraf.h
	${CXX} -c ${CXXFLAGS} d_hhm_prob.cpp
	
hhm_ovgraf.o: hhm_ovgraf.cpp hhm_ovgraf.h d_hhm_prob.h nd_hhm_prob.h
	${CXX} -c ${CXXFLAGS} hhm_ovgraf.cpp
	
h_m_ovgraf.o: h_m_ovgraf.cpp h_m_ovgraf.h hhm_ovgraf.h mmodel_prob.h m_trtree.h m_ovgraf.h nd_hhm_prob.h d_hhm_prob.h
	${CXX} -c ${CXXFLAGS} h_m_ovgraf.cpp

maindata.o: maindata.cpp maindata.h ac_trie.h ov_graf.h bprob_graf.h h_m_ovgraf.h 
	${CXX} -c ${CXXFLAGS} maindata.cpp
	
minov_graf.o: minov_graf.cpp maindata.h minov_graf.h m_trtree.h 
	${CXX} -c ${CXXFLAGS} minov_graf.cpp

mmodel_prob.o: mmodel_prob.cpp mmodel_prob.h minov_graf.h m_trtree.h 
	${CXX} -c ${CXXFLAGS} mmodel_prob.cpp
	
m_ovgraf.o: m_ovgraf.cpp m_ovgraf.h mmodel_prob.h m_trtree.h bprob_graf.h 
	${CXX} -c ${CXXFLAGS} m_ovgraf.cpp
	
m_trtree.o: m_trtree.cpp m_trtree.h ac_trie.h mmodel_prob.h h_m_ovgraf.h ov_graf.h 
	${CXX} -c ${CXXFLAGS} m_trtree.cpp
	
nd_hhm_prob.o: nd_hhm_prob.cpp nd_hhm_prob.h h_m_ovgraf.h 
	${CXX} -c ${CXXFLAGS} nd_hhm_prob.cpp
	
ov_graf.o: ov_graf.cpp maindata.h ac_trie.h ov_graf.h minov_graf.h bprob_graf.h m_ovgraf.h hhm_ovgraf.h 
	${CXX} -c ${CXXFLAGS} ov_graf.cpp
	
sf.o: sf.cpp maindata.h ac_trie.h ov_graf.h minov_graf.h bprob_graf.h mmodel_prob.h m_ovgraf.h  m_trtree.h nd_hhm_prob.h d_hhm_prob.h hhm_ovgraf.h 
	${CXX} -c ${CXXFLAGS} sf.cpp

sf_main.o: sf_main.cpp sf_main.h maindata.h ac_trie.h ov_graf.h bprob_graf.h h_m_ovgraf.h mmodel_prob.h m_trtree.h minov_graf.h hhm_ovgraf.h m_ovgraf.h
	${CXX} -c ${CXXFLAGS} sf_main.cpp
	
sfmodule.o: sfmodule.c sf_main.h
	${CC} -c -g -fPIC ${PYTHON_CFLAGS} sfmodule.c
	
sf.so: ac_trie.o bprob_graf.o d_hhm_prob.o hhm_ovgraf.o h_m_ovgraf.o maindata.o minov_graf.o mmodel_prob.o m_ovgraf.o m_trtree.o nd_hhm_prob.o ov_graf.o sf_main.o sfmodule.o
	${LINKER} ${SO_LDFLAGS} ${PYTHON_LDFLAGS} -o sf.so ac_trie.o bprob_graf.o d_hhm_prob.o hhm_ovgraf.o h_m_ovgraf.o maindata.o minov_graf.o mmodel_prob.o m_ovgraf.o m_trtree.o nd_hhm_prob.o ov_graf.o sf_main.o sfmodule.o
