/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块
*	文件名称 : main.c
*	版    本 : V1.1
*	说    明 : 学习串口与PC通信
*              实验目的：
*                1. 学习串口与PC通信。
*              实验内容：
*                1. 启动一个自动重装软件定时器，每100ms翻转一次LED2。
*              实验操作：
*                1. 串口接收到字符命令'1'，返回串口消息"接收到串口命令1"。
*                2. 串口接收到字符命令'2'，返回串口消息"接收到串口命令2"。
*                3. 串口接收到字符命令'3'，返回串口消息"接收到串口命令3"。
*                4. 串口接收到字符命令'4'，返回串口消息"接收到串口命令4"。
*                5. K1按键按下，串口打印"按键K1按下"。
*                6. K2按键按下，串口打印"按键K2按下"。
*                7. K3按键按下，串口打印"按键K3按下"。
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT查看打印信息，波特率115200，数据位8，奇偶校验位无，停止位1。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2019-08-17   Eric2013     1. CMSIS软包版本 V5.5.0
*                                         2. HAL库版本 V2.4.0
*		V1.1    2020-04-06   Eric2013     1. 更新串口标志清除错误。
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "bsp.h"			/* 底层硬件驱动 */
#include "arm_math.h"
#include "arm_const_structs.h"
#include "stdio.h"


/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V5-串口和PC机通信（驱动支持6个串口FIFO）"
#define EXAMPLE_DATE	"2020-04-06"
#define DEMO_VER		"1.1"

//static void arm_rfft_fast_f32_app(void);
static void arm_cfft_f32_app(void);
/* 输入和输出缓冲 */
#define TEST_LENGTH_SAMPLES 2048  
static float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float32_t testOutput_f32_10khz[TEST_LENGTH_SAMPLES];
static float32_t testOutput[TEST_LENGTH_SAMPLES];
void xcorr_test(void);
void  xcorr_func(float32_t * sig , int num ,float32_t *conq);


#define FFT_LENGTH 1024
float32_t fft_input[FFT_LENGTH*2];
float32_t fft_output[FFT_LENGTH*2];
float32_t ADC_ConvertedValue[FFT_LENGTH];


uint32_t fftSize = 1024; 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1; 

static uint8_t flag = 0;
static 	arm_rfft_instance_q15 S;

#define NUM 9
float a[NUM]={1,5,9,8,7,6,7,8,9};
float xcorr_out[NUM*2-1];
//extern float32_t testInput_f32_10khz[2048];

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参: 无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	//uint8_t ucKeyCode;	
	uint8_t read;
	
	const char buf1[] = "接收到串口命令1\r\n";

	bsp_Init();		/* 硬件初始化 */
	

	bsp_StartAutoTimer(0, 100);	/* 启动1个100ms的自动重装的定时器 */
	
	/* 主程序大循环 */
	while (1)
	{
		/* CPU bsp.c */
		bsp_Idle();	

/**/
		if (comGetChar(COM1, &read))
		{
			switch (read)
			{
				case '1':
					comSendBuf(COM1, (uint8_t *)buf1, strlen(buf1));
					arm_cfft_f32_app();
					//xcorr_test();
					break;
				default:
					break;
			}
		}
		
	}
}
/*add corr with unbiased */
void  xcorr_func(float32_t * sig , int num ,float32_t *conq)
{
	int lenth=num;
	for (int i=0;i<=lenth;i++)
	{
		for (int j=0;j<=i;j++)
		{
			conq[i]+=sig[j]*sig[lenth-1-i+j];
		}
 
	}
	
	for(int i=0;i<=lenth;i++)
	{
	  conq[i]=(conq[i]/(i+1));
	}


	for (int i=lenth;i<(lenth<<1)-1;i++)
	{
		conq[i]=conq[(lenth<<1)-2-i];
	}
	
}

void xcorr_test(void)
{
   xcorr_func(a, NUM, xcorr_out);
   for(int i = 0; i<2*NUM-1;i++)
   {
		printf("%f\r\n", xcorr_out[i]);
   }
   
}




