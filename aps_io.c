#include "aps.h"

// 将最终计划按格式写入文件
void write_plan(char *f_name, int days, int n, int m, int *m_row, int *agenda, int *plan)
{
    int i, j, k, p;
    int avec[n];

    FILE *fp;
    if ((fp = fopen(f_name,"w")) == NULL)
    {
        printf("Can't open %s.\n", f_name);
        exit(1);
    }

    for (j = 0; j < n; j++)
        fprintf(fp, "\t%d", (j+1));

    fprintf(fp, "\n");

    for (k = 0; k < m; k++)
    {
        for (i = m_row[k]; i < m_row[k+1]; i++)
        {
            for (j = 0; j < n; j ++)
                avec[j] = 0;

            for (j = 0; j < days; j ++)
            {
                p = plan[i*days + j];
                avec[p] = (p > 0) ? 1 : 0;
            }

            fprintf(fp, "%d", (i+1));
            for (j = 0; j < n; j ++)
            {
                fprintf(fp, "\t");
                if (avec[j] > 0)
                    fprintf(fp, "%d,%d", agenda[n+j], agenda[2*n+j]);
            }
            fprintf(fp, "\n");
        }
    }

    fclose(fp);
}

// 读取计划，将计划转成安排表
int read_plan(FILE *fp, int m, int *m_row, float *score)
{
    int i, j, k;

    const int len = 512;
    char tline[len];
    char p_tmp[10];
    fgets(tline, len, fp);

    int u_l, u_r, row_l, row_r;
    float d_use;
    int dura = 0;
    int t_pre, t_now;
    char *ptr;
    char *p;
    float r_score = 0.0;

    for (k = 0; k < m; k ++)
    {
        for (i = m_row[k]; i < m_row[k+1]; i ++)
        {
            fgets(tline, len, fp);
            row_l = BIG_NUM;
            row_r = 0;
            d_use = 0.0;
            ptr = strtok_r(tline, "\t,", &p);

            while (1)
            {
                ptr = strtok_r(NULL, "\t,", &p);
                if (ptr == NULL)
                    break;
                strcpy(p_tmp, ptr);
                u_l = atoi(p_tmp);

                ptr = strtok_r(NULL, "\t,", &p);
                if (ptr == NULL)
                    break;
                strcpy(p_tmp, ptr);
                u_r = atoi(p_tmp);
                
                row_l = (u_l < row_l) ? u_l : row_l;
                row_r = (u_r > row_r) ? u_r : row_r;
                dura = (u_r > dura) ? u_r : dura;
                d_use += (u_r - u_l);
            }
            if (row_l < row_r)
            {
                r_score += d_use/(row_r - row_l);
            }

        }
    }

    dura = (dura <= 10) ? BIG_NUM : dura;
    r_score = r_score/m_row[m];
    *score = (r_score <= 1) ? (1000.0/(dura+1.0) + r_score) : 0;

    return dura;
}


int import_data(FILE *fp, int rw, int *ml, int *mr)
{
    int i, j, k, col;

    const int len = 512;
    char tline[len];
    char ch[10];

    fgets(tline, len, fp);
    //printf("%s\n",tline);
    int n;
    int z = 0;
    for (i = 0; i < strlen(tline); i++)
    {
        //printf("  %d",tline[i]);
        if (tline[i] == 9)
            z = 0;
        else
        {
            ch[z] = tline[i];
            z++;
        }
    }
    ch[z] = '\0';
    n = atoi(ch);

    int dura = 0;
    for (k = 0; k < rw; k ++)
    {
        fgets(tline, len, fp);
        //printf("%s\n",tline);
        z = 0;
        col = 0;
        for (i = 0; i < strlen(tline); i++)
        {
            if (tline[i] == 9)
            {
                
                ch[z] = '\0';
                if ((col == 0) && (z > 0)) 
                {
                    if (atoi(ch) != (k+1))
                    {
                        printf("row number unmatch!\n");
                        exit(1);
                    }
                }
                else if (z > 0)
                {
                    mr[k*n+col-1] = atoi(ch);
                    dura = (atoi(ch) > dura) ? atoi(ch) : dura;
                }
                z = 0;
                col++;
            }
            else if (tline[i] == 44)
            {
                ch[z] = '\0';
                ml[k*n+col-1] = atoi(ch);
                z = 0;
            }
            else
            {
                ch[z] = tline[i];
                z++;
            }
                
        }

    }
    return dura;
}
