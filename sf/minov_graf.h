#pragma once

#ifndef MINOVGRAF
#define MINOVGRAF

#include "ov_graf.h"

class MinOV_Graf :
	public OV_Graf
{
private:
	int NLeaf;
	int NLOG;
public:
	int NCl;
	static MinOV_Graf* gMinOVG; 
private:
	virtual void MinGraf(void);
public:
	MinOV_Graf();
	virtual ~MinOV_Graf();
	static  void MinimizeGraf(void);
};

#endif