void FFTTask(void)
{
	uint16_t i;
	arm_cfft_instance_f32 S;
	
	/* ????FFT?? */
	fftSize = 1024; 
	/* ??? */
  ifftFlag = 0; 
/*store the adc data as complex data*/
	for(i=0;i<FFT_LENGTH;i++)
	{
		fft_input[2*i] = (float)ADC_ConvertedValue[i]*3.3f;
		fft_input[2*i+1] = 0;
	}		
		/* 1024 complex FFT caculation*/ 
	arm_cfft_f32(&S, fft_input, ifftFlag,1);	
	arm_cmplx_mag_f32(fft_input, testOutput, fftSize);

	/* print the fft data */
	for(i=0; i<fftSize; i++)
	{
		printf("%f\r\n", testOutput[i]);
	}
	/*run the ifft caculate*/
	ifftFlag = 1;
	arm_cfft_f32(&S,fft_input, ifftFlag,1);	
	
}
#if 0
/*
*********************************************************************************************************
*	函 数 名: arm_rfft_fast_f32_app
*	功能说明: 调用函数arm_rfft_fast_f32计算1024点实数序列的幅频响应并跟使用函数arm_cfft_f32计算的结果做对比。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void arm_rfft_fast_f32_app(void)
{
	uint16_t i;
	arm_rfft_fast_instance_f32 S;
	
	/* 实数序列FFT长度 */
	fftSize = 1024; 
	/* 正变换 */
    ifftFlag = 0; 
	
	/* 初始化结构体S中的参数 */
 	arm_rfft_fast_init_f32(&S, fftSize);
	
	printf("print the raw data.\r\n");
	/* 按照实部，实部，....的顺序存储数据 */
	for(i=0; i<1024; i++)
	{
		/* 50Hz正弦波，采样率1KHz */
		testInput_f32_10khz[i] = 1.2f*arm_sin_f32(2*3.1415926f*50*i/1000)+1;
		printf("%f\r\n", testInput_f32_10khz[i]);
	}
	
	/* 1024点实序列快速FFT */ 
	arm_rfft_fast_f32(&S, testInput_f32_10khz, testOutput_f32_10khz, ifftFlag);
	
	printf("This is the fft output value");
	for(i=0;i<2048;i++)
	{
		printf("%f\r\n", testOutput_f32_10khz[i]);

	}
	
	/* 为了方便跟函数arm_cfft_f32计算的结果做对比，这里求解了1024组模值，实际函数arm_rfft_fast_f32
	   只求解出了512组  
	*/ 
 	arm_cmplx_mag_f32(testOutput_f32_10khz, testOutput, fftSize*2);
	printf("This is the mode value");
	/* 串口打印求解的模值 */
	for(i=0; i<fftSize; i++)
	{
		//printf("%f\r\n", testOutput[i]);
	}
	#if 0
	/*ifft*/
    ifftFlag = 1; 
		arm_rfft_fast_f32(&S, testOutput_f32_10khz, testInput_f32_10khz, ifftFlag);
		/* print the data with ifft */
	printf("print the ifft data. \r\n");
	for(i=0; i<fftSize; i++)
	{
		printf("%f\r\n", testInput_f32_10khz[i]);
	}
	#endif

}
#endif
/*
*********************************************************************************************************
*	函 数 名: arm_cfft_f32_app
*	功能说明: 调用函数arm_cfft_f32_app计算幅频。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void arm_cfft_f32_app(void)
{
	uint16_t i;
	
	fftSize = 1024; 
    ifftFlag = 0; 
    doBitReverse = 1; 
	
	/* 按照实部，虚部，实部，虚部..... 的顺序存储数据 */
	for(i=0; i<1024; i++)
	{
		testInput_f32_10khz[i*2+1] = 0;
		
		/* 50Hz正弦波，采样率1KHz */
		testInput_f32_10khz[i*2] = arm_sin_f32(2*3.1415926f*50*i/1000);
	}
	
	/* CFFT变换 */ 
	arm_cfft_f32(&arm_cfft_sR_f32_len1024, testInput_f32_10khz, ifftFlag, doBitReverse);

	/* 求解模值  */ 
	arm_cmplx_mag_f32(testInput_f32_10khz, testOutput, fftSize);
	
	/* 串口打印求解的模值 */
	for(i=0; i<1024; i++)
	{
		printf("%f\r\n", testOutput[i]);
	}

}
