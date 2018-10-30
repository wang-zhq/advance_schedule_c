#include "aps.h"

// 压缩空间，前半程向后压缩，后半程向前压缩
void zip_space(int n_p, int n_a, int dura, int *m_con, int *t_need, int *m_need, 
		int *schdl_rec, float *dmx)
{
	int i, j, k, idx;
	int changed = 0;
	int m_remn[n_a*dura];
	int u_l, u_r, mvb_l, mvb_r;
	int d_mark;
	
	int cent_dis[3*n_p];
	int cds_srt[3*n_p];

	for (j = 0; j < n_p; j++)
	{
		cent_dis[j] = schdl_rec[j];
		cent_dis[n_p+j] = abs(schdl_rec[n_p+j] + schdl_rec[2*n_p+j] - dura);
		cent_dis[2*n_p+j] = schdl_rec[2*n_p+j];
	}

	if (rand()%2 == 1)
		plan_sort(n_p, cent_dis, cds_srt);
	else
		table_sort(n_p, cent_dis, cds_srt);

	for (i = 0; i < n_p; i++)
	{
		idx = cds_srt[i];
		u_l = schdl_rec[n_p+idx];
		u_r = schdl_rec[n_p*2+idx];
		if (u_l >= u_r)
			continue;

		source_remain(n_p, n_a, dura, m_con, t_need, m_need, schdl_rec, m_remn);

		mvb_l = 0;
		mvb_r = dura;
		for (j = 0; j < n_p; j++)
		{
			//关系矩阵 横向是向后依赖，纵向是向前
			mvb_l = ((schdl_rec[n_p*2+j] > mvb_l) && (dmx[j*n_p+idx] > 0)) ? schdl_rec[n_p*2+j] : mvb_l;
			mvb_r = ((schdl_rec[n_p+j] < mvb_r) && (dmx[idx*n_p+j] > 0)) ? schdl_rec[n_p+j] : mvb_r;
		}
		
		if (((u_l + u_r) < dura) && (u_r < mvb_r))
		{
			d_mark = mvb_r;
			for (j = 0; j < n_a; j++)
			{
				for (k = u_r; k < d_mark; k++)
				{
					if (m_remn[j*dura+k] < m_need[idx*n_a+j])
					{
						d_mark = k;
						break;
					}
				}

			}

			if (d_mark > u_r)
			{
				schdl_rec[n_p+idx] += (d_mark - u_r);
				schdl_rec[2*n_p+idx] += (d_mark - u_r);
			}			

		}
		else if (((u_l + u_r) > dura) && (u_l > mvb_l))
		{
			d_mark = mvb_l;
			for (j = 0; j < n_a; j++)
			{
				for (k = (u_l-1); k >= d_mark; k--)
				{
					if (m_remn[j*dura+k] < m_need[idx*n_a+j])
					{
						d_mark = k;
						break;
					}
				}
			}

			if (d_mark < (u_l-1))
			{
				schdl_rec[n_p+idx] += (d_mark-u_l+1);
				schdl_rec[2*n_p+idx] += (d_mark-u_l+1);
			}
		}
	}
	
}

// 统计所有的空白，并对旁白，中间的和空栏 分类
void delete_blank(int n_p, int dura, int r_ini, int rows, int *agenda, int *oneblock)
{
	int i, adjusted;

	int blk[dura*rows];

	for (i = 0; i < (dura*rows); i++)
		blk[i] = oneblock[r_ini*dura + i];

	adjusted = 1;
	while (adjusted > 0)
	{
		adjusted = move_space(n_p, dura, rows, blk);
		//printf("move state: %d\n", adjusted);
	}

	optim_space(n_p, dura, rows, agenda, blk);
		
	for (i = 0; i < (dura*rows); i++)
		oneblock[r_ini*dura + i] = blk[i];
}


