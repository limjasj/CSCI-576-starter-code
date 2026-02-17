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

#include <cmath>
#include <vector>

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

bool MyImage::Modify(char* scaleCh, char* quantizationCh, char* modeCh, char* extraCh)
{
	float scale = std::stof(scaleCh);
	int quantization = std::stoi(quantizationCh);
	int mode = std::stoi(modeCh);
	int extra = 0;
	if(extraCh != NULL) extra = std::stoi(extraCh);

	//scale
	//scale = 0.8f;
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

			newData[newIndex] = static_cast<unsigned char>(AverageKernel(0, oldX, oldY, oldWidth, oldHeight)); //r
			newData[newIndex + 1] = static_cast<unsigned char>(AverageKernel(1, oldX, oldY, oldWidth, oldHeight)); //g
			newData[newIndex + 2] = static_cast<unsigned char>(AverageKernel(2, oldX, oldY, oldWidth, oldHeight)); //b
		}
	}
	delete[] Data;
	Data = newData;
	Width = newWidth;
	Height = newHeight;

	bool isValidQuantization = (quantization > 0) && (quantization <= 256);

	if (isValidQuantization)
	{
		bool isUniform = (quantization % 3 == 0);
		if (isUniform && extra == 0)
		{
			if (mode == -1)
			{
				LinearQuantization(quantization);
			}
			else if (mode >= 0 && mode <= 255)
			{
				LogarithmicQuantization(quantization, mode);
			}
			else if (mode == 256)
			{
				OptimalIntervalQuantization(quantization);
			}
		}
		else if (extra == 1)
		{
			if (mode == -1)
			{
				LinearQuantizationExtra(quantization);
			}
			else if (mode >= 0 && mode <= 255)
			{
				LogarithmicQuantizationExtra(quantization, mode);
			}
			else if (mode == 256)
			{
				OptimalIntervalQuantizationExtra(quantization);
			}
		}
	}
	
	

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

void MyImage::LinearQuantization(int quantization)
{
	int bitsPerChannel = quantization / 3;
	int levels = 1 << bitsPerChannel; // 2^bitsPerChannel
	float interval = 256.0f / levels;
	double meanAbsError = 0.0f;

	for (int i = 0; i < Width * Height * 3; i++)
	{
		int value = Data[i];
		int index = static_cast<int>(value / interval);
		if (index >= levels) index = levels - 1; // Handle edge case where value is 255, set to max level
		int quantizedValue = static_cast<int>(value / interval) * interval + interval / 2; //index * interval -> middle val

		meanAbsError += std::abs(value - quantizedValue);

		Data[i] = static_cast<unsigned char>(quantizedValue);
	}

	//std::cout <<meanAbsError << std::endl;
}

void MyImage::LogarithmicQuantization(int quantization, int mode)
{
	int bitsPerChannel = quantization / 3;
	int levels = 1 << bitsPerChannel; // 2^bitsPerChannel //when 12, should be 16
	int interval = 256 / levels;
	double meanAbsError = 0.0f;

	for (int i = 0; i < Width * Height * 3; i++)
	{
		float value = static_cast<float>(Data[i]);
		float normalizedVal = value / 255.0f; //range from 0 to 1

		float logVal; //transform into log space
		if (mode == 0) 	logVal = log(1 + normalizedVal); //if mode is 0, don't use mode
		else logVal = log(1 + mode * normalizedVal) / log(1 + mode); //basically a fraction except with log, which is non linear. 1 keeps it from 0. (0-1)

		int index = static_cast<int>(logVal * levels); //which level it belongs to
		if (index >= levels) index = levels - 1;

		float quantizedLog = (index + 0.5f) / levels; //middle value of level, get ratio according to levels. this will be used to get final pixel val

		float reconstructed; //transform into linear space
		if (mode == 0) reconstructed = (exp(quantizedLog * log(1)) - 1);
		else reconstructed = (exp(quantizedLog * log(1 + mode)) - 1) / mode;

		int finalValue = static_cast<int>(reconstructed * 255.0f + 0.5f); //transform into pixel range
		//check bounds
		if (finalValue < 0) finalValue = 0;
		if (finalValue > 255) finalValue = 255;

		//std::cout << "original: " << (int)Data[i] << " finalValue: " << finalValue << " i: "<<i << std::endl;
		meanAbsError += std::abs(value - finalValue);
		Data[i] = static_cast<unsigned char>(finalValue);
	}

	//std::cout  << meanAbsError << std::endl;
}

