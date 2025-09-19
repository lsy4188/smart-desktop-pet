#include "algorithm.h"

//汉明窗口，函数的目的是在时域内对信号进行平滑处理，以减小信号两端的幅度，防止频谱泄漏。
//计算公式：w(n)=0.54-0.46cos(2 pi n/N-1)，其中 N 是窗口的长度
const uint16_t auw_hamm[31]={ 41,    276,    512,    276,     41 }; //Hamm=  long16(512* hamming(5)');


//uch_spo2_table is computed as  -45.060*ratioAverage* ratioAverage + 30.354 *ratioAverage + 94.845 ;
//ratioAverage 是一个用于计算血氧饱和度的比值。这个比值与红外光和红光的交流和直流分量的比值有关
const uint8_t uch_spo2_table[184]={ 95, 95, 95, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99, 
                            99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 
                            100, 100, 100, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98, 98, 98, 97, 97, 
                            97, 97, 96, 96, 96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91, 
                            90, 90, 89, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84, 83, 82, 82, 81, 81, 
                            80, 80, 79, 78, 78, 77, 76, 76, 75, 74, 74, 73, 72, 72, 71, 70, 69, 69, 68, 67, 
                            66, 66, 65, 64, 63, 62, 62, 61, 60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 
                            49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29, 
                            28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 12, 11, 10, 9, 7, 6, 5, 
                            3, 2, 1 } ;

static  int32_t an_dx[ BUFFER_SIZE-MA4_SIZE]; // delta
static  int32_t an_x[ BUFFER_SIZE]; //ir
static  int32_t an_y[ BUFFER_SIZE]; //red

void maxim_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer,  int32_t n_ir_buffer_length, uint32_t *pun_red_buffer, int32_t *pn_spo2, int8_t *pch_spo2_valid, 
                              int32_t *pn_heart_rate, int8_t  *pch_hr_valid)