int move_space(int n_p, int dura, int rows, int *blk)
{
	int i, j, k, p;

	int space_list[rows*n_p];

	int margin_l[rows];
	int margin_r[rows];
	int blank_line[rows];

	int n_sp = 0;
	int n_bl = 0;
	int m_l = 0;
	int m_r = 0;

	int u_l, u_r;
	int pre_space;

	for (i = 0; i < rows; i++)
	{
		margin_l[i] = 0;
		margin_r[i] = dura;
		blank_line[i] = 0;

		u_l = (blk[i*dura] > 0) ? 1 : 0;
		u_r = 1;

		for (k = 1; k < dura; k++)
		{
			if (blk[i*dura + k] > 0)
			{
				if (blk[i*dura + k - 1] == 0)
				{
					if (u_l == 0)
					{
						margin_l[i] = u_r;
						m_l ++;
					}
					else
					{
						space_list[3*n_sp] = u_l;
						space_list[3*n_sp + 1] = u_r - u_l;
						space_list[3*n_sp + 2] = i;
						n_sp ++;
					}
					
				}
				u_l = k + 1;
				u_r = k + 1;

			}
			else
			{
				u_r ++;				
			}			
		}

		// 在一行结尾判断右空白或空行
		if (u_l < u_r)
		{
			margin_r[i] = u_l;
			if (u_l == 0)
			{
				blank_line[i] = 1;
				n_bl ++;
			}
			else
			{
				m_r ++;
			}
		}
	
	}

	int space_c[n_sp*3];
	int space_srt[n_sp*3];
	for (i = 0; i < n_sp; i++)
	{
		space_c[i] = space_list[3*i];
		space_c[n_sp+i] = space_list[3*i+1];
		space_c[2*n_sp+i] = space_list[3*i+2];
	}
	table_sort(n_sp, space_c, space_srt);

	int row_out, row_in;

	if (n_sp == 0)
	{
		if (n_bl > 0)
		{
			for (i = 0; i < rows; i++)
			{
				if (blank_line[i] > 0)
				{
					row_in = i;
					break;
				}
			}

			for (i = 0; i < rows; i++)
			{
				if (blank_line[i] == 0)
				{
					for (k = 0; k < (dura-1); k++)
					{
						if ((blk[i*dura+k] > 0) && (blk[i*dura+k+1] > 0))
						{
							if (blk[i*dura+k] != blk[i*dura+k+1])
							{
								for (j = k+1; j < dura; j++)
								{
									blk[row_in*dura + j] = blk[i*dura + j];
									blk[i*dura + j] = 0;
								}
								return 9;
							}
						}
					}
				}
			}
			return 0;
		} 
		else
			return 0;
	
	}

	if (n_bl > 0)
	{
		for (i = 0; i < rows; i ++)
		{
			if (blank_line[i] > 0)
			{
				row_out = space_srt[3*n_sp-1];
				u_l = space_srt[n_sp-1];
				u_r = space_srt[2*n_sp-1] + u_l;
				for (k = u_r; k < dura; k++)
				{
					blk[i*dura + k] = blk[row_out*dura + k];
					blk[row_out*dura + k] = 0;
				}
				return 1;
			}
		}
	}

	for (row_in = 0; row_in < rows; row_in++)
	{
		if (margin_l[row_in] > 0)
		{
			for (j = (n_sp-1); j >= 0; j--)
			{
				row_out = space_srt[2*n_sp+j];
				u_l = space_srt[j];
				u_r = space_srt[n_sp+j] + u_l;
				if ((margin_l[row_in] >= u_l) && (margin_l[row_in] < u_r))
				{
					for (k = 0; k < u_l; k++)
					{
						blk[row_in*dura + k] = blk[row_out*dura + k];
						blk[row_out*dura + k] = 0;
					}
					return 2;
				}
			}
		}

		if ((margin_r[row_in] > 0) && (margin_r[row_in] < dura))
		{
			for (j = (n_sp-1); j >= 0; j--)
			{
				row_out = space_srt[2*n_sp+j];
				u_l = space_srt[j];
				u_r = space_srt[n_sp+j] + u_l;
				if ((margin_r[row_in] > u_l) && (margin_r[row_in] <= u_r))
				{
					for (k = u_r; k < dura; k++)
					{
						blk[row_in*dura + k] = blk[row_out*dura + k];
						blk[row_out*dura + k] = 0;
					}
					return 3;
				}
			}
		}

	}

	return 0;
}


