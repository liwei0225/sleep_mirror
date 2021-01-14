void FFTTestTask(void)
{
	OS_ERR err;
	CPU_TS ts;
	arm_cfft_radix4_instance_f32 scfft;
	int i = 0;
	unsigned char str[10];
	
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);
	while(1)
	{
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);
		for(i=0;i<FFT_LENGTH;i++)
		{
			fft.fft_input[2*i] = (float)fft.ADC_ConvertedValue[i]*3.3f/4096.0f;
			fft.fft_input[2*i+1] = 0;
		}		
		arm_cfft_radix4_f32(&scfft,fft.fft_input);
		arm_cmplx_mag_f32(fft.fft_input,fft.fft_output,FFT_LENGTH);
		for(i=0;i<FFT_LENGTH;i++)
		{
			sprintf((char*)str,"%.2f\r\n",fft.fft_output[i]);
			board.UART4Send(str,strlen((char*)str));
			OSTimeDly(1,OS_OPT_TIME_DLY,&err);
		}
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
		board.ADC1_DMA2Enable();
	}
}