/**
* \brief        Calculate the heart rate and SpO2 level
* \par          Details
*               通过检测PPG周期的峰值和相应的红/红外信号的AC/DC，计算出SPO2的比值。
*				因为这个算法的目标是Arm M0/M3。由于寄存器溢出，SPO2的公式没有达到精度。
*				因此，准确的SPO2是预先计算出来的，并且每个比率都节省了很长的uch_spo2_table[]。
*
* \param[in]	*pun_ir_buffer 				-红外传感器数据缓冲区
* \param[in]	n_ir_buffer_length 			-红外传感器数据缓冲区长度
* \param[in]	*pun_red_buffer 			-红光传感器数据缓冲区
* \param[out]	*pn_spo2 					-计算的SpO2值
* \param[out]	*pch_spo2_valid 			-如果计算的SpO2值有效，则为1
* \param[out]	*pn_heart_rate 				-计算心率值
* \param[out]	*pch_hr_valid 				- 1如果计算的心率值是有效的

*				存储计算结果的指针pn_spo2、pch_spo2_valid、pn_heart_rate和pch_hr_valid。
* \retval       None



* 实现步骤：
	主要实现过程包括：
	（1）去除IR信号的直流成分：
		 计算IR信号的平均值，然后从原始IR信号中减去平均值，得到去除直流成分的信号。
	
	（2）平滑信号：
		 对去除直流成分的IR信号进行4点移动平均。
	
	（3）计算信号差分：
		 计算平滑后的IR信号的差分。
	
	（4）使用汉明窗口翻转波形：
		 对差分信号使用汉明窗口，以便在后续步骤中检测波谷。
	
	（5）使用峰值检测找到波谷：
		 使用峰值检测算法找到波谷的位置，并保存在an_ir_valley_locs数组中。
		 
	（6）计算心率：
		 根据波谷的位置计算心率，单位为每分钟心跳数（BPM）。
		 
	（7）计算IR和红光信号的AC/DC比值：
		 在两个相邻波谷之间，找到IR和红光信号的AC（交流）和DC（直流）成分，并计算AC/DC比值。
		 在每个心跳周期内，计算红外和红光信号的AC分量和DC分量。
		 AC分量反映了血液的脉动变化，而DC分量反映了总体的血液吸收
		 
	（8）使用比值计算血氧饱和度：
		 使用IR和红光信号的AC/DC比值，查表计算血氧饱和度。结果存储在pn_spo2中。
		 红外和红光的AC/DC比值与血液的含氧量相关，因为含氧血液和脱氧血液对红外和红光的吸收比例不同
		 使用预先计算好的SPO2校准表，将红外和红光的AC/DC比值映射到实际的SPO2值。
*/
{
	uint32_t un_ir_mean, un_only_once;		// IR信号的均值和标志位，用于去除直流（DC）成分
	int32_t k, n_i_ratio_count;				// 循环变量k，AC/DC比值计数器
	int32_t i, s, m, n_exact_ir_valley_locs_count, n_middle_idx;
	// 循环变量i，临时变量s，计数器m，精确IR谷点数目，中间索引
	int32_t n_th1, n_npks, n_c_min;			// 阈值n_th1，峰值计数n_npks，最小值n_c_min
	int32_t an_ir_valley_locs[15];			// IR信号谷点的数组
	int32_t an_exact_ir_valley_locs[15];	// 精确IR信号谷点的数组
	int32_t an_dx_peak_locs[15];			// 微分信号峰点的数组
	int32_t n_peak_interval_sum;			// 峰值间隔总和

	int32_t n_y_ac, n_x_ac;					// 红光和红外光的AC（交流）分量
	int32_t n_spo2_calc;					// 计算得到的SPO2值
	int32_t n_y_dc_max, n_x_dc_max;			// 红光和红外光的DC（直流）最大值
	int32_t n_y_dc_max_idx, n_x_dc_max_idx; // 红光和红外光的DC最大值索引
	int32_t an_ratio[5], n_ratio_average;	// AC/DC比值数组和比值平均值
	int32_t n_nume, n_denom;				// 分子和分母，用于计算AC/DC比值


    // 去除IR信号的直流成分
    un_ir_mean = 0;
    for (k = 0; k < n_ir_buffer_length; k++) un_ir_mean += pun_ir_buffer[k];
    un_ir_mean = un_ir_mean / n_ir_buffer_length;
    for (k = 0; k < n_ir_buffer_length; k++) an_x[k] = pun_ir_buffer[k] - un_ir_mean;

    // 4点移动平均
    for (k = 0; k < BUFFER_SIZE - MA4_SIZE; k++)
    {
		// 计算4个点的和
        n_denom = (an_x[k] + an_x[k + 1] + an_x[k + 2] + an_x[k + 3]);
		// 计算移动平均值并替换原始值
        an_x[k] = n_denom / (int32_t)4;
    }

    // 计算信号差分，计算信号an_x的差分，即相邻元素之间的差值。
	//通过对原始信号进行差分运算，可以获取信号的变化率，有助于后续的信号处理和特征提取。
    for (k = 0; k < BUFFER_SIZE - MA4_SIZE - 1; k++)
        an_dx[k] = (an_x[k + 1] - an_x[k]);

    // 2点移动平均
    for (k = 0; k < BUFFER_SIZE - MA4_SIZE - 2; k++)
    {
        an_dx[k] = (an_dx[k] + an_dx[k + 1]) / 2;
    }

	// 汉明窗口
	//汉明窗口是一种窗口函数，通常用于在信号处理中对信号进行平滑处理，以减少频谱泄漏。
	//在这里，通过对差分信号应用汉明窗口，可以对信号进行加权，突出信号中的特定频率成分。
	//这个加权后的信号被用于后续的信号处理步骤，以提取相关的特征或信息。
	for (i = 0; i < BUFFER_SIZE - HAMMING_SIZE - MA4_SIZE - 2; i++)
	{
		s = 0;
		// 应用汉明窗口对信号进行加权
		for (k = i; k < i + HAMMING_SIZE; k++)
		{
			// 汉明窗口的权重数组为 auw_hamm
			// 将差分信号 an_dx 与汉明窗口加权，并求和
			s -= an_dx[k] * auw_hamm[k - i];
		}
		// 将加权和除以汉明窗口权重之和（1146）,归一化
		an_dx[i] = s / (int32_t)1146;
	}
	

	// 阈值计算，计算差分信号数组 an_dx 的平均阈值 n_th1
	n_th1 = 0; // 初始化阈值
	// 遍历差分信号数组 an_dx，计算阈值
	for (k = 0; k < BUFFER_SIZE - HAMMING_SIZE; k++)
	{
		// 阈值为正差分信号之和，如果差分信号为负值，则取绝对值累加
		n_th1 += ((an_dx[k] > 0) ? an_dx[k] : ((int32_t)0 - an_dx[k]));
	}
	// 将阈值除以数组长度，得到平均阈值
	n_th1 = n_th1 / (BUFFER_SIZE - HAMMING_SIZE);

	
	
	// 使用峰值检测找到波谷，计算心率
	/*
	思路：
	maxim_find_peaks 函数找到差分信号数组 an_dx 中的峰值位置，并将这些位置存储在 an_dx_peak_locs 数组中
	然后，通过计算峰值之间的平均间隔，利用心率计算公式计算心率值。
	最后，根据检测到的峰值位置，计算波谷位置，并将波谷位置存储在 an_ir_valley_locs 数组中。
	*/
	
	maxim_find_peaks(an_dx_peak_locs, &n_npks, an_dx, BUFFER_SIZE - HAMMING_SIZE, n_th1, 8, 5);

	n_peak_interval_sum = 0;

	// 如果检测到至少两个峰值
	if (n_npks >= 2)
	{
		// 计算峰值之间的平均间隔
		for (k = 1; k < n_npks; k++)
			n_peak_interval_sum += (an_dx_peak_locs[k] - an_dx_peak_locs[k - 1]);
		
		// 计算心率（每分钟心跳数）
		n_peak_interval_sum = n_peak_interval_sum / (n_npks - 1);
		*pn_heart_rate = (int32_t)(6000 / n_peak_interval_sum);
		
		// 设置心率有效标志为1
		*pch_hr_valid = 1;
	}
	else
	{
		// 如果未检测到足够的峰值，设置心率为无效值-999
		*pn_heart_rate = -999;
		
		// 设置心率有效标志为0
		*pch_hr_valid = 0;
	}

	// 将峰值位置加上 HAMMING_SIZE / 2 作为波谷位置，为后面寻找准确的波谷做准备
	for (k = 0; k < n_npks; k++)
		an_ir_valley_locs[k] = an_dx_peak_locs[k] + HAMMING_SIZE / 2;

	

	// 计算AC/DC比值，从原始的红外和红光信号中提取出用于计算的实际波谷位置
	/*
	首先，将原始的红外信号存储在 an_x 数组中，红光信号存储在 an_y 数组中。
	然后，遍历波谷位置数组，寻找每个波谷的实际位置。通过在波谷位置的前后寻找最小值，确定波谷的确切位置，
	并将这些位置存储在 an_exact_ir_valley_locs 数组中。
	*/
	for (k = 0; k < n_ir_buffer_length; k++)
	{
		an_x[k] = pun_ir_buffer[k];  // 将红外信号存储在 an_x 数组中
		an_y[k] = pun_red_buffer[k]; // 将红光信号存储在 an_y 数组中
	}

	n_exact_ir_valley_locs_count = 0;

	// 遍历波谷位置数组，找到每个波谷的实际位置
	for (k = 0; k < n_npks; k++)
	{
		un_only_once = 1;
		m = an_ir_valley_locs[k];
		n_c_min = 16777216; // 设置一个较大的初始值，2^24

		// 在波谷位置的前后寻找最小值，以确定波谷的实际位置
		if (m + 5 < BUFFER_SIZE - HAMMING_SIZE && m - 5 > 0)
		//检查实际红外信号谷值位置 m 前后是否有足够的空间，以防数组越界。
		{
			for (i = m - 5; i < m + 5; i++)
			//循环遍历从当前谷值位置向前后各取5个点的范围。
			{
				if (an_x[i] < n_c_min)
				{
					if (un_only_once > 0)
					{
						//第一次找到最小值，将其设为0以标记已经找到最小值。
						un_only_once = 0;
					}
					n_c_min = an_x[i];
					an_exact_ir_valley_locs[k] = i;
				}
			}
			
			// 如果找到了最小值，增加实际波谷位置的计数
			if (un_only_once == 0)
				n_exact_ir_valley_locs_count++;
		}
	}

	// 波谷是信号的极小值，用于确定每个心跳周期的起始点。如果确切波谷位置的数量小于2，说明信号比值超出范围，设置SPO2为无效值-999
	if (n_exact_ir_valley_locs_count < 2)
	{
		*pn_spo2 = -999;
		*pch_spo2_valid = 0;
		return;
	}


    // 4点移动平均，平滑信号并降低高频噪声的影响
    for (k = 0; k < BUFFER_SIZE - MA4_SIZE; k++)
    {
        an_x[k] = (an_x[k] + an_x[k + 1] + an_x[k + 2] + an_x[k + 3]) / (int32_t)4;
        an_y[k] = (an_y[k] + an_y[k + 1] + an_y[k + 2] + an_y[k + 3]) / (int32_t)4;
    }

    // 使用an_exact_ir_valley_locs，找到IR-RED DC和IR-RED AC进行SPO2校准比率
	//使用先前找到的红外信号的谷位置（an_exact_ir_valley_locs），来计算IR-RED DC和IR-RED AC的比率。用于SPO2（血氧饱和度）的校准。
    n_ratio_average = 0;
    n_i_ratio_count = 0;

    for (k = 0; k < 5; k++)
        an_ratio[k] = 0;
    for (k = 0; k < n_exact_ir_valley_locs_count; k++)
    {
        if (an_exact_ir_valley_locs[k] > BUFFER_SIZE)
        {
            *pn_spo2 = -999;   // 因为谷位置超出范围，不是正确的SPO2
            *pch_spo2_valid = 0;
            return;
        }
    }

	/*
	计算红光和红外信号的交流分量的乘积，并将计算得到的AC/DC比值保存到数组中。
	其中，通过找到两个谷值之间的最大值来估算直流分量，然后计算交流分量，并最终得到AC/DC比值。
	整个过程旨在对信号进行预处理，为后续的SPO2（血氧饱和度）计算提供必要的信息。
	*/
	
	for (k = 0; k < n_exact_ir_valley_locs_count - 1; k++)
	{
		n_y_dc_max = -16777216;   // 初始化红光信号的最大直流分量为一个较小的负数
		n_x_dc_max = -16777216;   // 初始化红外信号的最大直流分量为一个较小的负数

		// 判断两个谷值之间的距离是否大于10个样本点，确保两谷之间有足够的数据进行计算
		if (an_exact_ir_valley_locs[k + 1] - an_exact_ir_valley_locs[k] > 10)
		{
			// 遍历两个谷值之间的数据
			for (i = an_exact_ir_valley_locs[k]; i < an_exact_ir_valley_locs[k + 1]; i++)
			{
				// 找到红外信号的最大值及对应索引
				if (an_x[i] > n_x_dc_max)
				{
					n_x_dc_max = an_x[i];
					n_x_dc_max_idx = i;
				}

				// 找到红光信号的最大值及对应索引
				if (an_y[i] > n_y_dc_max)
				{
					n_y_dc_max = an_y[i];
					n_y_dc_max_idx = i;
				}
			}

			// 计算红光的交流分量
			//计算红光信号在两个相邻谷值之间的交流成分，使用两个谷值之间的光强差值乘以相邻两个峰谷之间的距离。
			
			//an_y: 这是存储红光信号光强数据的数组
			//an_exact_ir_valley_locs: 这是存储实际红外信号波谷位置的数组
			//n_y_dc_max_idx，红光信号的最大峰值位置的索引
			
			n_y_ac = (an_y[an_exact_ir_valley_locs[k + 1]] - an_y[an_exact_ir_valley_locs[k]]) * (n_y_dc_max_idx - an_exact_ir_valley_locs[k]);
			//将得到的红光的交流分量加到红光信号在前一个谷值位置的基线值上
			n_y_ac = an_y[an_exact_ir_valley_locs[k]] + n_y_ac / (an_exact_ir_valley_locs[k + 1] - an_exact_ir_valley_locs[k]);

			// 消除红光信号的直流分量
			n_y_ac = an_y[n_y_dc_max_idx] - n_y_ac;
			//写成abs(n_y_ac - an_y[n_y_dc_max_idx])是否可以？

			// 计算红外光的交流分量
			n_x_ac = (an_x[an_exact_ir_valley_locs[k + 1]] - an_x[an_exact_ir_valley_locs[k]]) * (n_x_dc_max_idx - an_exact_ir_valley_locs[k]);
			n_x_ac = an_x[an_exact_ir_valley_locs[k]] + n_x_ac / (an_exact_ir_valley_locs[k + 1] - an_exact_ir_valley_locs[k]);

			// 消除红外信号的直流分量
			n_x_ac = an_x[an_exact_ir_valley_locs[k + 1]] - n_x_ac;

			// 将红光信号的AC分量与红外信号最大峰值位置的直流分量相乘，作为计算SPO2的分子
			n_nume = (n_y_ac * n_x_dc_max) >> 7;

			// 将红外信号的AC分量与红光信号最大峰值位置的直流分量相乘，作为计算SPO2的分母
			n_denom = (n_x_ac * n_y_dc_max) >> 7;

			// 判断分母是否大于0，避免除零错误，并判断是否满足条件，满足则进行下一步计算
			if (n_denom > 0 && n_i_ratio_count < 5 && n_nume != 0)
			{
				// 计算AC/DC比值并保存到数组中
				an_ratio[n_i_ratio_count] = (n_nume * 20) / n_denom;
				n_i_ratio_count++;
			}
		}
	}

	/*
	对排序后的AC/DC比值数组进行处理，计算得到中值平均（或直接取中值），并通过判断得到的AC/DC比值
	是否在有效范围内，确定最终的SPO2值。如果AC/DC比值有效，通过查表获取对应的SPO2值，否则将SPO2
	值设为无效。在实际应用中，通过对AC/DC比值的处理，结合预先计算的SPO2表格，可以更准确地估算血氧饱和度。
	*/

	// 将存储AC/DC比值的数组按升序排列
	maxim_sort_ascend(an_ratio, n_i_ratio_count);

	// 计算中间值的索引
	n_middle_idx = n_i_ratio_count / 2;

	// 如果中间索引大于1，使用中值平均计算AC/DC比值
	if (n_middle_idx > 1)
		n_ratio_average = (an_ratio[n_middle_idx - 1] + an_ratio[n_middle_idx]) / 2;
	else
		n_ratio_average = an_ratio[n_middle_idx];

	// 判断计算得到的AC/DC比值是否在有效范围内
	if (n_ratio_average > 2 && n_ratio_average < 184)
	{
		// 通过查表获取对应的SPO2值
		n_spo2_calc = uch_spo2_table[n_ratio_average];
		*pn_spo2 = n_spo2_calc;
		*pch_spo2_valid = 1; // 设置SPO2有效标志

		// 可选：用以下公式计算浮点数SPO2，与查表结果进行比较
		// float_SPO2 = -45.060 * n_ratio_average * n_ratio_average / 10000 + 30.354 * n_ratio_average / 100 + 94.845;
	}
	else
	{
		// 如果计算得到的AC/DC比值不在有效范围内，设置SPO2为无效值
		*pn_spo2 = -999;
		*pch_spo2_valid = 0;
	}

}


