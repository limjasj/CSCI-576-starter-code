#include "Compression.h"

#define NOMINMAX
#define CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "Image.h"
#include <iostream>
#include <random>

#define PI 3.141592653589793

void Compression::Modify(char* modeCh, char* quantizerCh, char* bitsPerPixelCh, MyImage* thisImage)
{
    image = thisImage;
    int mode = std::stof(modeCh);
    int quantizer = std::stoi(quantizerCh);
    float bitsPerPixel = std::stoi(bitsPerPixelCh);
    
    //hard code values
    quantizer = 5;
    bitsPerPixel = -1;

    if (((quantizer == -1) && (bitsPerPixel == -1)) ||
        ((quantizer != -1) && (bitsPerPixel != -1)))
    {
        //only one of quantizer or bitsPerPizel should be provided
        return; 
    }

    if (quantizer == -1)
    {
        //int size = testBlockSize(1, 8);

        //int targetBytes = image->Width * image->Height * bitsPerPixel / 8;
		int targetBitsPerBlock = bitsPerPixel * 64; // 64 pixels in an 8x8 block
		int targetBitsTotal = bitsPerPixel * image->Width * image->Height;

        int low = 1;
        int high = 20;   // or larger if needed
        int bestQ = 1;

        while (low <= high)
        {
            int mid = (low + high) / 2;

            int size = testBlockSize(mid, 8);

            if (size > targetBitsTotal)
            {
                // too big → increase compression
                low = mid + 1;
            }
            else
            {
                bestQ = mid;
                high = mid - 1;
            }
        }
		quantizer = bestQ;
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

	//int blockNum = 3;

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

int Compression::testBlockSize(int Q, int N)
{
    //whole image, not block

    int totalBits = 0;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    //std::uniform_int_distribution<> randWidth(0, image->Width);
    //std::uniform_int_distribution<> randHeight(0, image->Height);

    for (int y = 0; y < image->Height; y += 8)
    {
        for (int x = 0; x < image->Width; x += 8)
        {
            for (int i = 0; i < 10; i++)
            {
                //int randomX = randWidth(gen);
                //int randomY = randHeight(gen);

                for (int c = 0; c < 3; c++)
                {
                    vector<vector<double>> block(N, vector<double>(N));
                    vector<vector<double>> coeff(N, vector<double>(N));
                    vector<vector<int>> qcoeff(N, vector<int>(N));

                    // extract block
                    for (int x = 0; x < N; x++) {
                        for (int y = 0; y < N; y++) {
                            block[x][y] = image->getPixel(x, y, c);
                        }
                    }

                    // DCT
                    computeDCT(block, coeff, N);

                    // quantize
                    quantizeBlock(coeff, qcoeff, N, Q);

                    int qcoeffSize = getQcoeffSize(qcoeff, N);
                    totalBits += qcoeffSize * 8;
                }
            }
        }
    }

	int avg = totalBits ;
    return avg;
    
}

int Compression::getQcoeffSize(std::vector<std::vector<int>>& qcoeff, int N)
{
	int totalCoeff = 0;
    for (int u = 0; u < N; u++)
    {
        for (int v = 0; v < N; v++)
        {
            if(qcoeff[u][v] != 0) {
                totalCoeff++;
			}
        }
    }

	return totalCoeff;
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
        inverseQuantizeBlock(qcoeff, coeff, N, Q);

        // IDCT
        computeIDCT(coeff, block, N);        

        // write pixels back
        for (int x = 0; x < N; x++)
        {
            for (int y = 0; y < N; y++)
            {
                image->setPixel(startX + x, startY + y, c, round(block[x][y]));
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
    //scale down
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
    //scale up
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
                    fwrite(&currQCoeffVal, sizeof(int), 1, f);

                }
            }
        }
    }

    fclose(f);
}