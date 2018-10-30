#include "aps.h"

float apsc(char *dt_name, char *res_dir, long max_iter)
{
    // 接收命令并读取文件
    
    FILE *fp;

    // ========================================================
    if ((fp = fopen(dt_name,"r")) == NULL)
    {
        printf("Can't open %s.\n", dt_name);
        exit(1);
    } 

    int n_p, n_a;
    fscanf(fp, "%d %d", &n_p, &n_a);

    int i, j, k, m, col;
    int m_con[n_a];
    int t_need[n_p];
    int m_need[n_p*n_a];
    float next_mx[n_p*n_p];
    float tran_mx[n_p*n_p];

    for (i = 0; i < n_a; i++)
        fscanf(fp, "%d", &m_con[i]);

    for (i = 0; i < n_p; i++)
    {
        fscanf(fp, "%d", &t_need[i]);
        for (j = 0; j < n_a; j++)
            fscanf(fp, "%d", &m_need[i*n_a+j]);
            
        for (j = 0; j < n_p; j++)
            next_mx[i*n_p+j] = 0.;

        fscanf(fp, "%d", &m);
        for (k = 0; k < m; k++)
        {
            fscanf(fp, "%d", &col);
            next_mx[i*n_p+col-1] = 1.;
        }
    }

    fclose(fp);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    char *pt;
    char *s_tmp = strtok_r((char *)dt_name,"/.",&pt);
    char *item_id = strtok_r(NULL,"/.",&pt);
    char *out_name = strcat( strcat( strcat((char *)res_dir, "/"), item_id), ".txt");
   
    // 计算关系传递矩阵
    relate_trans(n_p, next_mx, tran_mx);

    // 开始核心计算
    int n_w = 0;
    for (i = 0; i < n_p; i++)
        n_w += t_need[i];

    int schdl_rec[3*n_p];
    int ord_real[n_p];

    int idx, p, loop, grade;
    int du_min = BIG_NUM; 
    float high_mark = 0;
    float score;
    double n_best_record = 0.0;

    int num, changed;
    int mndi[n_p];

    int m_row[n_a+1];
    m_row[0] = 0;
    for (i = 0; i < n_a; i++)
        m_row[i+1] = m_row[i] + m_con[i];


    // 读存在的结果文件
    if ((fp = fopen(out_name,"r")) != NULL)
    {
        du_min = read_plan(fp, n_a, m_row, &high_mark);
        fclose(fp);
    }
    
    printf("%8s solving starts: duration %3d | score %5.2f\n", item_id, du_min, high_mark);
    // -------------------------------------------------------------------------

    time_t pr_start = time(NULL);
    srand((unsigned) time(&pr_start));

    for (loop = 1; loop <= max_iter; loop++)
    {

        for (i = 0; i < n_p; i++)
        {
            schdl_rec[i] = i;
            schdl_rec[n_p+i] = 0;
            schdl_rec[2*n_p+i] = 0;
        }

        num = (loop % 2 == 1) ? normal_decide(n_p, n_a, n_w, m_con, t_need, m_need, tran_mx, ord_real, schdl_rec)
            : reverse_decide(n_p, n_a, n_w, m_con, t_need, m_need, tran_mx, ord_real, schdl_rec);
        
        zip_space(n_p, n_a, num, m_con, t_need, m_need, schdl_rec, tran_mx);

        // 资源分配
        int plan_table[num*(m_row[n_a])];
        resource_allocate(n_p, n_a, num, m_con, m_row, t_need, m_need, schdl_rec, plan_table);

        for (j = 0; j < n_a; j++)
            delete_blank(n_p, num, m_row[j], m_con[j], schdl_rec, plan_table);

        score = cal_score(m_row[n_a], num, plan_table);

        if (num < du_min)
        {
            du_min = num;
            n_best_record = 0.0;

            write_plan(out_name, num, n_p, n_a, m_row, schdl_rec, plan_table);
            if (score > high_mark)
            {
                high_mark = score;
                printf("\033[1m\033[32m%8s\033[0m (loop %d) achieves a better duration \033[1m\033[33m%3d\033[0m | score \033[1m\033[33m%5.2f\033[0m\n", item_id, loop, du_min, high_mark);
            }
            else
                printf("\033[1m\033[32m%8s\033[0m (loop %d) achieves a better duration \033[1m\033[33m%3d\033[0m | score %5.2f\n", item_id, loop, du_min, high_mark);
        }
        else if (num == du_min)
            n_best_record ++;
            

        if (score > high_mark)
        {
            high_mark = score;
            write_plan(out_name, num, n_p, n_a, m_row, schdl_rec, plan_table);
            printf("\033[1m\033[31m%8s\033[0m (loop %d) obtains a higher score \033[1m\033[33m%5.2f\033[0m | duration %3d\n", item_id, loop, high_mark, du_min);

        }

    }

    time_t pr_stop = time(NULL);
    float re_idx = (1.0 - n_best_record/max_iter)*100.0;
    printf("\033[1m%8s\033[0m solving finish: time-cost %4ld\033s | duration \033[1m\033[32m%3d\033[0m | score \033[1m\033[32m%5.2f\033[0m | retry-index \033[1m\033[33m%5.2f\033[0m\n", 
        item_id, (pr_stop - pr_start), du_min, high_mark, re_idx);

    return re_idx;
}
    