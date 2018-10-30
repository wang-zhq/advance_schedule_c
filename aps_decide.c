#include "aps.h"

//正向排程
int normal_decide(int n_p, int n_a, int n_w, int *m_con, int *t_need, int *m_need,
        float *dmx_sav, int *ord_real, int *schdl_rec)
{
    int i, j, k, p, idx, tcan_in, ct_flag, i_tag;
    float tran_mx[n_p*n_p];
    float pst[n_p];    
    
    int ord_ini[n_p];
    int ord_wait[n_p];   

    int r_remain[n_w][n_a];
    int r_tmp[n_w][n_a];

    for (i = 0; i < n_w; i++)
    {
        for (j = 0; j < n_a; j++)
            r_remain[i][j] = m_con[j];
    }

    rand_gen(n_p, ord_ini);

    mt_copy(n_p*n_p, dmx_sav, tran_mx);
    mat_sum(n_p, tran_mx, pst);

    for (i = 0; i < n_p; i++)
    {
        if (pst[i] == 0)
        {
            ord_real[0] = i;
            break;
        }
    }

    for (i = 0; i < n_p; i++)
        ord_wait[i] = (ord_ini[i] == ord_real[0]) ? BIG_NUM : ord_ini[i];

    for (idx = 1; idx < (n_p-1); idx++)
    {
        for (j = 0; j < n_p; j++)
            tran_mx[ord_real[idx-1]*n_p + j] = 0;

        mat_sum(n_p, tran_mx, pst);
        for (j = 0; j < n_p; j++)
        {
            p = ord_wait[j];
            if (p < n_p)
            {
                if (pst[p] == 0)
                {
                    ord_wait[j] = BIG_NUM;
                    ord_real[idx] = p;
                    break;
                } 
            }
        }

        tcan_in = 0;
        for (k = 0; k < n_p; k++)
        {
            // 行与列需要考虑 ！！！！！
            if ((dmx_sav[k*n_p+p] > 0) && (schdl_rec[2*n_p+k] > tcan_in))
                tcan_in = schdl_rec[2*n_p+k]; 
        }

        ct_flag = 1;
        while (ct_flag == 1)
        {
            for (i = 0; i < n_w; i++)
            {
                for (k = 0; k < n_a; k++)
                    r_tmp[i][k] = r_remain[i][k];
            }

            i_tag = -1;
            for (i = tcan_in; i < (tcan_in+t_need[p]); i++)
            {
                for (k = 0; k < n_a; k++)
                {
                    r_tmp[i][k] = r_remain[i][k]-m_need[p*n_a+k];
                    if (r_tmp[i][k] < 0)
                        i_tag = i;
                }
            }

            if (i_tag >= 0)
                tcan_in = i_tag + 1;
            else
            {
                ct_flag = 0;
                schdl_rec[n_p+p] = tcan_in;
                schdl_rec[2*n_p+p] = tcan_in + t_need[p];
            }
        }
        for (i = 0; i < n_w; i++)
        {
            for (k = 0; k < n_a; k++)
                r_remain[i][k] = r_tmp[i][k];
        }

    }

    int num = 0;
    for (i = 0; i < n_p; i++)
    {
        if (ord_wait[i] < n_p)
            p = ord_wait[i];

        if (num < schdl_rec[2*n_p+i])
            num = schdl_rec[2*n_p+i];
    }
    ord_real[n_p-1] = p;
    schdl_rec[n_p+p] = num;
    schdl_rec[2*n_p+p] = num;

    return num;
}


