#include "Compression.h"

#define NOMINMAX
#define CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "Image.h"
#include <iostream>

#define PI 3.141592653589793

void Compression::Modify(char* modeCh, char* quantizerCh, char* bitsPerPixelCh, MyImage* thisImage)
{
    image = thisImage;
    int mode = std::stof(modeCh);
    int quantizer = std::stoi(quantizerCh);
    float bitsPerPixel = std::stoi(bitsPerPixelCh);
    
    //hard code values
    quantizer = 6;
    bitsPerPixel = -1;

    if (((quantizer == -1) && (bitsPerPixel == -1)) ||
        ((quantizer != -1) && (bitsPerPixel != -1)))
    {
        //only one of quantizer or bitsPerPizel should be provided
        return; 
    }

    if (quantizer == -1)
    {
        //calculate quantizer from bitsPerPixel
        int totalPixels = image->Width * image->Height;
        int totalBits = static_cast<int>(bitsPerPixel * totalPixels);
        int bitsPerBlock = 64; // 8x8 block has 64 coefficients
        int blocksPerImage = (totalPixels + 63) / 64; // round up to nearest block
		quantizer = totalBits / (blocksPerImage * bitsPerBlock);
    }

    //input IMAGE
    //    ↓
    //    Divide image into blocks
    //    ↓
    //    DCT transform per block
    //    ↓
    //    Quantize DCT coefficients
    //    ↓
    //    Save quantized coefficients to.DCT file
    //    ↓
    //    Inverse Quantization
    //    ↓
    //    Inverse DCT
    //    ↓
    //    Reconstruct image
    //    ↓
    //    Display image
    
    //mode ==1
    //iterate through 8x8 blocks

	int blockNum = 3;
    //processBlock(0, 0, 8, quantizer);

    for (int y = 0; y < image->Height; y += 8)
    {
        for (int x = 0; x < image->Width; x += 8)
        {
            //cout << "blockNum: " << blockNum << endl;
            //blockNum++;
            processBlock(x, y, 8, quantizer);
        }
    }

    // write to file
    saveDCT("InputImage.DCT");

}


void Compression::processBlock(int startX, int startY, int N, int Q)
{
    Block* myBlock = new Block(startX, startY, N);

    for (int c = 0; c < 3; c++)
    {
        //cout << "color: " << c << endl;

        vector<vector<double>> block(N, vector<double>(N));
        vector<vector<double>> coeff(N, vector<double>(N));
        vector<vector<int>> qcoeff(N, vector<int>(N));
        vector<vector<double>> newCoeff(N, vector<double>(N));
        vector<vector<double>> newBlock(N, vector<double>(N));

        // extract block
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                block[x][y] = image->getPixel(startX + x, startY + y, c);
            }
        }

        // DCT
        computeDCT(block, coeff, N);

        // quantize
        quantizeBlock(coeff, qcoeff, N, Q);

        // inverse quantize
        inverseQuantizeBlock(qcoeff, newCoeff, N, Q);

        // IDCT
        computeIDCT(newCoeff, newBlock, N);

        

        // write pixels back
        for (int x = 0; x < N; x++)
        {
            for (int y = 0; y < N; y++)
            {
                image->setPixel(startX + x, startY + y, c, round(newBlock[x][y]));
            }
        }

		myBlock->qCoeff[c] = qcoeff;
    }

    allBlocks.push_back(myBlock);

}

void Compression::computeDCT(std::vector<std::vector<double>>& block,
    std::vector<std::vector<double>>& coeff, int N)
{
    //iterate through each pixel in block
    //for each pixel, calculate the coefficient (from actual value)
	//n = dimension of block (eg 8 for 8x8)

    for (int u = 0; u < N; u++)
    {
        for (int v = 0; v < N; v++)
        {
            double sum = 0.0;

            for (int x = 0; x < N; x++)
            {
                for (int y = 0; y < N; y++)
                {
                    double pixel = block[x][y];

                    sum += pixel *
                        cos((2 * x + 1) * u * PI / (2 * N)) *
                        cos((2 * y + 1) * v * PI / (2 * N));
                }
            }


            double Cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
            double Cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;

			double value = 0.25 * Cu * Cv * sum;
            coeff[u][v] = value;
        }
    }
}

void Compression::computeIDCT(std::vector<std::vector<double>>& coeff,
    std::vector<std::vector<double>>& block, int N)
{
	//iterate through each pixel in block
    //for each pixel, calculate the block value (from coeffecient value)

    for (int x = 0; x < N; x++)
    {
        for (int y = 0; y < N; y++)
        {
            double sum = 0;

            for (int u = 0; u < N; u++)
            {
                for (int v = 0; v < N; v++)
                {
                    double Cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
                    double Cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;

                    sum += Cu * Cv * coeff[u][v] *
                        cos((2 * x + 1) * u * PI / (2 * N)) *
                        cos((2 * y + 1) * v * PI / (2 * N));
                }
            }

            block[x][y] = 0.25 * sum;
        }
    }
}

void Compression::quantizeBlock(std::vector<std::vector<double>>& coeff,
    std::vector<std::vector<int>>& qcoeff, int N, int Q)
{
    double scale = pow(2.0, Q);

    for (int u = 0; u < N; u++)
    {
        for (int v = 0; v < N; v++)
        {
			int value = round(coeff[u][v] / scale);
            qcoeff[u][v] = value;
        }
    }
}

void Compression::inverseQuantizeBlock(std::vector<std::vector<int>>& qcoeff,
    std::vector<std::vector<double>>& coeff, int N, int Q)
{
    double scale = pow(2.0, Q);

    for (int u = 0; u < N; u++)
    {
        for (int v = 0; v < N; v++)
        {
            coeff[u][v] = qcoeff[u][v] * scale;
        }
    }
}

void Compression::saveDCT(string filename)
{
	//save quantized coefficients to .DCT file
    //for each color for each block

    FILE* f = fopen(filename.c_str(), "wb");

    for (auto& currBlock : allBlocks)
    {
        for (auto& currColor : currBlock->qCoeff)
        {
            for (auto currQCoeff : currColor)
            {
                for (auto currQCoeffVal : currQCoeff)
                {
                    //int N = qcoeff.size;
                    fwrite(&currQCoeffVal, sizeof(int), 1, f);

                    //cout << currQCoeff << " ";
                    /*for (int c = 0; c < 3; c++)
                    {
                        for (int i = 0; i < N * N; i++)
                        {
                            fwrite(&qcoeff[c][i], sizeof(int), 1, f);
                        }
                    }*/
                }
            }
        }
    }

    //Wcout <<  " "<<endl;

    fclose(f);
}