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


    if (((quantizer == -1) && (bitsPerPixel == -1)) ||
        ((quantizer != -1) && (bitsPerPixel != -1)))
    {
        //only one of quantizer or bitsPerPizel should be provided
        return; 
    }

    if (quantizer == -1)
    {
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
                // too big, increase compression
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


    if (mode == 1)
    {
        for (int y = 0; y < image->Height; y += 8)
        {
            for (int x = 0; x < image->Width; x += 8)
            {
                processBlock(x, y, 8, quantizer);
            }
        }
    }
    else if (mode == 2)
    {
        //n = 2, 4, 8, 16, 32
        for (int y = 0; y < image->Height; y += 32)
        {
            for (int x = 0; x < image->Width; x += 32)
            {
                splitBlock(x, y, 32);
            }
        }

        for (int i = 0; i < allBlocks.size(); i++)
        {
            Block* block = allBlocks[i];
            processDynamicBlock(block->x, block->y, block->size, quantizer, block);
        }
    }

    // write to file
    saveDCT("InputImage.DCT");

}

void Compression::splitBlock(int x, int y, int size)
{
	Block* block = new Block(x, y, size);

    if (size <= 2)
    {
        allBlocks.push_back(block);
        return;
    }

    double var = findVariance(x, y, size);

    if (var < 2200)   // threshold
    {
        allBlocks.push_back(block);
        return;
    }

    int half = size / 2;

    splitBlock(x, y, half);
    splitBlock(x + half, y, half);
    splitBlock(x, y + half, half);
    splitBlock(x + half, y + half, half);
}

int Compression::testBlockSize(int Q, int N)
{
    //whole image, not block

    int totalBits = 0;
    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (int y = 0; y < image->Height; y += N)
    {
        for (int x = 0; x < image->Width; x += N)
        {
            //for (int i = 0; i < 10; i++)
            {
                for (int c = 0; c < 3; c++)
                {
                    vector<vector<double>> block(N, vector<double>(N));
                    vector<vector<double>> coeff(N, vector<double>(N));
                    vector<vector<int>> qcoeff(N, vector<int>(N));

                    // extract block
                    for (int v = 0; v < N; v++) {
                        for (int u = 0; u < N; u++) {
                            block[u][v] = image->getPixel(x +u, y+v, c);
                        }
                    }

                    // DCT
                    computeDCT(block, coeff, N);

                    // quantize
                    quantizeBlock(coeff, qcoeff, N, Q);

                    int qcoeffSize = getQcoeffSize(qcoeff, N);
                    totalBits += qcoeffSize * N;
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
    int size = N;

    Block* myBlock = new Block(startX, startY, size);
    myBlock->size = size;

    for (int c = 0; c < 3; c++)
    {
        //cout << "color: " << c << endl;

        vector<vector<double>> block(size, vector<double>(size));
        vector<vector<double>> coeff(size, vector<double>(size));
        vector<vector<int>> qcoeff(size, vector<int>(size));

        // extract block
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                block[x][y] = image->getPixel(startX + x, startY + y, c);
            }
        }

        // DCT
        computeDCT(block, coeff, size);

        // quantize
        quantizeBlock(coeff, qcoeff, size, Q);

        // inverse quantize
        inverseQuantizeBlock(qcoeff, coeff, size, Q);

        // IDCT
        computeIDCT(coeff, block, size);

        // write pixels back
        for (int x = 0; x < size; x++)
        {
            for (int y = 0; y < size; y++)
            {
                image->setPixel(startX + x, startY + y, c, round(block[x][y]));
            }
        }

		myBlock->qCoeff[c] = qcoeff;
    }

    allBlocks.push_back(myBlock);
}

void Compression::processDynamicBlock(int startX, int startY, int N, int Q, Block* currBlock)
{
    int size = currBlock->size;

    for (int c = 0; c < 3; c++)
    {
        //cout << "color: " << c << endl;

        vector<vector<double>> block(size, vector<double>(size));
        vector<vector<double>> coeff(size, vector<double>(size));
        vector<vector<int>> qcoeff(size, vector<int>(size));

        // extract block
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                block[x][y] = image->getPixel(startX + x, startY + y, c);
            }
        }

        // DCT
        computeDCT(block, coeff, size);

        // quantize
        quantizeBlock(coeff, qcoeff, size, Q);

        // inverse quantize
        inverseQuantizeBlock(qcoeff, coeff, size, Q);

        // IDCT
        computeIDCT(coeff, block, size);

        // write pixels back
        for (int x = 0; x < size; x++)
        {
            for (int y = 0; y < size; y++)
            {
                int val = round(block[x][y]);
                val = max(0, min(255, val));
                image->setPixel(startX + x, startY + y, c, val);
            }
        }

        currBlock->qCoeff[c] = qcoeff;
    }

}

void Compression::computeDCT(std::vector<std::vector<double>>& block,
    std::vector<std::vector<double>>& coeff, int N)
{
    //iterate through each pixel in block
    //for each pixel, calculate the coefficient (from actual value)
	//n = dimension of block 

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


            double Cu = (u == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
            double Cv = (v == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);

            double value = Cu * Cv * sum;
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
                    double Cu = (u == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
                    double Cv = (v == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);

                    sum += Cu * Cv * coeff[u][v] *
                        cos((2 * x + 1) * u * PI / (2 * N)) *
                        cos((2 * y + 1) * v * PI / (2 * N));
                }
            }

            block[x][y] = sum;
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
	//save quantized coefficients to dct file
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

double Compression::findVariance(int startX, int startY, int size)
{
    double avg = 0;
    int count = size * size * 3;

    //find avg
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            for (int i = 0; i < 3; i++) //for each color
            {
                avg += image->getPixel(startX + x, startY + y, i);
            }
        }
    }
    avg /= count;

	//add up how much each pixel value deviates from the avg
    double totalVariance = 0;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            for (int i = 0; i < 3; i++)
            {
                double currPoint = image->getPixel(startX + x, startY + y, i);
                totalVariance += (currPoint - avg) * (currPoint - avg);
            }
        }
    }

    return totalVariance / count;
}