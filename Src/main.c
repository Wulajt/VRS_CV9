/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "lis3mdltr.h"
#include "lsm6ds0.h"
#include "tim.h"
#include "display.h"
#include "lps25hb.h"

uint8_t temp = 0;
float mag[3], acc[3];

void SystemClock_Config(void);

extern uint64_t disp_time;
uint64_t saved_time;
double num_to_display = 10;

int get_altitude(int8_t temp, int16_t press){
    double tmp1 = 1013/(double)press;
    double tmp2 = pow(tmp1, (1/5.257));
    int alti = ((tmp2 - 1)*(temp + 273))/0.0065;
    return alti;
}

int main(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();

  setSegments();
  setDigits();

  LL_mDelay(2000);

  resetDigits();
  resetSegments();

  MX_TIM3_Init();
  uint8_t lps = lps25hb_init();
  uint8_t hts = hts221_init();
  uint8_t lsm = lsm6ds0_init();

  uint8_t text[21] = "branislav_kutas_98344";
  uint8_t txtIndex = 0;
  uint8_t cycleRight = 1;

  while (1)
  {
	  int16_t lps_press = lps25hb_get_press();
	  int8_t lps_temp = lps25hb_get_temp();
	  //int8_t lsm_temp = lsm6ds0_get_temp();
	  //int8_t hts_temp = hts221_get_temp();
	  int8_t hts_humi = hts221_get_humi();
	  int alt = get_altitude(lps_temp, lps_press);

	  if(disp_time > (saved_time + 500))
	         {
	             uint8_t toDisplay[4] = "";

	             if (txtIndex >= 17){
	                 cycleRight = 0;
	             }
	             else if (txtIndex <= 0){
	                 cycleRight = 1;
	             }

	             for (int i = 0; i<4; i++){
	                 toDisplay[i] = text[i + txtIndex];
	             }

	             if (cycleRight){
	                 txtIndex++;
	             }
	             else {
	                 txtIndex--;
	             }

	             displayString(toDisplay);

	             saved_time = disp_time;
	         }
	    }


	  //os			   x      y        z
//	  lsm6ds0_get_acc(acc, (acc+1), (acc+2));
//	  LL_mDelay(50);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_HSI);
  LL_SYSTICK_EnableIT();
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
