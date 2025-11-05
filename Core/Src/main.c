/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Button.h"
#include "SEGGER_RTT.h"
#include "Angle.h"
#include "fashion_star_uart_servo.h"
#include "oled.h"
#include "u8g2.h"
#include "user_uart.h"
#include "dispDriver.h"
#include "ui.h"
#include "question.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern u8g2_t u8g2;
Angle_HandleTypeDef hANGLE;
Button_HandleTypeDef hKEY;
ui_t MainUI;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern float Angle;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float Angle = 0.0f;  // 在这里定义全局变量
float Angle_Offset = 122.7f;

float bott = 2.0f;
uint8_t questionTotalFlag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void FSUSExample_PingServo(void);
void FSUSExample_ReadData(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_I2C3_Init();
  MX_SPI4_Init();
  MX_UART4_Init();
  MX_UART8_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  SEGGER_RTT_Init();//RTT日志

  Angle_Init(&hANGLE, &hadc1); //角位移传感器

  u8g2Init(&u8g2);
  MiaoUi_Setup(&MainUI);
  User_Uart_Init(&huart8);

  HAL_TIM_Base_Start_IT(&htim7);
	// int16_t mid_offset_value = (int16_t)(bott * 10);  // 转换为0.1度单位
 //  FSUS_WriteData(&FSUS_Usart,0,FSUS_PARAM_ANGLE_MID_OFFSET,(uint8_t *)&mid_offset_value,2);
  //FSUS_WriteData(&FSUS_Usart,0,FSUS_PARAM_OVER_VOLT_HIGH,(uint8_t *)&bott,2);
  //FSUS_ResetUserData(&FSUS_Usart,0);
  //FSUS_SetOriginPoint(&FSUS_Usart,0);
  //FSUS_StopOnControlMode(&FSUS_Usart,0,0,500);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 处理角度传感器逻辑
    Angle_Process(&hANGLE);
  	//FSUS_SetOriginPoint(&FSUS_Usart,0);
    // 获取当前角度并进行零点校准
    float raw_angle = Angle_GetFilteredAngle(&hANGLE);
    Angle = raw_angle;


    ui_loop(&MainUI);
    // u8g2_DrawStr(&u8g2 ,0,10,"Hello World");
    // u8g2_SendBuffer(&u8g2);

    //HAL_Delay(100);
    //FSUSExample_PingServo();
  	//FSUSExample_ReadData();
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief TIM7 中断回调函数
  * @param htim: TIM句柄指针
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 判断是否为TIM7中断
    if (htim->Instance == TIM7)
    {
    	questionTotalFlag = getQuestionFlag();

    	switch (questionTotalFlag) {
    		case 1:
    			FSUS_SetServoAngleMTurn(&FSUS_Usart,0,360,120,0);

    			//FSUS_SetOriginPoint(&FSUS_Usart,0);
    			break;

    		case 2:
    			//FSUS_SetServoAngleMTurn(&FSUS_Usart,0,,500,0);
    			FSUS_SetServoAngleByInterval(&FSUS_Usart,0,-20,100,20,20,0);
    			float servoAngle=0;
				//FSUS_QueryServoAngle(&FSUS_Usart,0,&servoAngle);
    			//SEGGER_RTT_printf(0,"Servo Angle: %f\n",servoAngle);
    			break;

    		case 3:
    			// 获取当前摆杆角度 (-180~180度)
    			float pendulum_angle = Angle;
    			// 计算舵机补偿角度 (取反以保持水平)A
    			float servo_angle = pendulum_angle;

    			// 限制舵机角度范围 (根据舵机规格调整)
    			// if (servo_angle > 180.0f) servo_angle = 180.0f;
    			// if (servo_angle < -180.0f) servo_angle = -180.0f;

    			servo_angle = servo_angle * 0.6f;

    			servo_angle = servo_angle-73.5f;
    			SEGGER_RTT_printf(0,"%f\n",servo_angle);
    			// 控制舵机转动到指定角度
    			FSUS_SetServoAngleByInterval(&FSUS_Usart,0,servo_angle,100,20,20,0);
    			break;

    		default:
    			//SEGGER_RTT_printf(0,"Question Total Flag Error\n");
    			break;
    	}

      HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
    }
}