void maxim_find_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num)
/**
* \brief        Find peaks
* \par          Details
*               Find at most MAX_NUM peaks above MIN_HEIGHT separated by at least MIN_DISTANCE
*
* \retval       None
*/
{
    maxim_peaks_above_min_height( pn_locs, pn_npks, pn_x, n_size, n_min_height );
    maxim_remove_close_peaks( pn_locs, pn_npks, pn_x, n_min_distance );
    *pn_npks = min( *pn_npks, n_max_num );
}

void maxim_peaks_above_min_height(int32_t *pn_locs, int32_t *pn_npks, int32_t  *pn_x, int32_t n_size, int32_t n_min_height)
/**
* \brief        Find peaks above n_min_height
* \par          Details
*               Find all peaks above MIN_HEIGHT
*
* \retval       None
*/
{
    int32_t i = 1, n_width;
    *pn_npks = 0;
    
    while (i < n_size-1){
        if (pn_x[i] > n_min_height && pn_x[i] > pn_x[i-1]){            // find left edge of potential peaks
            n_width = 1;
            while (i+n_width < n_size && pn_x[i] == pn_x[i+n_width])    // find flat peaks
                n_width++;
            if (pn_x[i] > pn_x[i+n_width] && (*pn_npks) < 15 ){                            // find right edge of peaks
                pn_locs[(*pn_npks)++] = i;        
                // for flat peaks, peak location is left edge
                i += n_width+1;
            }
            else
                i += n_width;
        }
        else
            i++;
    }
}


