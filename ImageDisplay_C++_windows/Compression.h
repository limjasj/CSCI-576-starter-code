#ifndef COMPRESSION
#define COMPRESSION

#define CRT_SECURE_NO_WARNINGS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "afxwin.h"

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>


using namespace std;

struct Block
{
    int x;
    int y;
    int size;

    std::vector<std::vector<std::vector<int>>> qCoeff;

    Block(int x0, int y0, int N)
    {
        x = x0;
        y = y0;
        size = N;

        qCoeff.resize(3, std::vector<std::vector<int>>(N, std::vector<int>(N)));
    }
};

class Compression
{
    
private:
	class MyImage* image;
    vector<Block*> allBlocks;

public:
	vector<Block*> getAllBlocks() { return allBlocks; }

	void Modify(char* modeCh, char* quantizerCh, char* bitsPerPixelCh, MyImage* thisImage);
	
    void computeDCT(vector<vector<double>>& block, 
        vector<vector<double>>& coeff, int N);
	
    void processBlock(int startX, int startY, int N, int Q);
    void processDynamicBlock(int startX, int startY, int N, int Q, Block* block);
    
    void computeIDCT(vector<vector<double>>& coeff,
        vector<vector<double>>& block, int N);

    void quantizeBlock(vector<vector<double>>& coeff,
        vector<vector<int>>& qcoeff, int N, int Q);

    void inverseQuantizeBlock(vector<vector<int>>& qcoeff,  
        vector<vector<double>>& coeff, int N,int Q);

    void saveDCT(string filename);

	int testBlockSize(int Q, int N);
    int testDynamicBlockSize(int Q, int N);

    int getQcoeffSize(std::vector<std::vector<int>>& qcoeff, int N);

	double findVariance(int startX, int startY, int size);
    void splitBlock(int x, int y, int size);
};

#endif

