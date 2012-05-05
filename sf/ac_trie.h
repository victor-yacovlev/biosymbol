#pragma once

#ifndef AC_Trie_H
#define AC_Trie_H

//#include "stdafx.h"
#include "maindata.h"

class OV_Graf;
class AC_Trie;

class AC_Trie
{
public:
	int sign;					 //number of symbol int the alphabet which corresponds to the vertex
	int iEnd;			 //number of word in pattern if this vertex is leaf, 0 otherwise
	int lp;			 // depth of the vertex
	int num;			 // number of the vertex
	int main;		   	// if this vertex is main then main is 1 else 0;  
	AC_Trie **Childs;    // childs
	AC_Trie *Parent;			// link to the parent in the prefix tree
	AC_Trie *nv;				// nv - link to maximal suffix-prefix
	OV_Graf* toLOG;			    // link to the same vertex in Overlap graph 
	int Rnum;
	static int NumACNodes;	// number of current new vertex  in the trie 
	static AC_Trie* gTrie;			//root of the trie
private:
	void MainNode();
	void MarkMainNodes();
public:
	AC_Trie();
	~AC_Trie();
	int InsertWord(int* Word, int Number);	//adds word in AC trie
	static void CreateTrie();					//creates the trie
	void Path(AC_Trie* &T, std::string* word);	
	void PrintWay(ostream *ff);
	void PrintTrie(ostream *ff);
	void Deb_Print(ofstream* ff);
};
#endif