/* 舵机通讯检测 */
void FSUSExample_PingServo(void)
{
  FSUS_STATUS status_code; // 状态码
  uint8_t servo_id = 0;	 // 舵机ID = 0

  Usart_DataTypeDef *servo_usart = &FSUS_Usart; // 串口总线舵机对应的USART

  SEGGER_RTT_printf(0,"===Test Uart Servo Ping===r\n");
  while (1)
  {
    // 舵机通信检测
    status_code = FSUS_Ping(servo_usart, servo_id);
    if (status_code == FSUS_STATUS_SUCCESS)
    {
      // 舵机在线， LED1闪烁(绿灯)
      SEGGER_RTT_printf(0,"Servo Online \r\n");
      HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
    }
    else
    {
      // 舵机离线， LED0闪烁(红灯)
      SEGGER_RTT_printf(0,"Servo Offline,Error Code=%d \r\n", status_code);
      HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
    }
    // 延时等待1s
    HAL_Delay(1000);
  }
}

/*读取舵机状态*/
void FSUSExample_ReadData(void)
{
	uint8_t servo_id = 0;	// 连接在转接板上的总线伺服舵机ID号
	FSUS_STATUS statusCode; // 状态码

	// 数据表里面的数据字节长度一般为1个字节/2个字节/4个字节
	// 查阅通信协议可知,舵机角度上限的数据类型是有符号短整型(UShort, 对应STM32里面的int16_t),长度为2个字节
	// 所以这里设置value的数据类型为int16_t
	int16_t value;
	uint8_t dataSize;
	// 传参数的时候, 要将value的指针强行转换为uint8_t
	Usart_DataTypeDef *servo_usart = &FSUS_Usart; // 串口总线舵机对应的USART

	// 读取电压
	statusCode = FSUS_ReadData(servo_usart, servo_id, FSUS_PARAM_VOLTAGE, (uint8_t *)&value, &dataSize);

	SEGGER_RTT_printf(0,"read ID: %d\r\n", servo_id);

	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		SEGGER_RTT_printf(0,"read sucess, voltage: %d mV\r\n", value);
	}
	else
	{
		SEGGER_RTT_printf(0,"fail\r\n");
	}

	// 读取电流
	statusCode = FSUS_ReadData(servo_usart, servo_id, FSUS_PARAM_CURRENT, (uint8_t *)&value, &dataSize);
	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		SEGGER_RTT_printf(0,"read sucess, current: %d mA\r\n", value);
	}
	else
	{
		SEGGER_RTT_printf(0,"fail\r\n");
	}

	// 读取功率
	statusCode = FSUS_ReadData(servo_usart, servo_id, FSUS_PARAM_POWER, (uint8_t *)&value, &dataSize);
	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		SEGGER_RTT_printf(0,"read sucess, power: %d mW\r\n", value);
	}
	else
	{
		SEGGER_RTT_printf(0,"fail\r\n");
	}
	// 读取温度
	statusCode = FSUS_ReadData(servo_usart, servo_id, FSUS_PARAM_TEMPRATURE, (uint8_t *)&value, &dataSize);
	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		double temperature, temp;
		temp = (double)value;
		temperature = 1 / (log(temp / (4096.0f - temp)) / 3435.0f + 1 / (273.15 + 25)) - 273.15;
		SEGGER_RTT_printf(0,"read sucess, temperature: %f\r\n", temperature);
	}
	else
	{
		SEGGER_RTT_printf(0,"fail\r\n");
	}
	// 读取工作状态
	statusCode = FSUS_ReadData(servo_usart, servo_id, FSUS_PARAM_SERVO_STATUS, (uint8_t *)&value, &dataSize);
	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		// 舵机工作状态标志位
		// BIT[0] - 执行指令置1，执行完成后清零。
		// BIT[1] - 执行指令错误置1，在下次正确执行后清零。
		// BIT[2] - 堵转错误置1，解除堵转后清零。
		// BIT[3] - 电压过高置1，电压恢复正常后清零。
		// BIT[4] - 电压过低置1，电压恢复正常后清零。
		// BIT[5] - 电流错误置1，电流恢复正常后清零。
		// BIT[6] - 功率错误置1，功率恢复正常后清零。
		// BIT[7] - 温度错误置1，温度恢复正常后清零。

		if ((value >> 3) & 0x01)
			SEGGER_RTT_printf(0,"read sucess, voltage too high\r\n");
		if ((value >> 4) & 0x01)
			SEGGER_RTT_printf(0,"read sucess, voltage too low\r\n");
	}
	else
	{
		SEGGER_RTT_printf(0,"fail\r\n");
	}
	SEGGER_RTT_printf(0,"================================= \r\n");

	// 死循环
	while (1)
	{
	}
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

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
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
