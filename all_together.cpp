#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int findCentroid(int *x, int **K, int m, int n);
float calculateDistance(int *x, int *k);
int *calculateAverage(int *C, int CSize, int **X, int m, int n);
void print2dArray(int **K, int m);

int main(int argc, char const *argv[])
{
    Mat img_mat = imread("image.png");

    int n = img_mat.rows * img_mat.cols;
    int m = 8;

    unsigned char *input = (unsigned char *)(img_mat.data);

    int **img = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        img[i] = (int *)malloc(3 * sizeof(int));
    }

    for (int i = 0; i < img_mat.rows; i++)
    {
        for (int j = 0; j < img_mat.cols; j++)
        {
            img[i * img_mat.rows + j][0] = (int)img_mat.ptr<uchar>(i);
            img[i * img_mat.rows + j][1] = input[img_mat.step * j + i + 1];
            img[i * img_mat.rows + j][2] = input[img_mat.step * j + i + 2];

            cout << i * img_mat.rows + j << " ---> " << input[img_mat.step * j + i] << endl;
        }
    }

    int **K = (int **)malloc(m * sizeof(int *));

    for (int i = 0; i < m; i++)
    {
        K[i] = (int *)malloc(3 * sizeof(int));

        K[i][0] = img[i][0];
        K[i][1] = img[i][1];
        K[i][2] = img[i][2];
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
            int cent = findCentroid(img[i], K, m, n);
            CSizes[cent]++;
            C[cent] = (int *)realloc(C[cent], CSizes[cent] * sizeof(int));
            C[cent][CSizes[cent] - 1] = i;
        }
        //Creating KNew for new centroid values KNew[m][3]
        int **KNew = (int **)malloc(m * sizeof(int *));

        for (int i = 0; i < m; i++)
        {
            KNew[i] = (int *)malloc(3 * sizeof(int));
            cout << "XXXXXXXXXXXXXXXXXXXXXXXX : " << C[i][0] << endl;
            KNew[i] = calculateAverage(C[i], CSizes[i], img, m, n);
        }

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

    // imshow("img", img);
    // imwrite("new.png", img);

    waitKey(0);

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

    cout<< sum[0] << " - " << sum[1] << " - " << sum[2] <<endl;

    if(CSize != 0){
        sum[0] /= CSize;
        sum[1] /= CSize;
        sum[2] /= CSize;
    }
    

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