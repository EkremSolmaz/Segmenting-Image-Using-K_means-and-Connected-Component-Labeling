#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <limits.h>
#include <time.h>

int findCentroid(int *x, int **K, int m, int n);
float calculateDistance(int *x, int *k);
int *calculateAverage(int *C, int CSize, int **X, int m, int n);
void print2dArray(int **K, int m);
void print1dArray(int *C, int m);

int main()
{
    CvMat *img = cvLoadImageM("peppers_color.jpg", CV_LOAD_IMAGE_UNCHANGED);

    int nrow = img->rows;
    int ncol = img->cols;

    printf("%d - %d\n", nrow, ncol);

    int **X = (int **)malloc(nrow * ncol * sizeof(int *));
    for (int i = 0; i < nrow; i++)
    {
        for (int j = 0; j < ncol; j++)
        {
            X[i * ncol + j] = (int *)malloc(3 * sizeof(int));

            X[i * ncol + j][0] = cvGet2D(img, i, j).val[0];
            X[i * ncol + j][1] = cvGet2D(img, i, j).val[1];
            X[i * ncol + j][2] = cvGet2D(img, i, j).val[2];
        }
    }

    int n = img->rows * img->cols;
    int m = 4;

    printf("%d\n", n);

    int **K = (int **)malloc(m * sizeof(int *));

    for (int i = 0; i < m; i++)
    {
        K[i] = (int *)malloc(3 * sizeof(int));

        // K[i][0] = X[i][0];
        // K[i][1] = X[i][1];
        // K[i][2] = X[i][2];

        K[i][0] = rand() % 256;
        K[i][1] = rand() % 256;
        K[i][2] = rand() % 256;
    }

    float difference;
    int thresh = 5;
    print2dArray(K, m);
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

        for (int i = 0; i < m; i++)
        {
            difference += calculateDistance(K[i], KNew[i]);
            K[i] = KNew[i];
        }

        printf("%f\n", difference);

        // print2dArray(X, n);
        printf("\n==================\n");
        print2dArray(K, m);
        // print2dArray(KNew, m);

        printf("\n==================\n");

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

    if (CSize != 0)
    {
        sum[0] /= CSize;
        sum[1] /= CSize;
        sum[2] /= CSize;
    }

    return sum;
}

void print2dArray(int **K, int m)
{
    printf("R\tG\tB\n");

    for (int i = 0; i < m; i++)
    {
        printf("%d\t%d\t%d\n", K[i][0], K[i][1], K[i][2]);
    }
}

void print1dArray(int *C, int m)
{
    for (int i = 0; i < m; i++)
    {
        printf("%d\t", C[i]);
    }
    printf("\n==================> %d\n", m);
}