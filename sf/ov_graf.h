#pragma once

#ifndef OV_Graf_H
#define OV_Graf_H

#include <string>
#include <vector>
#include <iostream>


//information about an equivalence class H*
typedef struct Clazz
{
	std::vector<std::string> Words;	//set of words from H*
	std::vector<std::string> Lsigns;	//words from the set Back(H*) 
	std::vector<int> States;			//states from EndState(H*)
}Clazz;

class OV_Graf
{
public:
	std::string Lsign;			//sign of the vertex in LOG
	std::string Rsign;
	int iEnd;					//number of a word in the pattern if this vertex is leaf, 0 otherwise
	int Lnum;				//number of this vertex in LOG/ROG	
	int NLChilds;
	int NRChilds;
	OV_Graf** LChilds;			//list of childs in LOG 
	OV_Graf** RChilds;			// List of childs in ROG
	OV_Graf *RParent;			// parent of this vertex in ROG
	OV_Graf *LParent;			// parent of this vertex in LOG
	int len;					
	OV_Graf* ForCopy;
	static int NumOVNodes;		// number of vertexes in the graph
	static int NClasses;		 //number of classes
	static OV_Graf* gOVG;				//root of the graph
	static Clazz* Classes;				//data for all equivalence classes

public:
	OV_Graf(int num1);
	OV_Graf();
	virtual ~OV_Graf();
	static int CreateGraf(OV_Graf* root);
	void MinGraf(void);
	void CreateChilds(void);
	void DeleteLeaves(void);
	void CalPH(double prob);
	void MCalClasses(std::string word);
    void PrintLOG(int z, std::ofstream *ff);
    void PrintROG(int z, std::ofstream *ff);
	virtual void Copy(int m, OV_Graf* G);
	void DebPrint(void);
	void DebPrint1(void);

};

extern std::string* Signs;
#endif /* OV_Graf_H */
