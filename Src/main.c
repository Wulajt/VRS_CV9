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
#include "button.h"
#include "string.h"

void SystemClock_Config(void);

uint8_t check_button_state(GPIO_TypeDef* PORT, uint8_t PIN);

uint8_t switch_state = 0;

extern uint64_t disp_time;
uint64_t saved_time;
uint8_t txtIndex = 0;

float get_altitude(float temp, float press){
    double tmp1 = 1013/(double)press;
    double tmp2 = pow(tmp1, (1/5.257));
    float alti = ((tmp2 - 1)*(temp + 273))/0.0065; // Vypocet nadmorskej vysky
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

  uint8_t text[12];
  uint8_t cycleRight = 1;

  uint8_t temp_text[12] = "temp_%.1f";
  uint8_t hum_text[12] = "hum_%2d";
  uint8_t bar_text[12] = "bar_%.2f";
  uint8_t alt_text[12] = "alt_%.1f";

  float lps_temp = lps25hb_get_temp();
  int8_t hts_humi = hts221_get_humi();
  float lps_press = lps25hb_get_press();
  float alt = get_altitude(lps_temp, lps_press);

  while (1)
    {
        if(disp_time > (saved_time + 500))
        {
            memset(text, 0, 12);
            switch (switch_state){
            case 0:
                lps_temp = lps25hb_get_temp();
                sprintf(text, temp_text, lps_temp);
                break;
            case 1:
                hts_humi = hts221_get_humi();
                sprintf(text, hum_text, hts_humi);
                break;
            case 2:
                lps_press = lps25hb_get_press();
                sprintf(text, bar_text, lps_press);
                break;
            case 3:
                lps_temp = lps25hb_get_temp();
                lps_press = lps25hb_get_press();
                alt = get_altitude(lps_temp, lps_press);
                sprintf(text, alt_text, alt);
                break;
            default:
                break;
            }

            auto txt_length = strlen(text);

            uint8_t toDisplay[4] = "";

            if (txtIndex >= (txt_length - 4)){
                cycleRight = 0;
            }
            else if (txtIndex <= 0){
                cycleRight = 1;
            }

            uint8_t resolution = 0;
            uint8_t skipped = 0;
            for (int i = 0; i<4; i++){
                if (text[i + txtIndex + resolution] == '.' || resolution > 0){
                    resolution++;
                    skipped = 1;
                }
                toDisplay[i] = text[i + txtIndex + skipped];
            }

            if (cycleRight){
                txtIndex++;
            }
            else {
                txtIndex--;
            }

            displayString(toDisplay, resolution);

            saved_time = disp_time;
        }
    }
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

uint8_t checkButtonState(GPIO_TypeDef* PORT, uint8_t PIN, uint8_t edge, uint8_t samples_window, uint8_t samples_required)
{
		uint8_t button_state = 0, timeout = 0;

		while(button_state < samples_required && timeout < samples_window)
		{
			if((LL_GPIO_IsInputPinSet(PORT, PIN)))
			{
				button_state += 1;
			}
			else
			{
				button_state = 0;
			}

			timeout += 1;
			LL_mDelay(1);
		}

		if((button_state >= samples_required) && (timeout <= samples_window))
		{
			return 1;
		}
		else
		{
			return 0;
		}
}


void EXTI3_IRQHandler(void)
{
	if(checkButtonState(GPIO_PORT_BUTTON,
						GPIO_PIN_BUTTON,
						BUTTON_EXTI_TRIGGER,
						BUTTON_EXTI_SAMPLES_WINDOW,
						BUTTON_EXTI_SAMPLES_REQUIRED))
	{
		switch_state += 1;
		txtIndex = 0;

		if(switch_state >=4){
			switch_state = 0;
		}
	}

	/* Clear EXTI4 pending register flag */
	//EXTI->PR |= (EXTI_PR_PIF4);
	//LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);

	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3) != RESET)
	{
	    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
