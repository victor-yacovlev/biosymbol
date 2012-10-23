CC=gcc 
CFLAGS=-Werror -g  -fPIC
PYTHON_CFLAGS=$(shell python-config --cflags)
PYTHON_LDFLAGS=$(shell python-config --ldflags)
CXX=g++
CXXFLAGS=${CFLAGS}
LINKER=g++
SO_LDFLAGS=-shared

all: pymodule


pymodule: _rna.so

clean:
	rm -rf *.o


main.o: main.cpp 
	${CXX} -c -g -fPIC ${PYTHON_CFLAGS} main.cpp

_rna.so: main.o
	${LINKER} ${SO_LDFLAGS} ${PYTHON_LDFLAGS} -lboost_python -o _rna.so main.o 
