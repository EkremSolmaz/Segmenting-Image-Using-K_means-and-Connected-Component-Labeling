#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

using namespace std;

int findCentroid(int *x, int **K, int m, int n);
float calculateDistance(int *x, int *k);
int *calculateAverage(int *C, int CSize, int **X, int m, int n);
void print2dArray(int **K, int m);
void print1dArray(int *C, int m);

int main(int argc, char const *argv[])
{

    srand(time(0));

    int n = 42;
    int m = 4;

    // Creating n random pixel variable X[n][3]
    int **X = (int **)malloc(n * sizeof(int *));

    for (int i = 0; i < n; i++)
    {
        X[i] = (int *)malloc(3 * sizeof(int));

        X[i][0] = rand() % 256;
        X[i][1] = rand() % 256;
        X[i][2] = rand() % 256;
    }

    // Creating m random k centroid K[m][3]
    int **K = (int **)malloc(m * sizeof(int *));

    for (int i = 0; i < m; i++)
    {
        K[i] = (int *)malloc(3 * sizeof(int));

        K[i][0] = X[i][0];
        K[i][1] = X[i][1];
        K[i][2] = X[i][2];
    }

    float difference;
    int thresh = 5;

    do
    {
        difference = 0;

        // Creating m array to group pixels in X C[m][?]
        int **C = (int **)malloc(m * sizeof(int *));
        for (int i = 0; i < m; i++)
        {
            C[i] = (int *)malloc(sizeof(int));
        }

        // Creating m size array for sizes of groups CSizes[m]
        int *CSizes = (int *)calloc(m, sizeof(int));

        //Find every pixel's centroid and add it to its group
        for (int i = 0; i < n; i++)
        {
            int cent = findCentroid(X[i], K, m, n);
            CSizes[cent]++;
            C[cent] = (int *)realloc(C[cent], CSizes[cent] * sizeof(int));
            C[cent][CSizes[cent] - 1] = i;
        }
        // for (int i = 0; i < m; i++)
        // {
        //     print1dArray(C[i], CSizes[i]);
        // }
        

        //Creating KNew for new centroid values KNew[m][3]
        int **KNew = (int **)malloc(m * sizeof(int *));

        for (int i = 0; i < m; i++)
        {
            KNew[i] = (int *)malloc(3 * sizeof(int));

            KNew[i] = calculateAverage(C[i], CSizes[i], X, m, n);
        }

        // for (int i = 0; i < m; i++)
        // {
        //     int* avg = calculateAverage(C[i], CSizes[i], X, m, n);
        //     KNew[i] = avg;
        // }

        for (int i = 0; i < m; i++)
        {
            difference += calculateDistance(K[i], KNew[i]);

            K[i] = KNew[i];
        }

        cout << difference << endl;

        print2dArray(K, m);
        // print2dArray(KNew, m);

        std::cout << "==================" << std::endl;

    } while (difference > thresh);

    return 0;
}

int findCentroid(int *x, int **K, int m, int n)
{
    int minDist = INT_MAX;
    int minIdx = -1;

    for (int i = 0; i < m; i++)
    {
        float dst = calculateDistance(x, K[i]);
        if (dst < minDist)
        {
            minDist = dst;
            minIdx = i;
        }
    }

    return minIdx;
}

float calculateDistance(int *x, int *k)
{

    float distance = 0;

    distance += (k[0] - x[0]) * (k[0] - x[0]);
    distance += (k[1] - x[1]) * (k[1] - x[1]);
    distance += (k[2] - x[2]) * (k[2] - x[2]);

    return sqrt(distance);
}

int *calculateAverage(int *C, int CSize, int **X, int m, int n)
{
    int *sum;
    sum = (int *)calloc(3, sizeof(int));

    for (int i = 0; i < CSize; i++)
    {
        sum[0] += X[C[i]][0];
        sum[1] += X[C[i]][1];
        sum[2] += X[C[i]][2];
    }

    sum[0] /= CSize;
    sum[1] /= CSize;
    sum[2] /= CSize;

    return sum;
}

void print2dArray(int **K, int m)
{
    cout << "R\tG\tB" << endl;
    for (int i = 0; i < m; i++)
    {
        cout << K[i][0] << "\t" << K[i][1] << "\t" << K[i][2] << endl;
    }
}

void print1dArray(int *C, int m)
{
    for (int i = 0; i < m; i++)
    {
        cout << C[i] << "\t";
    }
    cout << endl;
    cout << "====================> " << m << endl;
}