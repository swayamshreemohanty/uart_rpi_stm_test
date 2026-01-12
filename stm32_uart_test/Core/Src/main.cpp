/**
 ******************************************************************************
 * @file           : stm32_uart_test.cpp
 * @brief          : STM32 Nucleo H7A3ZI-Q UART Communication Test (C++)
 *                   For bidirectional communication with Raspberry Pi 4
 ******************************************************************************
 * Hardware Connections:
 * - USART3 (default on Nucleo H7A3ZI-Q)
 * - PD8: USART3_TX -> Connect to RPi RX (GPIO 15)
 * - PD9: USART3_RX -> Connect to RPi TX (GPIO 14)
 * - GND: Connect to RPi GND
 * 
 * Configuration in STM32CubeIDE:
 * 1. Enable USART3 in .ioc file
 * 2. Set baud rate to 115200
 * 3. Mode: Asynchronous
 * 4. Word Length: 8 Bits
 * 5. Parity: None
 * 6. Stop Bits: 1
 * 7. In project settings, change main.c to main.cpp
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <cstring>
#include <cstdio>
#include <cstdarg>

/* Private classes -----------------------------------------------------------*/
class UARTComm {
private:
    static constexpr uint16_t RX_BUFFER_SIZE = 128;
    static constexpr uint16_t TX_BUFFER_SIZE = 128;
    
    UART_HandleTypeDef* huart;
    uint8_t rxBuffer[RX_BUFFER_SIZE];
    uint8_t txBuffer[TX_BUFFER_SIZE];
    uint8_t rxData;
    volatile uint16_t rxIndex;
    volatile bool dataReceived;
    uint32_t heartbeatCounter;
    
public:
    UARTComm(UART_HandleTypeDef* uart) 
        : huart(uart), rxData(0), rxIndex(0), 
          dataReceived(false), heartbeatCounter(0) {
        memset(rxBuffer, 0, RX_BUFFER_SIZE);
        memset(txBuffer, 0, TX_BUFFER_SIZE);
    }
    
    void init() {
        huart->Instance = USART3;
        huart->Init.BaudRate = 115200;
        huart->Init.WordLength = UART_WORDLENGTH_8B;
        huart->Init.StopBits = UART_STOPBITS_1;
        huart->Init.Parity = UART_PARITY_NONE;
        huart->Init.Mode = UART_MODE_TX_RX;
        huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart->Init.OverSampling = UART_OVERSAMPLING_16;
        
        if (HAL_UART_Init(huart) != HAL_OK) {
            Error_Handler();
        }
    }
    
    void startReceive() {
        HAL_UART_Receive_IT(huart, &rxData, 1);
    }
    
    void sendMessage(const char* message) {
        HAL_UART_Transmit(huart, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
    }
    
    void sendFormatted(const char* format, ...) {
        va_list args;
        va_start(args, format);
        int len = vsnprintf((char*)txBuffer, TX_BUFFER_SIZE, format, args);
        va_end(args);
        
        if (len > 0 && len < TX_BUFFER_SIZE) {
            HAL_UART_Transmit(huart, txBuffer, len, HAL_MAX_DELAY);
        }
    }
    
    void processReceivedData() {
        if (dataReceived) {
            dataReceived = false;
            
            // Echo back received data with acknowledgment
            sendFormatted("STM32->RPi: ACK [%s]\r\n", rxBuffer);
            
            // Clear receive buffer
            memset(rxBuffer, 0, RX_BUFFER_SIZE);
            rxIndex = 0;
        }
    }
    
    void sendHeartbeat() {
        static uint32_t lastHeartbeat = 0;
        uint32_t currentTick = HAL_GetTick();
        
        if (currentTick - lastHeartbeat >= 2000) {
            sendFormatted("STM32 Heartbeat: %lu\r\n", heartbeatCounter++);
            lastHeartbeat = currentTick;
        }
    }
    
    void handleRxCallback() {
        if (rxData == '\n' || rxData == '\r') {
            // End of line received
            rxBuffer[rxIndex] = '\0';
            dataReceived = true;
        }
        else if (rxIndex < (RX_BUFFER_SIZE - 1)) {
            // Store received character
            rxBuffer[rxIndex++] = rxData;
        }
        
        // Re-enable reception for next byte
        HAL_UART_Receive_IT(huart, &rxData, 1);
    }
    
    UART_HandleTypeDef* getHandle() const { return huart; }
};

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;
UARTComm* uartComm = nullptr;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    
    /* Configure the system clock */
    SystemClock_Config();
    
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    
    /* Create and initialize UART communication object */
    uartComm = new UARTComm(&huart3);
    uartComm->init();
    uartComm->startReceive();
    
    /* Send startup message */
    uartComm->sendMessage("STM32 Nucleo H7A3ZI-Q UART Ready (C++)\r\n");
    
    /* Infinite loop */
    while (1)
    {
        // Process incoming data
        uartComm->processReceivedData();
        
        // Send periodic heartbeat
        uartComm->sendHeartbeat();
        
        HAL_Delay(10);
    }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();  // For LED if needed
}

/**
 * @brief  UART Receive Complete Callback
 * @param  huart: UART handle
 * @retval None
 */
extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3 && uartComm != nullptr) {
        uartComm->handleRxCallback();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* Blink LED or add error handling */
    }
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
extern "C" void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add implementation to report error */
}
#endif /* USE_FULL_ASSERT */