void maxim_remove_close_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance)
/**
* \brief        Remove peaks
* \par          Details
*               Remove peaks separated by less than MIN_DISTANCE
*
* \retval       None
*/
{
    
    int32_t i, j, n_old_npks, n_dist;
    
    /* Order peaks from large to small */
    maxim_sort_indices_descend( pn_x, pn_locs, *pn_npks );

    for ( i = -1; i < *pn_npks; i++ ){
        n_old_npks = *pn_npks;
        *pn_npks = i+1;
        for ( j = i+1; j < n_old_npks; j++ ){
            n_dist =  pn_locs[j] - ( i == -1 ? -1 : pn_locs[i] ); // lag-zero peak of autocorr is at index -1
            if ( n_dist > n_min_distance || n_dist < -n_min_distance )
                pn_locs[(*pn_npks)++] = pn_locs[j];
        }
    }

    // Resort indices longo ascending order
    maxim_sort_ascend( pn_locs, *pn_npks );
}

void maxim_sort_ascend(int32_t *pn_x,int32_t n_size) 
/**
* \brief        Sort array
* \par          Details
*               Sort array in ascending order (insertion sort algorithm)
*
* \retval       None
*/
{
    int32_t i, j, n_temp;
    for (i = 1; i < n_size; i++) {
        n_temp = pn_x[i];
        for (j = i; j > 0 && n_temp < pn_x[j-1]; j--)
            pn_x[j] = pn_x[j-1];
        pn_x[j] = n_temp;
    }
}

void maxim_sort_indices_descend(int32_t *pn_x, int32_t *pn_indx, int32_t n_size)
/**
* \brief        Sort indices
* \par          Details
*               Sort indices according to descending order (insertion sort algorithm)
*
* \retval       None
*/ 
{
    int32_t i, j, n_temp;
    for (i = 1; i < n_size; i++) {
        n_temp = pn_indx[i];
        for (j = i; j > 0 && pn_x[n_temp] > pn_x[pn_indx[j-1]]; j--)
            pn_indx[j] = pn_indx[j-1];
        pn_indx[j] = n_temp;
    }
}


