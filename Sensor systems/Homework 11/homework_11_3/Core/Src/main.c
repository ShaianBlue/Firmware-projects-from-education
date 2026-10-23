/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {true, false} bool;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

//----------------------BUTTON-MATRIX-RELATED VARIABLES-----------------------//
bool STARTUP_OVERFLOW_TIM11 = true;
uint16_t KEYB_R[4] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_2, GPIO_PIN_3};
uint16_t KEYB_C[4] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11};

int j = 0;                            //moving column index for buttons scan

int START_COUNT[4][4]={
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

int ALREADY_PRINTED[4][4]={
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

int MILLIS_OF_PRESS[4][4]={
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

char CHAR_MATRIX[4][4]={
  {'0', '1', '2', '3'},
  {'4', '5', '6', '7'},
  {'8', '9', 'A', 'B'},
  {'C', 'D', 'E', 'F'}
};
//----------------------------------------------------------------------------//

//------------------------LED-MATRIX-RELATED VARIABLES------------------------//
bool STARTUP_OVERFLOW_TIM9 = true;

// Output pin for the SS
#define RCLK_PIN GPIOB, GPIO_PIN_6
// Data, which should be received
char RX_data = 0;
// Storing the data
char command = 0;
// A flag to show if we have received data
uint8_t RX_flag = 0;
// Index, showing which coloumn in the LED to light up
uint8_t index = 0;
// Making sure that the character can be lighten up by the index
uint8_t matrix[5][2] = {{0, 16}, {0, 8}, {0, 4}, {0, 2}, {0, 1}};

// Every possible characters, we want to ligth up with the corresponding data we receive
uint8_t matrix_0[5][2] = {{62, 16}, {113, 8}, {73, 4}, {71, 2}, {62, 1}};
uint8_t matrix_1[5][2] = {{33, 16}, {65, 8}, {127, 4}, {1, 2}, {1, 1}};
uint8_t matrix_2[5][2] = {{35, 16}, {69, 8}, {73, 4}, {81, 2}, {33, 1}};
uint8_t matrix_3[5][2] = {{73, 16}, {73, 8}, {73, 4}, {73, 2}, {54, 1}};
uint8_t matrix_4[5][2] = {{120, 16}, {8, 8}, {8, 4}, {127, 2}, {8, 1}};
uint8_t matrix_5[5][2] = {{121, 16}, {73, 8}, {73, 4}, {73, 2}, {70, 1}};
uint8_t matrix_6[5][2] = {{62, 16}, {73, 8}, {73, 4}, {73, 2}, {70, 1}};
uint8_t matrix_7[5][2] = {{64, 16}, {64, 8}, {67, 4}, {76, 2}, {112, 1}};
uint8_t matrix_8[5][2] = {{54, 16}, {73, 8}, {73, 4}, {73, 2}, {54, 1}};
uint8_t matrix_9[5][2] = {{48, 16}, {72, 8}, {72, 4}, {72, 2}, {127, 1}};
uint8_t matrix_A[5][2] = {{31, 16}, {36, 8}, {68, 4}, {36, 2}, {31, 1}};
uint8_t matrix_B[5][2] = {{127, 16}, {73, 8}, {73, 4}, {73, 2}, {54, 1}};
uint8_t matrix_C[5][2] = {{62, 16}, {65, 8}, {65, 4}, {65, 2}, {34, 1}};
uint8_t matrix_D[5][2] = {{127, 16}, {65, 8}, {65, 4}, {65, 2}, {62, 1}};
uint8_t matrix_E[5][2] = {{127, 16}, {73, 8}, {73, 4}, {73, 2}, {65, 1}};
uint8_t matrix_F[5][2] = {{127, 16}, {72, 8}, {72, 4}, {72, 2}, {64, 1}};

//----------------------------------------------------------------------------//

//-----------------------INFRARED-TX-RELATED VARIABLES------------------------//
bool STARTUP_OVERFLOW_TIM10 = true;
bool INFRARED_BUSY = false;

int B = 0;                            //infrared UART byte index of bit-to-transmit

int complete_byte[9];                 //containing byte to infrared-transmit + stop bit
//----------------------------------------------------------------------------//

//-----------------------INFRARED-RX-RELATED VARIABLES------------------------//
uint8_t IR_byte;                      //infrared received byte
//----------------------------------------------------------------------------//

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM11_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM9_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* calling_timer){
  if(calling_timer==&htim11){       //timer11 (for button matrix scan) set to trigger every 50ms
    timer11_custom_callback();
  }
  if(calling_timer==&htim10){       //timer10 (for TX baudrate) set to trigger every (1/2400)s
    timer10_custom_callback();
  }
  if(calling_timer==&htim9){        //timer9 (for LED matrix lightning) set to trigger every 4ms
    timer9_custom_callback();
  }
}

//-----------------------BUTTON-MATRIX-RELATED FUNCTIONS----------------------//
void timer11_custom_callback(){
  if(STARTUP_OVERFLOW_TIM11==true){
    STARTUP_OVERFLOW_TIM11=false;
  }
  else{
    activate_next_column();           //NOTE: we need to set the Systick interrupt to a higher priority level
    HAL_Delay(10);                    //in order to execute a delay inside another callback (nested interrupt,
                                      //lower preemption --> higher priority). See ioc configuration for details
    //Updating all press times
    for(int Y=0; Y<4; Y++){
      for(int K=0; K<4; K++){
        if(START_COUNT[Y][K]==1){
          MILLIS_OF_PRESS[Y][K]=MILLIS_OF_PRESS[Y][K]+50;
        }
      }
    }

    //Scanning through all rows
    for(int i=0; i<4; i++){
      if(HAL_GPIO_ReadPin(GPIOC, KEYB_R[i])==GPIO_PIN_RESET){
        if(START_COUNT[i][j]==0){
          START_COUNT[i][j]=1;
        }
      }
      else{
        START_COUNT[i][j]=0;
        ALREADY_PRINTED[i][j]=0;
        MILLIS_OF_PRESS[i][j]=0;
      }
    }

    //Moving to the next column
    j++;
    if(j==4){
      j=0;
    }
  }
}

void activate_next_column(){
  //Turn off all columns...
  for(int k=0; k<4; k++){
    HAL_GPIO_WritePin(GPIOC, KEYB_C[k], GPIO_PIN_RESET);
  }
  //...and turn on only the next column
  HAL_GPIO_WritePin(GPIOC, KEYB_C[j], GPIO_PIN_SET);      //remember that "j" is a moving index (and global variable)
}
//----------------------------------------------------------------------------//

//-----------------------INFRARED-TX-RELATED FUNCTIONS------------------------//
void timer10_custom_callback(){
  if(STARTUP_OVERFLOW_TIM10==true){
    STARTUP_OVERFLOW_TIM10=false;
  }
  else{

    if(B<=8){
      if(complete_byte[B]==0){
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
      }
      else if(complete_byte[B]==1){
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
      }
      B++;
    }

    else{
      B=0;
      HAL_TIM_Base_Stop_IT(&htim10);
      //__HAL_TIM_SET_COUNTER(&htim10, 0);    //for safety, if necessary
      INFRARED_BUSY=false;
    }

  }
}

void infrared_UART_TX(uint8_t byte_to_send){
  if(INFRARED_BUSY==false){
    INFRARED_BUSY=true;

    for(int h=0; h<8; h++){                                           //converting uint8_t into array of single bits
      complete_byte[7-h]=((byte_to_send)&(0b10000000>>h))>>(7-h);     //NOTE: we must invert the bit sequence
    }                                                                 //because it will be received inverted again

    complete_byte[8]=1;                                               //appending stop bit
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);                         //transmitting start bit
    HAL_TIM_Base_Start_IT(&htim10);
  }
  else{
    //do nothing, ignore request
  }
}
//----------------------------------------------------------------------------//

//-----------------------INFRARED-RX-RELATED FUNCTIONS------------------------//
void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart) {
	// We receive the data, that the other group have transmitted
	HAL_UART_Receive_IT(&huart1, &RX_data, sizeof(RX_data));
	command = RX_data;
	// We acknowledge that we have received a byte form UART
	RX_flag = 1;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* calling_SPI){
	if(calling_SPI == &hspi1){
		// Setting the SS pin
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		// Increment the index
		index++;
		// Resetting index for index higher than 4
		if(index > 4){
			index = 0;
		}
		// Resetting the SS pin
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	}
}
//----------------------------------------------------------------------------//

