//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"
#include <string>
#include <iostream>
#include <algorithm>

#define NOMINMAX
#include <Windows.h>

// Constructor and Desctructors
MyImage::MyImage() 
{
	Data = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
}

MyImage::~MyImage()
{
	if ( Data )
		delete Data;
}


// Copy constructor
MyImage::MyImage( MyImage *otherImage)
{
	Height = otherImage->Height;
	Width  = otherImage->Width;
	Data   = new unsigned char[Width*Height*3];
	strcpy(otherImage->ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage->Data[i];
	}


}



// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
	Height = otherImage.Height;
	Width  = otherImage.Width;
	Data   = new unsigned char[Width*Height*3];
	strcpy( (char *)otherImage.ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}
	
	return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if ( IN_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	unsigned char *Rbuf = new unsigned char[Height*Width];
	unsigned char *Gbuf = new unsigned char[Height*Width];
	unsigned char *Bbuf = new unsigned char[Height*Width];

	for (i = 0; i < Width*Height; i ++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}
	
	// Allocate Data structure and copy
	Data = new unsigned char[Width*Height*3];
	for (i = 0; i < Height*Width; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1]	= Gbuf[i];
		Data[3*i+2]	= Rbuf[i];
	}

	// Clean up and return
	delete[] Rbuf;
	delete[] Gbuf;
	delete[] Bbuf;
	fclose(IN_FILE);

	return true;

}



// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if ( OUT_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	unsigned char *Rbuf = new unsigned char[Height*Width];
	unsigned char *Gbuf = new unsigned char[Height*Width];
	unsigned char *Bbuf = new unsigned char[Height*Width];

	for (i = 0; i < Height*Width; i++)
	{
		Bbuf[i] = Data[3*i];
		Gbuf[i] = Data[3*i+1];
		Rbuf[i] = Data[3*i+2];
	}

	
	// Write data to file
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}
	
	// Clean up and return
	delete[] Rbuf;
	delete[] Gbuf;
	delete[] Bbuf;
	fclose(OUT_FILE);

	return true;

}




// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
bool MyImage::Modify()
{

	// TO DO by student
	
	// sample operation
	for ( int i=0; i<Width*Height; i++ )
	{
		Data[3*i] = 0;
		Data[3*i+1] = 0;

	}

	return false;
}
int roundNum(float num)
{
	return static_cast<int>(num + 0.5f);
}

bool MyImage::Modify(char* scaleCh, char* quantizationCh, char* modeCh)
{


	float scale = std::stof(scaleCh);
	int quantization = std::stoi(quantizationCh);
	int mode = std::stoi(modeCh);

	//scale
	scale = 0.8f;
	int oldWidth = Width ;
	int oldHeight = Height;
	int newWidth = static_cast<int>( oldWidth * scale);
	int newHeight = static_cast<int>(oldHeight * scale);

	int rowSize = newWidth * 3;
	int paddedRowSize = (rowSize + 3) & (~3);

	unsigned char* newData = new unsigned char[paddedRowSize * newHeight];

	for (int y = 0; y < newHeight; y++) //row, y
	{
		for (int x = 0; x < newWidth; x++) //cols, x
		{
			int oldX = static_cast<int>(x / scale);
			int oldY = static_cast<int>(y / scale);

			if (oldX < 0) oldX = 0;
			if (oldY < 0) oldY = 0;
			if (oldX >= oldWidth)  oldX = oldWidth - 1;
			if (oldY >= oldHeight) oldY = oldHeight - 1;

			int oldIndex = (oldY * oldWidth + oldX) * 3;
			int newIndex = y * paddedRowSize + x * 3;


			newData[newIndex] = Data[oldIndex]; //r
			newData[newIndex + 1] = Data[oldIndex + 1]; //g
			newData[newIndex + 2] = Data[oldIndex + 2]; //b

			//std::cout << "oldX: " << oldX << " oldY: " << oldY << " newX: " << x << " newY: " << y << " R: " << (int)newData[newIndex] <<" G: " << (int)newData[newIndex + 1] << " B: " << (int)newData[newIndex + 2] << std::endl;
		}
	}
	delete[] Data;
	Data = newData;
	Width = newWidth;
	Height = newHeight;


	return 0;
}



int MyImage::AverageKernel(int color, int oldX, int oldY, int oldWidth, int oldHeight)
{
	int sum = 0;
	int count = 0;
	for(int y = -1; y<= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int newX = oldX + (x);
			int newY = oldY + (y);
			if (newX <0 || newY <0 || newX >= oldWidth || newY >= oldHeight) 
				continue;

			int index = (newY * oldWidth + newX) * 3 + color;
			int newNum = (int)Data[index];
			sum += newNum;
			count++;
			//std::cout << newNum << " ";
		}
	
	}
	int avg = sum / count;
	//std::cout << "sum: " <<sum << " avg: " << avg<< std::endl;

	return avg;
}