void MyImage::OptimalIntervalQuantization(int quantization)
{
	int bitsPerChannel = quantization / 3;
	int levels = 1 << bitsPerChannel;
	double meanAbsError = 0.0f;

	for (int color = 0; color < 3; color++)
	{
		//make histogram that counts how many pixels of each color value
		std::vector<int> histogram(256, 0);
		for (int i = color; i < Width * Height * 3; i += 3)
		{
			histogram[Data[i]]++;
		}

		//fill levelCount with the middle of each level
		int interval = 256 / levels;
		std::vector<float> levelCenters(levels, 0.0f);
		for (int i = 0; i < levels; i++)
		{
			levelCenters[i] = (i + 0.5f) * interval;
		}

		bool isStable = true;
		if (isStable)
		{
			isStable = false;
			std::vector<float> newLevelCenters(levels, 0); //new center for each level
			std::vector<float> levelCount(levels, 0); //count how many pixels belong to each level
			//assign each intensity to closest level
			for (int intensity = 0; intensity < 256; intensity++)
			{
				if (histogram[intensity] == 0) continue;

				//find closest level to this intensity
				int bestLevel = 0;
				float minDist = std::fabs(intensity - levelCenters[0]);
				for (int k = 0; k < levels; k++)
				{
					float dist = std::fabs(intensity - levelCenters[k]);
					if (dist < minDist)
					{
						minDist = dist;
						bestLevel = k;
					}
				}

				newLevelCenters[bestLevel] += intensity * histogram[intensity];
				levelCount[bestLevel] += histogram[intensity];
			}

			for (int k = 0; k < levels; k++)
			{
				if (levelCount[k] > 0)
				{
					float newCenter = newLevelCenters[k] / levelCount[k]; //average intensity for the level
					if (abs(newCenter - levelCenters[k]) > 0.5f) isStable = true; //if center passes threshold

					levelCenters[k] = newCenter; //set new level center
				}
			}
		}

		//do quantizatioin
		for (int i = color; i < Width * Height * 3; i+=3)
		{
			int value = Data[i];

			//find best index
			int bestLevel = 0;
			float minDist = std::fabs(value - levelCenters[0]);
			for (int k = 0; k < levels; k++)
			{
				float dist = std::fabs(value - levelCenters[k]);
				if (dist < minDist)
				{
					minDist = dist;
					bestLevel = k;
				}
			}

			int quantizedValue = static_cast<int>(levelCenters[bestLevel] + 0.5); 
			if (quantizedValue < 0) quantizedValue = 0;
			if (quantizedValue > 255) quantizedValue = 255;

			meanAbsError += std::fabs(value - quantizedValue);
			Data[i] = static_cast<unsigned char>(quantizedValue);
		}
	}

	//std::cout << meanAbsError << std::endl;
}

void MyImage::LinearQuantizationExtra(int quantization)
{
	double meanAbsError = 0.0f;
	double bestError = DBL_MAX;
	int bestBitSplit[3] = { 0,0,0 };

	for (int bR = 0; bR <= quantization; bR++)
	{
		for (int bG = 0; bG <= quantization - bR; bG++)
		{
			int bB = quantization - bR - bG;
			int currBitSplit[3] = { bR, bG, bB };
			double totalError = 0.0;

			for (int color = 0; color < 3; color++)
			{
				int bits = currBitSplit[color];
				int levels = exp2(bits);
				float interval = 256.0f / levels;

				for (int i = 0; i < Width * Height; i++)
				{
					int value = Data[i];
					int index = static_cast<int>(value / interval);
					if (index >= levels) index = levels - 1; // Handle edge case where value is 255, set to max level
					int quantizedValue = static_cast<int>(index * interval + interval / 2); //index * interval -> middle val

					totalError += std::fabs(value - quantizedValue);
				}
			}

			//per color bit combo
			if (totalError < bestError)
			{
				bestError = totalError;
				bestBitSplit[0] = bR;
				bestBitSplit[1] = bG;
				bestBitSplit[2] = bB;
			}			
		}
	}

	//do quaantization with best bit split
	for (int color = 0; color < 3; color++)
	{
		int bits = bestBitSplit[color];
		int levels = exp2(bits);
		float interval = 256.0f / levels;
		for (int i = 0; i < Width * Height; i++)
		{
			int value = Data[i * 3 + color];

			int index = static_cast<int>(value / interval);
			if (index >= levels) index = levels - 1; // Handle edge case where value is 255, set to max level
			int quantizedValue = static_cast<int>(index * interval + interval / 2); //index * interval -> middle val

			meanAbsError += std::abs(value - quantizedValue);

			Data[i * 3 + color] = static_cast<unsigned char>(quantizedValue);
		}
	}
	//std::cout << meanAbsError << std::endl;
}

