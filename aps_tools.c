#include "aps.h"

// 关系传递矩阵
void relate_trans(int n, float *A, float *B)
{
    char TRANS = 'N';
    float alpha = 0.1;
    float beta = 1.;
    int m = n*n;
    int j;
    float C[m];

    mt_copy(m, A, B);
    sgemm_(&TRANS, &TRANS, &n, &n, &n, &alpha, A, &n, A, &n, &beta, B, &n);

    for (j = 0; j < 10; j++)
    {
        mt_copy(m, B, C);
        sgemm_(&TRANS, &TRANS, &n, &n, &n, &alpha, C, &n, C, &n, &beta, B, &n);
    }
}

// 生成一个不大于999的随机序列
void rand_gen(int n, int *A)
{
    int i;
    long S[n];

    for (i = 0; i < n; i++)
        S[i] = (rand()%1000)*1000 + i;

    qsort(S, n, sizeof(long), comp);

    for (i = 0; i < n; i++)
        A[i] = S[i]%1000;

}

// 对矩阵按行求和
void mat_sum(int n, float *A, float *y)
{
    char TRANS = 'N';
    float one = 1;
    int m = n*n;
    int inc = 1;
    int i, j;
    float x[n];

    for (i = 0; i < n; i++)
    {
        x[i] = 1.;
        y[i] = 0.;
    }

    sgemv_(&TRANS, &n, &n, &one, A, &n, x, &inc, &one, y, &inc);
}

// 排序算法
void plan_sort(int n, int *p_in, int *p_out)
{
    int i;
    long pt[n];
    const int bn = 512;
    for (i = 0; i < n; i++)
        pt[i] = p_in[n+i]*1000*1000 + (bn - p_in[2*n+i])*1000 + p_in[i];

    qsort(pt, n, sizeof(long), comp);
    for (i = 0; i < n; i++)
    {
        p_out[i] = pt[i]%1000;
        p_out[n+i] = pt[i]/1000/1000;
        p_out[2*n+i] = bn - (pt[i]%(1000*1000))/1000;
    }

}

void table_sort(int n, int *p_in, int *p_out)
{
    int i;
    long pt[n];
    for (i = 0; i < n; i++)
        pt[i] = p_in[n+i]*1000*1000 + p_in[2*n+i]*1000 + p_in[i];

    qsort(pt, n, sizeof(long), comp);
    for (i = 0; i < n; i++)
    {
        p_out[i] = pt[i]%1000;
        p_out[n+i] = pt[i]/1000/1000;
        p_out[2*n+i] = (pt[i]%(1000*1000))/1000;
    }

}

// 浮点向量的拷贝
void mt_copy(int n, float *A, float *B)
{
    int i;
    for (i = 0; i < n; i++)
        B[i] = A[i];
}

// 据说是给快速索引用的
int comp(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;
}

