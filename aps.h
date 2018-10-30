#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
//#include <io.h>

#define BIG_NUM 32767

// BLAS 函数
void sgemm_(char* transa, char* transb, int *m, int *n, int *k, float *alpha, 
        float *a, int *lda, float *b, int *ldb, float *beta, float *c, int *ldc);
void sgemv_(char* transa, int *m, int *n, float *alpha, float *a, int *lda, 
        float *x, int *incx, float *beta, float *y, int *incy);

// 工具类 aps_tools.c
int comp(const void *a, const void *b);
void relate_trans(int n, float *A, float *B);
void mt_copy(int n, float *A, float *B);
void rand_gen(int n, int *A);
void mat_sum(int n, float *A, float *y);

void plan_sort(int n, int *p_in, int *p_out);
void table_sort(int n, int *p_in, int *p_out);

// 数据与文件操作  aps_io.c
void source_remain(int n, int a, int dura, int *m_con, int *tnd, int *mnd, int *agenda, int *m_remain);
float cal_score(int rows, int dura, int *plan);
void write_plan(char *f_name, int days, int n, int m, int *m_row, int *agenda, int *plan);
int read_plan(FILE *fp, int m, int *m_row, float *score);
int import_data(FILE *fp, int rw, int *ml, int *mr);

// 利用随机方法的排程算法  aps_decide.c
int normal_decide(int n, int a, int n_w, int *m_con, int *tnd, int *mnd, float *dmx, int *ord, int *agenda);
int reverse_decide(int n, int a, int n_w, int *m_con, int *tnd, int *mnd, float *dmx, int *ord, int *agenda);
int quick_decide(int n, int a, int n_w, int *m_con, int *tnd, int *mnd, float *dmx, int *ord, int *agenda);
int snapback_decide(int n, int a, int n_w, int *m_con, int *tnd, int *mnd, float *dmx, int *ord, int *agenda);

// 为每台机器分配工作  aps_assign.c
void resource_allocate(int n, int a, int dura, int *m_con, int *m_row, int *tnd, int *mnd, int *agenda, int *plan);
void block_arrange(int n, int a, int ida, int dura, int row_ini, int rows, int *tnd, int *mnd, int *agenda, int *plan);

// 优化使用空间  aps_opt.c
void zip_space(int n, int a, int dura, int *m_con, int *tnd, int *mnd, int *agenda, float *dmx);
void delete_blank(int n, int dura, int r_ini, int rows, int *agenda, int *oneblock);
int move_space(int n, int dura, int rows, int *blk);

int best_choice(int n, int *mnd, int *agenda, int d_start, int d_stop, int *chosen);
void optim_space(int n, int dura, int rows, int *agenda, int *blk);
void warp_space(int n, int rows, int dura, int rid, int d_start, int d_stop, int *blk, int *agenda);

// DLL out
float rscr(char *dt_name, char res_dir);
float apsc(char *dt_name, char *res_dir, long max_iter);
