#ifndef QUANTIZATION
#define QUANTIZATION

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

class Quantization 
{
private :
	class MyImage* thisImage;

public:

	 bool Modify(char* scaleCh, char* quantizationCh, char* modeCh, char* extraCh, MyImage* thisImage);
	 int AverageKernel(int color, int oldX, int oldY, int oldWidth, int oldHeight, MyImage* thisImage);
	 void LinearQuantization(int quantization, MyImage* thisImage);
	 void LinearQuantizationExtra(int quantization, MyImage* thisImage);
	 void LogarithmicQuantization(int quantization, int mode, MyImage* thisImage);
	 void LogarithmicQuantizationExtra(int quantization, int mode, MyImage* thisImage);
	 void OptimalIntervalQuantization(int quantization, MyImage* thisImage);
	 void OptimalIntervalQuantizationExtra(int quantization, MyImage* thisImage);


};

#endif
