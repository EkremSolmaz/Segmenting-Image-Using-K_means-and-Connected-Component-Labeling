#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;

int checkNeighbors(int x, int y, int **A, int **B, int n, int m);
void fixLabels(int **B, int n, int m, int replacement, int old);
void print2dArray(int **K, int n, int m);

int main(int argc, char const *argv[])
{
    srand(time(0));

    int n = 4;
    int m = 4;

    int **A = (int **)malloc(n * sizeof(int *));
    int **B = (int **)calloc(n, sizeof(int *));

    for (int i = 0; i < n; i++)
    {
        A[i] = (int *)malloc(m * sizeof(int));
        B[i] = (int *)calloc(m, sizeof(int));

        for (int j = 0; j < m; j++){
            A[i][j] = rand() % 4 + 1;
        }
    }


    int newLabel = 1;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            int group = checkNeighbors(i, j, A, B, n, m);

            if (group == -1)
            {
                B[i][j] = newLabel;
                newLabel++;
            }
            else
                B[i][j] = group;
        }
    }

    cout << "========================" << endl;
    print2dArray(A, n, m);
    cout << "========================" << endl;
    print2dArray(B, n, m);
    cout << "========================" << endl;

    return 0;
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
    cout<<x<<"-"<<y<<"--->"<<label<<endl;
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

void print2dArray(int **K, int n, int m)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            cout << K[i][j] << "\t";
        }
        cout << endl;
    }
}