//逆向排程
int reverse_decide(int n_p, int n_a, int n_w, int *m_con, int *t_need, int *m_need,
        float *dmx_sav, int *ord_real, int *schdl_rec)
{
    int i, j, k, p, idx, tcan_in, ct_flag, i_tag;
    float tran_mx[n_p*n_p];
    float pst[n_p];    
    
    int ord_ini[n_p];
    int ord_wait[n_p];    

    int r_remain[n_w][n_a];
    int r_tmp[n_w][n_a];

    for (i = 0; i < n_w; i++)
    {
        for (j = 0; j < n_a; j++)
            r_remain[i][j] = m_con[j];
    }

    rand_gen(n_p, ord_ini);

    for (i = 0; i < n_p; i++)
    {
        for (j = 0; j < n_p; j++)
            tran_mx[i*n_p+j] = dmx_sav[j*n_p+i];
    }
    
    mat_sum(n_p, tran_mx, pst);

    for (i = 0; i < n_p; i++)
    {
        if (pst[i] == 0)
        {
            ord_real[0] = i;
            break;
        }
    }

    for (i = 0; i < n_p; i++)
        ord_wait[i] = (ord_ini[i] == ord_real[0]) ? BIG_NUM : ord_ini[i];

    for (idx = 1; idx < (n_p-1); idx++)
    {
        for (j = 0; j < n_p; j++)
            tran_mx[ord_real[idx-1]*n_p + j] = 0;

        mat_sum(n_p, tran_mx, pst);
        for (j = 0; j < n_p; j++)
        {
            p = ord_wait[j];
            if (p < n_p)
            {
                if (pst[p] == 0)
                {
                    ord_wait[j] = BIG_NUM;
                    ord_real[idx] = p;
                    break;
                } 
            }
        }

        tcan_in = 0;
        for (k = 0; k < n_p; k++)
        {
            // 行与列需要考虑 ！！！！！
            if ((dmx_sav[p*n_p+k] > 0) && (schdl_rec[2*n_p+k] > tcan_in))
                tcan_in = schdl_rec[2*n_p+k]; 
        }

        ct_flag = 1;
        while (ct_flag == 1)
        {
            for (i = 0; i < n_w; i++)
            {
                for (k = 0; k < n_a; k++)
                    r_tmp[i][k] = r_remain[i][k];
            }

            i_tag = -1;
            for (i = tcan_in; i < (tcan_in+t_need[p]); i++)
            {
                for (k = 0; k < n_a; k++)
                {
                    r_tmp[i][k] = r_remain[i][k]-m_need[p*n_a+k];
                    if (r_tmp[i][k] < 0)
                        i_tag = i;
                }
            }

            if (i_tag >= 0)
                tcan_in = i_tag + 1;
            else
            {
                ct_flag = 0;
                schdl_rec[n_p+p] = tcan_in;
                schdl_rec[2*n_p+p] = tcan_in + t_need[p];
            }
        }
        for (i = 0; i < n_w; i++)
        {
            for (k = 0; k < n_a; k++)
                r_remain[i][k] = r_tmp[i][k];
        }

    }

    int num = 0;
    for (i = 0; i < n_p; i++)
    {
        if (ord_wait[i] < n_p)
            p = ord_wait[i];

        if (num < schdl_rec[2*n_p+i])
            num = schdl_rec[2*n_p+i];
    }
    ord_real[n_p-1] = p;
    schdl_rec[n_p+p] = num;
    schdl_rec[2*n_p+p] = num;

    int sd_tmp;
    // 对日程表进行反向计算，并排出实际顺序
    for (i = 0; i < n_p; i++)
    {
        sd_tmp = schdl_rec[n_p+i];
        schdl_rec[n_p+i] = num - schdl_rec[2*n_p+i];
        schdl_rec[2*n_p+i] = num - sd_tmp;
    }
    
    return num;
}


//快速排程
int quick_decide(int n_p, int n_a, int n_w, int *m_con, int *t_need, int *m_need,
        float *dmx_sav, int *ord_real, int *schdl_rec)
{
    int i, j, k, p, i_tag;
    float tran_mx[n_p*n_p];
    float pst[n_p];
    
    int ord_ini[n_p];
    int ord_wait[n_p];   

    int r_remain[n_w][n_a];

    for (i = 0; i < n_w; i++)
    {
        for (j = 0; j < n_a; j++)
            r_remain[i][j] = m_con[j];
    }

    rand_gen(n_p, ord_ini);
    mt_copy(n_p*n_p, dmx_sav, tran_mx);

    for (i = 0; i < n_p; i++)
    {
        ord_wait[i] = ord_ini[i];
        ord_real[i] = 0;
    }

    int m = 0;
    int num = 0;
    int dura = 0;
    int ct_flag = 1;
    while (ct_flag == 1)
    {
        ct_flag = 0;
        mat_sum(n_p, tran_mx, pst);

        for (j = 0; j < n_p; j++)
        {
            p = ord_wait[j];
            if ((p < n_p) && (pst[p] == 0))
            {
                
                ct_flag = 1;
                i_tag = 1;
                for (k = 0; k < n_a; k++)
                {
                    if (r_remain[num][k] < m_need[p*n_a+k])
                    {
                        i_tag = 0;
                        break;
                    }
                }

                for (k = 0; k < n_p; k++)
                {
                    // 行与列需要考虑 ！！！！！
                    if ((dmx_sav[k*n_p+p] > 0) && (schdl_rec[2*n_p+k] > num))
                    {
                        i_tag = 0; 
                        break;
                    }
                }

                if (i_tag == 1)
                {
                    //printf("%d %d\n", p, num);
                    ord_real[m] = p;
                    m++;

                    ord_wait[j] = BIG_NUM;
                    schdl_rec[n_p + p] = num;
                    schdl_rec[2*n_p + p] = num + t_need[p];
                    dura = (schdl_rec[2*n_p+p] > dura) ? schdl_rec[2*n_p+p] : dura;

                    for (i = 0; i < n_p; i++)
                    {
                        tran_mx[p*n_p + i] = 0;
                    }

                    if (t_need[p] > 0)
                    {
                        for (i = 0; i < t_need[p]; i++)
                        {
                            for (k = 0; k < n_a; k++)
                            {
                                r_remain[num+i][k] -= m_need[p*n_a+k];
                            }
                        }  
                    } 
                    

                }
                else
                    break;
            
            }
        }
        num ++;
    }
    return dura;
}