void MyImage::LogarithmicQuantizationExtra(int quantization, int mode)
{
	double meanAbsError = 0.0f;
	double bestError = DBL_MAX;
	int bestBitSplit[3] = { 0,0,0 };

	for (int bR = 0; bR <= quantization; bR++)
	{
		for (int bG = 0; bG <= quantization - bR; bG++)
		{
			int bB = quantization - bR - bG;
			int currBitSplit[3] = { bR, bG, bB };
			double totalError = 0.0;

			for (int i = 0; i < Width * Height; i++)
			{

				for (int color = 0; color < 3; color++)
				{
					int bits = currBitSplit[color];
					int levels = exp2(bits);

					float value = static_cast<float>(Data[i * 3 + color]);
					float normalizedVal = value / 255.0f; //range from 0 to 1

					float logVal; //transform into log space
					if (mode == 0) 	logVal = log(1 + normalizedVal); //if mode is 0, don't use mode
					else logVal = log(1 + mode * normalizedVal) / log(1 + mode); //basically a fraction except with log, which is non linear. 1 keeps it from 0. (0-1)

					int index = static_cast<int>(logVal * levels); //which level it belongs to
					if (index >= levels) index = levels - 1;

					float quantizedLog = (index + 0.5f) / levels; //middle value of level, get ratio according to levels. this will be used to get final pixel val

					float reconstructed; //transform into linear space
					if (mode == 0) reconstructed = (exp(quantizedLog) - 1);
					else reconstructed = (exp(quantizedLog * log(1 + mode)) - 1) / mode;

					int finalValue = static_cast<int>(reconstructed * 255.0f + 0.5f); //transform into pixel range
					//check bounds
					if (finalValue < 0) finalValue = 0;
					if (finalValue > 255) finalValue = 255;

					//std::cout << "original: " << (int)Data[i] << " finalValue: " << finalValue << " i: "<<i << std::endl;
					totalError += std::fabs(value - finalValue);
				}
			}
			//per color bit combo
			if (totalError < bestError)
			{
				bestError = totalError;
				bestBitSplit[0] = bR;
				bestBitSplit[1] = bG;
				bestBitSplit[2] = bB;
			}
		}
	}

	//do quaantization with best bit split
	for (int i = 0; i < Width * Height; i++)
	{
		for (int color = 0; color < 3; color++)
		{
			int bits = bestBitSplit[color];
			int levels = exp2(bits);

			float value = static_cast<float>(Data[i * 3 + color]);
			float normalizedVal = value / 255.0f; //range from 0 to 1

			float logVal; //transform into log space
			if (mode == 0) 	logVal = log(1 + normalizedVal); //if mode is 0, don't use mode
			else logVal = log(1 + mode * normalizedVal) / log(1 + mode); //basically a fraction except with log, which is non linear. 1 keeps it from 0. (0-1)

			int index = static_cast<int>(logVal * levels); //which level it belongs to
			if (index >= levels) index = levels - 1;

			float quantizedLog = (index + 0.5f) / levels; //middle value of level, get ratio according to levels. this will be used to get final pixel val

			float reconstructed; //transform into linear space
			if (mode == 0) reconstructed = (exp(quantizedLog) - 1);
			else reconstructed = (exp(quantizedLog * log(1 + mode)) - 1) / mode;

			int finalValue = static_cast<int>(reconstructed * 255.0f + 0.5f); //transform into pixel range
			//check bounds
			if (finalValue < 0) finalValue = 0;
			if (finalValue > 255) finalValue = 255;

			//std::cout << "original: " << (int)Data[i] << " finalValue: " << finalValue << " i: "<<i << std::endl;
			meanAbsError += std::abs(value - finalValue);
			Data[i * 3 + color] = static_cast<unsigned char>(finalValue);
			
		}
	}

	//std::cout << meanAbsError << std::endl;
}

