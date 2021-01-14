/*
*********************************************************************************************************
*
*	ģ������ : ������ģ��
*	�ļ����� : main.c
*	��    �� : V1.1
*	˵    �� : ѧϰ������PCͨ��
*              ʵ��Ŀ�ģ�
*                1. ѧϰ������PCͨ�š�
*              ʵ�����ݣ�
*                1. ����һ���Զ���װ�����ʱ����ÿ100ms��תһ��LED2��
*              ʵ�������
*                1. ���ڽ��յ��ַ�����'1'�����ش�����Ϣ"���յ���������1"��
*                2. ���ڽ��յ��ַ�����'2'�����ش�����Ϣ"���յ���������2"��
*                3. ���ڽ��յ��ַ�����'3'�����ش�����Ϣ"���յ���������3"��
*                4. ���ڽ��յ��ַ�����'4'�����ش�����Ϣ"���յ���������4"��
*                5. K1�������£����ڴ�ӡ"����K1����"��
*                6. K2�������£����ڴ�ӡ"����K2����"��
*                7. K3�������£����ڴ�ӡ"����K3����"��
*              ע�����
*                1. ��ʵ���Ƽ�ʹ�ô������SecureCRT�鿴��ӡ��Ϣ��������115200������λ8����żУ��λ�ޣ�ֹͣλ1��
*                2. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2019-08-17   Eric2013     1. CMSIS����汾 V5.5.0
*                                         2. HAL��汾 V2.4.0
*		V1.1    2020-04-06   Eric2013     1. ���´��ڱ�־�������
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/	
#include "bsp.h"			/* �ײ�Ӳ������ */
#include "arm_math.h"
#include "arm_const_structs.h"
#include "stdio.h"


/* ���������������̷������� */
#define EXAMPLE_NAME	"V5-���ں�PC��ͨ�ţ�����֧��6������FIFO��"
#define EXAMPLE_DATE	"2020-04-06"
#define DEMO_VER		"1.1"

//static void arm_rfft_fast_f32_app(void);
static void arm_cfft_f32_app(void);
/* ������������ */
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
*	�� �� ��: main
*	����˵��: c�������
*	��    ��: ��
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	//uint8_t ucKeyCode;	
	uint8_t read;
	
	const char buf1[] = "���յ���������1\r\n";

	bsp_Init();		/* Ӳ����ʼ�� */
	

	bsp_StartAutoTimer(0, 100);	/* ����1��100ms���Զ���װ�Ķ�ʱ�� */
	
	/* �������ѭ�� */
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
*	�� �� ��: arm_rfft_fast_f32_app
*	����˵��: ���ú���arm_rfft_fast_f32����1024��ʵ�����еķ�Ƶ��Ӧ����ʹ�ú���arm_cfft_f32����Ľ�����Աȡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void arm_rfft_fast_f32_app(void)
{
	uint16_t i;
	arm_rfft_fast_instance_f32 S;
	
	/* ʵ������FFT���� */
	fftSize = 1024; 
	/* ���任 */
    ifftFlag = 0; 
	
	/* ��ʼ���ṹ��S�еĲ��� */
 	arm_rfft_fast_init_f32(&S, fftSize);
	
	printf("print the raw data.\r\n");
	/* ����ʵ����ʵ����....��˳��洢���� */
	for(i=0; i<1024; i++)
	{
		/* 50Hz���Ҳ���������1KHz */
		testInput_f32_10khz[i] = 1.2f*arm_sin_f32(2*3.1415926f*50*i/1000)+1;
		printf("%f\r\n", testInput_f32_10khz[i]);
	}
	
	/* 1024��ʵ���п���FFT */ 
	arm_rfft_fast_f32(&S, testInput_f32_10khz, testOutput_f32_10khz, ifftFlag);
	
	printf("This is the fft output value");
	for(i=0;i<2048;i++)
	{
		printf("%f\r\n", testOutput_f32_10khz[i]);

	}
	
	/* Ϊ�˷��������arm_cfft_f32����Ľ�����Աȣ����������1024��ģֵ��ʵ�ʺ���arm_rfft_fast_f32
	   ֻ������512��  
	*/ 
 	arm_cmplx_mag_f32(testOutput_f32_10khz, testOutput, fftSize*2);
	printf("This is the mode value");
	/* ���ڴ�ӡ����ģֵ */
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
*	�� �� ��: arm_cfft_f32_app
*	����˵��: ���ú���arm_cfft_f32_app�����Ƶ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void arm_cfft_f32_app(void)
{
	uint16_t i;
	
	fftSize = 1024; 
    ifftFlag = 0; 
    doBitReverse = 1; 
	
	/* ����ʵ�����鲿��ʵ�����鲿..... ��˳��洢���� */
	for(i=0; i<1024; i++)
	{
		testInput_f32_10khz[i*2+1] = 0;
		
		/* 50Hz���Ҳ���������1KHz */
		testInput_f32_10khz[i*2] = arm_sin_f32(2*3.1415926f*50*i/1000);
	}
	
	/* CFFT�任 */ 
	arm_cfft_f32(&arm_cfft_sR_f32_len1024, testInput_f32_10khz, ifftFlag, doBitReverse);

	/* ���ģֵ  */ 
	arm_cmplx_mag_f32(testInput_f32_10khz, testOutput, fftSize);
	
	/* ���ڴ�ӡ����ģֵ */
	for(i=0; i<1024; i++)
	{
		printf("%f\r\n", testOutput[i]);
	}

}