//-----------------------LED-MATRIX-RELATED FUNCTIONS-------------------------//
void timer9_custom_callback(){
  if(STARTUP_OVERFLOW_TIM9==true){
    STARTUP_OVERFLOW_TIM9=false;
  }
  else{
	  // Transmit the letter and column, which should be lighten up every 4 ms
	  HAL_SPI_Transmit_DMA(&hspi1, matrix[index], 2);
  }
}
//----------------------------------------------------------------------------//

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim11);
  HAL_TIM_Base_Start_IT(&htim9);
  HAL_UART_Receive_IT(&huart1, &RX_data, sizeof(RX_data));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    for(int I=0; I<4; I++){
      for(int J=0; J<4; J++){
        if(MILLIS_OF_PRESS[I][J]>=200){                       //pressings shorter than 200ms will be ignored
          if(ALREADY_PRINTED[I][J]==0){
            infrared_UART_TX((uint8_t)(CHAR_MATRIX[I][J]));   //perform transmission
            ALREADY_PRINTED[I][J]=1;
          }
          else{
            //do nothing
          }
        }
      }
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(RX_flag == 1) {
    		  // The matrix, which should be send to the LED matrix is chosen with the corresponding character
    		  switch(command) {
    		  	  case '0':
    		  		  memcpy(matrix, matrix_0, sizeof(matrix));
    		  		  break;
    		  	  case '1':
    		  		  memcpy(matrix, matrix_1, sizeof(matrix));
    		  		  break;
    		  	  case '2':
    		  		  memcpy(matrix, matrix_2, sizeof(matrix));
    		  		  break;
    		  	  case '3':
    		  		  memcpy(matrix, matrix_3, sizeof(matrix));
    		  		  break;
    		  	  case '4':
    		  		  memcpy(matrix, matrix_4, sizeof(matrix));
    		  		  break;
    		  	  case '5':
    		  		  memcpy(matrix, matrix_5, sizeof(matrix));
    		  		  break;
    		  	  case '6':
    		  		  memcpy(matrix, matrix_6, sizeof(matrix));
    		  		  break;
    		  	  case '7':
    		  		  memcpy(matrix, matrix_7, sizeof(matrix));
    		  		  break;
    		  	  case '8':
    		  		  memcpy(matrix, matrix_8, sizeof(matrix));
    		  		  break;
    		  	  case '9':
    		  		  memcpy(matrix, matrix_9, sizeof(matrix));
    		  		  break;
    		  	  case 'A':
    		  		  memcpy(matrix, matrix_A, sizeof(matrix));
    		  		  break;
    		  	  case 'B':
    		  		  memcpy(matrix, matrix_B, sizeof(matrix));
    		  		  break;
    		  	  case 'C':
    		  		  memcpy(matrix, matrix_C, sizeof(matrix));
    		  		  break;
    		  	  case 'D':
    		  		  memcpy(matrix, matrix_D, sizeof(matrix));
    		  		  break;
    		  	  case 'E':
    		  		  memcpy(matrix, matrix_E, sizeof(matrix));
    		  		  break;
    		  	  case 'F':
    		  		  memcpy(matrix, matrix_F, sizeof(matrix));
    		  		  break;
    		  }
    		  // We reset the ackowledgement of the data
    		  RX_flag = 0;
    	  }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2210-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1105-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 4000-1;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 84-1;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 0;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 35000-1;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 100-1;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 42000-1;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 2400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 PC10 PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
