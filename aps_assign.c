#include "aps.h"

// 计算剩余量
void source_remain(int n_p, int n_a, int dura, int *m_con, int *t_need, int *m_need, 
		int *schdl_rec, int *m_remain)
{
	int i, j, k;
	for (j = 0; j < (n_a*dura); j++)
		m_remain[j] = m_con[j/dura];
	
	for (i = 0; i < n_p; i ++)
	{
		for (k = 0; k < n_a; k++)
		{
			if (m_need[i*n_a+k] > 0)
			{
				for (j = schdl_rec[n_p+i]; j < schdl_rec[n_p*2+i]; j++)
					m_remain[k*dura+j] -= m_need[i*n_a+k];
			}
		}
	}

}


// 资源安排
void resource_allocate(int n_p, int n_a, int dura, int *m_con, int *m_row, int *t_need,
        int *m_need, int *schdl_rec, int *plan_table)
{
    int i, k;

    for (i = 0; i < dura*m_row[n_a]; i++)
        plan_table[i] = 0;

    int schr_sorted[3*n_p];
    if (rand()%2 == 1)
        plan_sort(n_p, schdl_rec, schr_sorted);
    else
        table_sort(n_p, schdl_rec, schr_sorted);

    for (k = 0; k < n_a; k++)
        block_arrange(n_p, n_a, k, dura, m_row[k], m_con[k], t_need, m_need, schr_sorted, plan_table);

}



void block_arrange(int n_p, int n_a, int ida, int dura, int row_ini, int rows, int *t_need,
        int *m_need, int *agenda, int *plan_table)
{
    int i, j, k, u, p, day, loc;
    int u_l, u_r;

    int iblock[rows][dura];
    // 一定要记得初始化！
    for (i = 0; i < rows*dura; i++)
        iblock[i/dura][i%dura] = 0;

    for (i = 0; i < n_p; i++)
    {
        p = agenda[i];
        u_l = agenda[n_p + i];
        u_r = agenda[2*n_p + i];

        if (m_need[p*n_a+ida] == 0)
            continue;

        for (k = 0; k < m_need[p*n_a+ida]; k++)
        {
            loc = BIG_NUM;
            for (j = 0; j < rows; j++)
            {
                if (iblock[j][u_l] == 0)
                {
                    loc = j;
                    break;
                }
            }
            // printf("%d, %d, %d, %d\n",p, loc, u_l, rows);
            if (loc > rows)
            {
                printf("Preallocated row exceed bound.\n");
                exit(1);
            }

            for (day = u_l; day < u_r; day++)
                iblock[loc][day] = p;

        }
    }

    for (i = 0; i < rows*dura; i++)
        plan_table[row_ini*dura + i] = iblock[i/dura][i%dura];

}

// 计算得分
float cal_score(int rows, int dura, int *plan)
{
    int i, j, k, u_l, u_r;
    float d_use;
    float d_score = 0.0;
    for (i = 0; i < rows; i++)
    {
        u_l = dura;
        u_r = 0;
        d_use = 0.0;
        for (k = 0; k < dura; k++)
        {
            if (plan[i*dura + k] > 0)
            {
                u_l = (k < u_l) ? k : u_l;
                u_r = (k > u_r) ? k : u_r;
                d_use ++;
            }
        }
        if (u_r >= u_l)
        {
            d_score += d_use/(u_r - u_l + 1.0);
        }
    }

    return (1000.0/(dura+1.0) + d_score/rows);
}

