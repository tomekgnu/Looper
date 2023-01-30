
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "fmc.h"

/* USER CODE BEGIN Includes */
#include "tm_stm32_hd44780.h"
#include "ads1256_test.h"
#include "tm_stm32f4_keypad.h"
#include "tm_stm32f4_ili9341.h"
#include "stm32f429i_discovery_sdram.h"
#include "main.h"
#include "midi.h"
#include "menu.h"
#include "drums.h"
#include "string.h"
#include "math.h"
#include "drums.h"
#include "stdlib.h"
#include "audio.h"
#include "spi_flash.h"
#include "spiffs.h"
#include "SRAMDriver.h"
#include "tm_stm32f4_fatfs.h"
#include "memops.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_cdc_if.h"
#include "joystick.h"
#include "AT45DBXX.h"

#define pi 3.14159
#define LOG_PAGE_SIZE       256
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
LooperApplication looper;
uint8_t switches = 0;
char lcdline[30];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void my_spiffs_mount();


/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

FATFS FatFs;
spiffs fs;
static u8_t spiffs_work_buf[LOG_PAGE_SIZE*2];
static u8_t spiffs_fds[32*4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE+32)*4];

BOOL fs_mounted = FALSE;


TM_KEYPAD_Button_t Keypad_Button;
FUNCTION saveFunction = IDLE;
BOOL Skip_Read_Button = FALSE;
BOOL show_status_line = FALSE;
uint32_t joystick_data[2];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint32_t data = 0,bytesWritten = 0;
	HAL_StatusTypeDef status;
	uint8_t at45db[20];


	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  looper.Function = IDLE;
  looper.TwoChannels = FALSE;
  looper.ch1.Active = TRUE;
  looper.ch1.Monitor = FALSE;
  looper.ch2.Active = FALSE;
  looper.ch2.Monitor = FALSE;
  looper.timeIncrement = 0;
  looper.SampleBytes = 2;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_FMC_Init();
  MX_TIM4_Init();
  MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_SPI5_Init();
  MX_DAC_Init();
  MX_TIM8_Init();
  MX_USB_DEVICE_Init();
  MX_TIM9_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  sFLASH_Init();
  my_spiffs_mount();
  BSP_SDRAM_Init();
  HAL_NVIC_DisableIRQ(EXTI2_IRQn);
  //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
  TM_ILI9341_Init();
  //Rotate LCD for 90 degrees
  TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);
  //FIll lcd with color
  TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);

  status = HAL_TIM_Base_Start_IT(&htim9);

  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  status = HAL_TIM_Base_Start_IT(&htim4);
  status = HAL_ADC_Start_IT(&hadc1);

  status = HAL_ADC_Start_DMA(&hadc3,joystick_data,2);

  ADS1256_WriteCmd(CMD_RESET);
  ADS1256_WriteCmd(CMD_SDATAC);
  data = ADS1256_ReadChipID();


  ADS1256_CfgADC(ADS1256_GAIN_2, ADS1256_15000SPS);
  ADS1256_SetDiffChannel(0);
  ADS1256_WriteCmd(CMD_SELFCAL);
  ADS1256_WriteCmd(CMD_SELFOCAL);

  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  ADS1256_WriteCmd(CMD_RDATAC);
  //FATFS_UnLinkDriver(SD_Path);
  TM_KEYPAD_Init();

  setupMidi();
  talkMIDI(0xB0, 0, 0x01); //Default bank GM1

  TM_HD44780_Init(20, 4);
  TM_HD44780_Clear();
  TM_HD44780_Puts(0,0,"Looper");

  InitSRAM();
  menuInit();
  menuShow(0);

  // TODO: write initial rhythm header to SRAM
  SRAM_seekWrite(0,SRAM_SET);
  writeSRAM((unsigned char *)"\0\0\0\0\0\0\0\0\0\0\0\0",12);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (TRUE)
  {
	  checkSD();
	  if(Skip_Read_Button == FALSE)
		  Keypad_Button = TM_KEYPAD_Read();
	  Skip_Read_Button = FALSE;
	  /* Keypad was pressed */
	  if (Keypad_Button != TM_KEYPAD_Button_NOPRESSED) {/* Keypad is pressed */
		  menuShow(Keypad_Button);

	  }// end of key pressed

	  SHOW_STATUS_LINE();

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */


	}


  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
static s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst) {
    sFLASH_ReadBuffer(dst,addr,size);
    return SPIFFS_OK;
  }

  static s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src) {
	sFLASH_WriteBuffer(src,addr,size);
    return SPIFFS_OK;
  }

  static s32_t my_spiffs_erase(u32_t addr, u32_t size) {

    return sFLASH_Erase(addr, size);
  }

void my_spiffs_mount() {
    spiffs_config cfg;
    cfg.phys_size = N25Q256A_FLASH_SIZE; // use all spi flash
    cfg.phys_addr = 0; // start spiffs at start of spi flash
    cfg.phys_erase_block = N25Q256A_SECTOR_SIZE; // according to datasheet
    cfg.log_block_size = N25Q256A_SECTOR_SIZE; // let us not complicate things
    cfg.log_page_size = N25Q256A_PAGE_SIZE; // as we said

    cfg.hal_read_f = my_spiffs_read;
    cfg.hal_write_f = my_spiffs_write;
    cfg.hal_erase_f = my_spiffs_erase;

    int res = SPIFFS_mount(&fs,
      &cfg,
      spiffs_work_buf,
      spiffs_fds,
      sizeof(spiffs_fds),
      spiffs_cache_buf,
      sizeof(spiffs_cache_buf),
      0);
    printf("mount res: %i\n", res);
  }

FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}



void buttonHandler() {
	if(IS_BUT_DOWN(BUT_REC) && HAL_GPIO_ReadPin(Recording_GPIO_Port,Recording_Pin) == GPIO_PIN_SET )
		BUT_UP(BUT_REC);

	if(IS_BUT_DOWN(BUT_PLAY) && HAL_GPIO_ReadPin(Playback_GPIO_Port,Playback_Pin) == GPIO_PIN_SET )
		BUT_UP(BUT_PLAY);

	if(IS_BUT_DOWN(BUT_OVERDUB) && HAL_GPIO_ReadPin(Overdubbing_GPIO_Port,Overdubbing_Pin) == GPIO_PIN_SET )
		BUT_UP(BUT_OVERDUB);

	if(IS_BUT_DOWN(BUT_TOGFUN) && HAL_GPIO_ReadPin(ToggleFunction_GPIO_Port,ToggleFunction_Pin) == GPIO_PIN_SET )
		BUT_UP(BUT_TOGFUN);

	if(IS_BUT_DOWN(BUT_JOYSTICK) && HAL_GPIO_ReadPin(Joystick_SW_GPIO_Port,Joystick_SW_Pin) == GPIO_PIN_SET )
			BUT_UP(BUT_JOYSTICK);
}

void set_function(FUNCTION fun){
	if(looper.Function != fun){
		looper.Function = fun;
		show_status_line = TRUE;
	}
	saveFunction = fun;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
			/* User can add his own implementation to report the file name and line number,
			 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
