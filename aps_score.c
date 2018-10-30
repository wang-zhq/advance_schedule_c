#include "aps.h"

float resscr(char *dt_name, char *res_dir)
{
    // 接收命令并读取文件
    //printf("%s, %s\n", dt_name, res_dir);
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

    // 开始核心计算
    float high_mark = 0;
    float score;
    int num;

    int m_row[n_a+1];
    m_row[0] = 0;
    for (i = 0; i < n_a; i++)
        m_row[i+1] = m_row[i] + m_con[i];

    int mx_l[m_row[n_a]*n_p];
    int mx_r[m_row[n_a]*n_p];
    for (i = 0; i < m_row[n_a]*n_p; i++)
    {
        mx_l[i] = 0;
        mx_r[i] = 0;
    }
    // 读存在的结果文件
    //printf("%s\n",out_name);
    if ((fp = fopen(out_name,"r")) != NULL)
    {
        num = import_data(fp, m_row[n_a], mx_l, mx_r);
        fclose(fp);
    }

    int schdl_rec[3*n_p];
    for (i = 0; i < n_p*3; i++)
        schdl_rec[i] = 0;
    
    int plan_table[num*m_row[n_a]];
    for (i = 0; i < num*m_row[n_a]; i++)
        plan_table[i] = 0;
    
    int u_l, u_r;
    for (i = 0; i < m_row[n_a]; i++)
    {
        for (k = 0; k < n_p; k++)
        {
            u_l = mx_l[i*n_p + k];
            u_r = mx_r[i*n_p + k];
            if (u_r > u_l)
            {
                for (j = u_l; j < u_r; j++)
                    plan_table[i*num + j] = k;
            }
        }
    }

    score = cal_score(m_row[n_a], num, plan_table);

    //printf("The score of %s is \033[1m\033[33m%5.2f\033[0m\n", item_id, score);

    return score;
}
    