//快速逆向排程
int snapback_decide(int n_p, int n_a, int n_w, int *m_con, int *t_need, int *m_need,
        float *dmx_sav, int *ord_real, int *schdl_rec)
{
    int i, j, k, p, i_tag;
    float tran_mx[n_p*n_p];
    float pst[n_p];
    
    int ord_ini[n_p];
    int ord_wait[n_p];   

    int r_remain[n_w][n_a];

    for (i = 0; i < n_w; i++)
    {
        for (j = 0; j < n_a; j++)
            r_remain[i][j] = m_con[j];
    }

    rand_gen(n_p, ord_ini);
    for (i = 0; i < n_p; i++)
    {
        for (j = 0; j < n_p; j++)
            tran_mx[i*n_p+j] = dmx_sav[j*n_p+i];
    }

    for (i = 0; i < n_p; i++)
    {
        ord_wait[i] = ord_ini[i];
        ord_real[i] = 0;
    }

    int m = 0;
    int num = 0;
    int dura = 0;
    int ct_flag = 1;
    while (ct_flag == 1)
    {
        ct_flag = 0;
        mat_sum(n_p, tran_mx, pst);

        for (j = 0; j < n_p; j++)
        {
            p = ord_wait[j];
            if ((p < n_p) && (pst[p] == 0))
            {
                ct_flag = 1;
                i_tag = 1;
                for (k = 0; k < n_a; k++)
                {
                    if (r_remain[num][k] < m_need[p*n_a+k])
                    {
                        i_tag = 0;
                        break;
                    }
                }

                for (k = 0; k < n_p; k++)
                {
                    // 行与列需要考虑 ！！！！！
                    if ((dmx_sav[p*n_p+k] > 0) && (schdl_rec[2*n_p+k] > num))
                    {
                        i_tag = 0; 
                        break;
                    }
                }

                if (i_tag == 1)
                {
                    //printf("%d %d\n", p, num);
                    ord_real[m] = p;
                    m++;

                    ord_wait[j] = BIG_NUM;
                    schdl_rec[n_p + p] = num;
                    schdl_rec[2*n_p + p] = num + t_need[p];
                    dura = (schdl_rec[2*n_p+p] > dura) ? schdl_rec[2*n_p+p] : dura;

                    for (i = 0; i < n_p; i++)
                    {
                        tran_mx[p*n_p + i] = 0;
                    }

                    if (t_need[p] > 0)
                    {
                        for (i = 0; i < t_need[p]; i++)
                        {
                            for (k = 0; k < n_a; k++)
                            {
                                r_remain[num+i][k] -= m_need[p*n_a+k];
                            }
                        }  
                    } 
                    

                }
                else
                    break;
            
            }
        }
        num ++;
    }

    int sd_tmp;
    // 对日程表进行反向计算，并排出实际顺序
    for (i = 0; i < n_p; i++)
    {
        sd_tmp = schdl_rec[n_p+i];
        schdl_rec[n_p+i] = dura - schdl_rec[2*n_p+i];
        schdl_rec[2*n_p+i] = dura - sd_tmp;
    }

    return dura;
}

