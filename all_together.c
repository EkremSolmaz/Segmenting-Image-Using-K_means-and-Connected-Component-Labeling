#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

int findCentroid(int *x, int **K, int m, int n);
float calculateDistance(int *x, int *k);
int *calculateAverage(int *C, int CSize, int **X, int m, int n);
void print2dArray(int **K, int m);
void print1dArray(int *C, int m);
int checkNeighbors(int x, int y, int **A, int **B, int n, int m);
void fixLabels(int **B, int n, int m, int replacement, int old);
void print2dArray2(int **K, int n, int m);

int main()
{
    srand(time(0));

    char filename[100] = "shark.jpg";
    int no_of_centroid = 32;

    CvMat *img = cvLoadImageM(filename, CV_LOAD_IMAGE_UNCHANGED);

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
    int m = no_of_centroid;

    printf("%d\n", n);

    int **K = (int **)malloc(m * sizeof(int *));

    for (int i = 0; i < m; i++)
    {
        K[i] = (int *)malloc(3 * sizeof(int));

        // K[i][0] = X[i][0];
        // K[i][1] = X[i][1];
        // K[i][2] = X[i][2];

        K[i][0] = X[rand() % n][0];
        K[i][1] = X[rand() % n][1];
        K[i][2] = X[rand() % n][2];
    }

    float difference;
    int thresh = 1;
    print2dArray(K, m);
    int **C = (int **)malloc(m * sizeof(int *));
    int *CSizes = (int *)calloc(m, sizeof(int));

    do
    {
        difference = 0;
        // Creating m array to group pixels in X C[m][?]
        for (int i = 0; i < m; i++)
        {
            C[i] = (int *)malloc(sizeof(int));
        }
        // Creating m size array for sizes of groups CSizes[m]
        CSizes = (int *)calloc(m, sizeof(int));

        //Find every pixel's centroid and add it to its group
        for (int i = 0; i < n; i++)
        {
            int cent = findCentroid(X[i], K, m, n);
            CSizes[cent]++;
            C[cent] = (int *)realloc(C[cent], CSizes[cent] * sizeof(int));
            C[cent][CSizes[cent] - 1] = i;
        }
        print1dArray(CSizes, m);

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

    int **newX = (int **)malloc(nrow * sizeof(int *));
    int **labelHolder = (int **)calloc(nrow, sizeof(int *));
    for (int i = 0; i < nrow; i++)
    {
        newX[i] = (int *)malloc(ncol * sizeof(int));
        labelHolder[i] = (int *)calloc(ncol, sizeof(int));
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < CSizes[i]; j++)
        {
            int idx = C[i][j];
            newX[idx / ncol][idx % ncol] = i;

            X[C[i][j]][0] = K[i][0];
            X[C[i][j]][1] = K[i][1];
            X[C[i][j]][2] = K[i][2];
        }
    }
    //before ccl

    int newLabel = 1;

    for (int i = 0; i < nrow; i++)
    {
        for (int j = 0; j < ncol; j++)
        {
            int group = checkNeighbors(i, j, newX, labelHolder, nrow, ncol);

            if (group == -1)
            {
                labelHolder[i][j] = newLabel;
                newLabel++;
            }
            else
                labelHolder[i][j] = group;
        }
    }
    printf("%d-%d\n", nrow, ncol);
    print2dArray2(newX, nrow, ncol);
    print2dArray2(labelHolder, nrow, ncol);

    //new color for every unique label
    int **labelColors = (int **)malloc(newLabel * sizeof(int *));

    int ctr = 0;
    int step = (255 / ((int)cbrt(newLabel) + 1));

    for (int i = 0; i < 255; i += step)
    {
        for (int j = 0; j < 255; j += step)
        {
            for (int k = 0; k < 255; k += step)
            {
                labelColors[ctr] = (int *)malloc(3 * sizeof(int));
                labelColors[ctr][0] = i;
                labelColors[ctr][1] = j;
                labelColors[ctr][2] = k;
                ctr++;
            }
        }
    }

    //after ccl

    //save after K_means
    for (int i = 0; i < nrow; i++)
    {
        for (int j = 0; j < ncol; j++)
        {
            int red = X[i * ncol + j][2];
            int green = X[i * ncol + j][1];
            int blue = X[i * ncol + j][0];

            CvScalar new_value = cvScalar(blue, green, red, 0);
            cvSet2D(img, i, j, new_value);
            cvSet2D(img, i, j, new_value);
            cvSet2D(img, i, j, new_value);
        }
    }

    char kmeansfilename[100] = "new_images/K_means_new_";
    char k_str[10];
    sprintf(k_str, "%d", no_of_centroid);
    strcat(kmeansfilename, k_str);
    strcat(kmeansfilename, "_");
    strcat(kmeansfilename, filename);
    cvSaveImage(kmeansfilename, img, 0);
    printf("K_means result saved as %s\n", kmeansfilename);

    //save after CCL
    for (int i = 0; i < nrow; i++)
    {
        for (int j = 0; j < ncol; j++)
        {
            int red = labelColors[labelHolder[i][j]][2];
            int green = labelColors[labelHolder[i][j]][1];
            int blue = labelColors[labelHolder[i][j]][0];

            CvScalar new_value = cvScalar(blue, green, red, 0);
            cvSet2D(img, i, j, new_value);
            cvSet2D(img, i, j, new_value);
            cvSet2D(img, i, j, new_value);
        }
    }

    char cclfilename[100] = "new_images/CCL_new_";
    strcat(cclfilename, k_str);
    strcat(cclfilename, "_");
    strcat(cclfilename, filename);
    cvSaveImage(cclfilename, img, 0);
    printf("CCL result saved as %s\n", cclfilename);

    printf("========================> %d - %d\n", ((int)cbrt(newLabel) + 1), step);

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

int checkNeighbors(int x, int y, int **A, int **B, int n, int m)
{
    int label = -1;

    for (int i = max(0, x - 1); i < min(n, x + 1); i++)
    {
        for (int j = max(0, y - 1); j < min(m, y + 1); j++)
        {
            if (x != i || y != j)
                if (B[i][j] != 0)
                    if (A[x][y] == A[i][j])
                        if (label != -1 && label != B[i][j])
                        {
                            //conflict
                            fixLabels(B, n, m, label, B[i][j]);
                        }
                        else if (B[i][j] != 0)
                        {
                            label = B[i][j];
                        }
        }
    }
    printf("%d-%d--->%d\n", x, y, label);
    return label;
}

void fixLabels(int **B, int n, int m, int replacement, int old)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (B[i][j] == old)
                B[i][j] = replacement;
        }
    }
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

void print2dArray2(int **K, int n, int m)
{
    printf("======================\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            printf("%d\t", K[i][j]);
        }
        printf("\n");
    }
    printf("======================\n");
}