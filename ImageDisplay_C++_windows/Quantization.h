#ifndef QUANTIZATION
#define QUANTIZATION
#include "Image.h"

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

private:
	MyImage *image;

public:

	static bool Modify(char* scaleCh, char* quantizationCh, char* modeCh, char* extraCh, MyImage* thisImage);
	static int AverageKernel(int color, int oldX, int oldY, int oldWidth, int oldHeight, MyImage* thisImage);
	static void LinearQuantization(int quantization, MyImage* thisImage);
	static void LinearQuantizationExtra(int quantization, MyImage* thisImage);
	static void LogarithmicQuantization(int quantization, int mode, MyImage* thisImage);
	static void LogarithmicQuantizationExtra(int quantization, int mode, MyImage* thisImage);
	static void OptimalIntervalQuantization(int quantization, MyImage* thisImage);
	static void OptimalIntervalQuantizationExtra(int quantization, MyImage* thisImage);


};

#endif