void optim_space(int n_p, int dura, int rows, int *agenda, int *blk)
{
	int i, j, k, p;

	int row_margins[3*rows];
	int u_l, u_r, b_l, b_r;

	for (i = 0; i < rows; i++)
	{
		u_l = dura;
		u_r = 0;
		for (k = 0; k < dura; k++)
		{
			if (blk[i*dura + k] > 0)
			{
				u_l = (k < u_l) ? k : u_l;
				u_r = (k > u_r) ? k : u_r;
			}
		}

		if (u_r >= u_l)
		{
			row_margins[i] = i;
			row_margins[rows + i] = u_r - u_l + 1;
			row_margins[2*rows + i] = u_l; 
		}
		else
		{
			row_margins[i] = i;
			row_margins[rows + i] = 0;
			row_margins[2*rows + i] = 0; 
		}
	}
	// 开始空间排列优化
	int blk_re[dura*rows];
	int rm_srt[3*rows];
	table_sort(rows, row_margins, rm_srt);

	for (i = 0; i < rows; i ++)
	{
		p = rm_srt[i];
		for (k = 0; k < dura; k++)
		{
			blk_re[i*dura + k] = blk[p*dura + k];
		}
	}

	for (i = 0; i < (rows-1); i++)
	{
		if (rm_srt[rows + i] == dura)
			break;
		
		u_l = rm_srt[2*rows + i];
		u_r = u_l + rm_srt[rows + i];
		b_l = u_l + 1;
		for (k = u_l + 1; k < u_r; k++)
		{
			if ((blk_re[i*dura + k] > 0) && (blk_re[i*dura + k - 1] == 0))
			{
				b_r = k;
				//printf("bl: %d; br: %d; row: %d\n", b_l, b_r, i);
				warp_space(n_p, rows, dura, i, b_l, b_r, blk_re, agenda);
			}
			b_l =  (blk_re[i*dura + k] > 0) ? k + 1 : b_l;
		}
	}

	for (i = 0; i < rows*dura; i++)
		blk[i] = blk_re[i];
	
}

void warp_space(int n, int rows, int dura, int rid, int d_start, int d_stop, int *blk, int *agenda)
{
	int i, j, k;
	int u_l, u_r;
	int mnd[n];
	int chosen[n];
	for (i = 0; i < n; i++)
	{
		mnd[i] = 0;
		chosen[i] = 0;
	}

	for (j = rid+1; j < rows; j++)
	{
		for (k = d_start; k < d_stop; k++)
			mnd[blk[j*dura+k]] = 1; 
	}

	int ch = best_choice(n, mnd, agenda, d_start, d_stop, chosen);

	if (ch > 0)
	{
		for (i = 0; i < n; i++)
		{
			if (chosen[i] > 0)
			{
				u_l = agenda[n + i];
				u_r = agenda[2*n + i];					
				
				for (k = rid+1; k < rows; k++)
				{
					if (blk[k*dura+u_l] == i)
					{
						for (j = u_l; j < u_r; j++)
						{
							blk[k*dura + j] = 0;
							blk[rid*dura + j] = i;
						}
						break;
					}
				}
			}

		}
	}

}

int best_choice(int n, int *mnd, int *agenda, int d_start, int d_stop, int *chosen)
{
	int i, j, k, p, idx;
    int waiting[n];
	int m = 0;

	for (i = 0; i < n; i++)
	{
		chosen[i] = 0;
        if ((mnd[i] > 0) && (agenda[n+i] >= d_start) && (agenda[2*n+i] <= d_stop))
        {
            m++;
            waiting[i] = 1;
        }
        else
            waiting[i] = 0;
	}

	if (m == 0)
		return 0;

    m += 2;
    waiting[0] = 1;
    waiting[n-1] = 1;
	// 为候选制造空的头和尾
	int agd_wait[3*m];
	int agd_srt[3*m];
	k = 0;
	for (i = 0; i < n; i++)
	{
		if (waiting[i] > 0)
		{
			agd_wait[k] = agenda[i];
            agd_wait[m + k] = agenda[2*n + i];
			agd_wait[2*m + k] = agenda[n + i];
			k++;
		} 
	}
    if (k != m)
    {
        printf("best_choice data check failed, k %d unequal to m %d. \n", k, m);
        exit(1);
    }

	table_sort(m, agd_wait, agd_srt);

	int dstm[m][m];
	int row_min[m];
	int relv[m];
	int u_l, u_r;
	for (i = 0; i < m; i++)
	{
		for (k = 0; k < m; k++)
		{
			u_l = agd_srt[2*m + i];
			u_r = agd_srt[m + k];
			
			dstm[i][k] = ((u_l >= u_r) && (i != k)) ? (u_l - u_r - mnd[i]) : (BIG_NUM/2);

			//printf(" %e", dstm[i][k]);
		}
		//printf("\n");
	}

	row_min[0] = 0;
	relv[0] = 0;
	for (i = 1; i < m; i++)
	{
		row_min[i] = BIG_NUM/2;
		relv[i] = BIG_NUM/2;
		for (k = 0; k < i; k++)
		{
			dstm[i][k] += row_min[k];
			if (dstm[i][k] <= row_min[i])
			{
				row_min[i] = dstm[i][k];
				relv[i] = k;
			}
		}

	}

	p = (m-1);
	while (p > 0)
	{
		p = relv[p];
		chosen[agd_srt[p]] = 1;
	}
	chosen[0] = 0;

	m = 0;
	for (k = 0; k < n; k++)
		m = (chosen[k] > 0) ? m + 1 : m;

	return m;

}