void MyImage::OptimalIntervalQuantizationExtra(int quantization)
{
	float meanAbsError = 0.0f;

	double bestError = DBL_MAX;
	int bestBitSplit[3] = { 0,0,0 };
	std::vector<float> bestCenters[3];

	//make histogram that counts how many pixels of each color value
	std::vector<int> histogram[3];
	for (int color = 0; color < 3; color++)
	{
		histogram[color] = std::vector<int>(256, 0);
	}
	
	//fill histogram for each color
	for (int i = 0; i < Width * Height; i++)
	{
		for (int color = 0; color < 3; color ++)
		{
			histogram[color][Data[(i * 3) + color]]++;
		}
	}

	// Try all bit splits
	for (int bR = 0; bR <= quantization; bR++)
	{
		for (int bG = 0; bG <= quantization - bR; bG++)
		{
			int bB = quantization - bR - bG;
			int currBitSplit[3] = { bR, bG, bB };
			double totalError = 0.0;
			std::vector<float> currLevelCenters[3];

			for (int color = 0; color < 3; color++)
			{
				int channelBits = currBitSplit[color];
				int levels = exp2( channelBits);

				//fill levelCount with the middle of each level
				int interval = 256 / levels;
				currLevelCenters[color] = std::vector<float>(levels, 0.0f);
				for (int i = 0; i < levels; i++)
				{
					currLevelCenters[color][i] = (i + 0.5f) * interval;
				}

				bool isStable = true;
				if (isStable)
				{
					isStable = false;
					std::vector<float> newLevelCenters(levels, 0); //new center for each level
					std::vector<float> levelCount(levels, 0); //count how many pixels belong to each level
					//assign each intensity to closest level
					for (int intensity = 0; intensity < 256; intensity++)
					{
						if (histogram[color][intensity] == 0) continue;

						//find closest level to this intensity
						int bestLevel = 0;
						float minDist = std::fabs(intensity - currLevelCenters[color][0]);
						for (int k = 0; k < levels; k++)
						{
							float dist = std::fabs(intensity - currLevelCenters[color][k]);
							if (dist < minDist)
							{
								minDist = dist;
								bestLevel = k;
							}
						}

						newLevelCenters[bestLevel] += intensity * histogram[color][intensity];
						levelCount[bestLevel] += histogram[color][intensity];
					}

					for (int k = 0; k < levels; k++)
					{
						if (levelCount[k] > 0)
						{
							float newCenter = newLevelCenters[k] / levelCount[k]; //average intensity for the level
							if (fabs(newCenter - currLevelCenters[color][k]) > 0.5f) isStable = true; //if center passes threshold

							currLevelCenters[color][k] = newCenter; //set new level center
						}
					}
				}

				//per color, in color bit combo
				for (int intensity = 0; intensity < 256; intensity++)
				{
					if (histogram[color][intensity] == 0) continue;

					int bestLevel = 0;
					float minDist = fabs(intensity - currLevelCenters[color][0]);

					for (int k = 1; k < levels; k++)
					{
						float dist = fabs(intensity - currLevelCenters[color][k]);
						if (dist < minDist)
						{
							minDist = dist;
							bestLevel = k;
						}
					}

					float quantized = currLevelCenters[color][bestLevel];
					totalError += histogram[color][intensity] * fabs(intensity - quantized);
				}
			}
			//per color bit combo
			if (totalError < bestError)
			{
				bestError = totalError;
				bestBitSplit[0] = bR;
				bestBitSplit[1] = bG;
				bestBitSplit[2] = bB;

				for (int color = 0; color < 3; color++)
					bestCenters[color] = currLevelCenters[color];
			}
		}
	}

		//do quantizatioin
	for (int i = 0; i < Width * Height; i ++)
	{
		for (int color = 0; color < 3; color++)
		{
			int value = Data[(i * 3) +color];

			//find best index
			int bestLevel = 0;
			int levels = bestCenters[color].size();

			float minDist = std::fabs(value - bestCenters[color][0]);
			for (int k = 0; k < levels; k++)
			{
				float dist = std::fabs(value - bestCenters[color][k]);
				if (dist < minDist)
				{
					minDist = dist;
					bestLevel = k;
				}
			}

			int quantizedValue = static_cast<int>(bestCenters[color][bestLevel] + 0.5);
			if (quantizedValue < 0) quantizedValue = 0;
			if (quantizedValue > 255) quantizedValue = 255;

			meanAbsError += std::fabs(value - quantizedValue);
			Data[i * 3 + color] = static_cast<unsigned char>(quantizedValue);
		}
	}

	//std::cout << meanAbsError << std::endl;
}

