#include "user_uart.h"
#include "tasks.h"
#include "rpi_vision.h"

#include <string.h>

uint8_t UsartSendBuf[USART_SEND_BUF_SIZE + 1];
uint8_t UsartRecvBuf[USART_RECV_BUF_SIZE + 1];
RingBufferTypeDef UsartSendRingBuf;//发送缓冲区
RingBufferTypeDef UsartRecvRingBuf;//接收缓冲区
Usart_DataTypeDef FSUS_Usart;
uint8_t rc1;//接收中断缓存

void User_Uart_Init(UART_HandleTypeDef *huartx)
{
	// 创建缓冲组
	RingBuffer_Init(&UsartSendRingBuf, USART_SEND_BUF_SIZE, UsartSendBuf);
	RingBuffer_Init(&UsartRecvRingBuf, USART_RECV_BUF_SIZE, UsartRecvBuf);
	// 初始化自定义用户串口结构体
	FSUS_Usart.recvBuf = &UsartRecvRingBuf;
	FSUS_Usart.sendBuf = &UsartSendRingBuf;
	FSUS_Usart.huartX = huartx;
	// 开启接收中断
	HAL_UART_Receive_IT(FSUS_Usart.huartX, (uint8_t *)&rc1, 1);
}
// 发送数据
void Usart_SendAll(Usart_DataTypeDef *usart)
{
	uint8_t value;
	while (RingBuffer_GetByteUsed(usart->sendBuf))
	{
		value = RingBuffer_Pop(usart->sendBuf);
		HAL_UART_Transmit(usart->huartX, &value, 1, 1);
	}
}
extern UART_HandleTypeDef huart2;
extern RPiVision_HandleTypeDef g_rpi_vision;
uint8_t uart2_rx_byte;

#define UART2_PID_PACKET_SIZE 64U

static char uart2_rx_buf[UART2_PID_PACKET_SIZE];
static volatile uint8_t uart2_rx_len = 0U;
static volatile uint8_t uart2_packet_len = 0U;
static volatile uint8_t uart2_packet_ready = 0U;
static uint8_t uart2_rx_state = 0U;
static uint8_t uart2_drop_packet = 0U;

static void User_Uart_SkipSpaces(const char **cursor)
{
    while ((**cursor == ' ') || (**cursor == '\t')) {
        ++(*cursor);
    }
}

static uint8_t User_Uart_ParseFloat(const char **cursor, float *value)
{
    const char *text = *cursor;
    float result = 0.0f;
    float fraction_scale = 0.1f;
    float sign = 1.0f;
    uint8_t has_digit = 0U;

    User_Uart_SkipSpaces(&text);
    if ((*text == '+') || (*text == '-')) {
        if (*text == '-') sign = -1.0f;
        ++text;
    }

    while ((*text >= '0') && (*text <= '9')) {
        result = result * 10.0f + (float)(*text - '0');
        has_digit = 1U;
        ++text;
    }

    if (*text == '.') {
        ++text;
        while ((*text >= '0') && (*text <= '9')) {
            result += (float)(*text - '0') * fraction_scale;
            fraction_scale *= 0.1f;
            has_digit = 1U;
            ++text;
        }
    }

    if (!has_digit) return 0U;

    User_Uart_SkipSpaces(&text);
    *cursor = text;
    *value = sign * result;
    return 1U;
}

static int User_Uart_ParsePID(const char *packet,
                              float *kp,
                              float *ki,
                              float *kd)
{
    const char *cursor = packet;
    float second;

    if (!User_Uart_ParseFloat(&cursor, kp) || (*cursor != ',')) return 0;
    ++cursor;
    if (!User_Uart_ParseFloat(&cursor, &second)) return 0;

    if (*cursor == '\0') {
        *ki = 0.0f;
        *kd = second;
        return 2;
    }

    if (*cursor != ',') return 0;
    ++cursor;
    if (!User_Uart_ParseFloat(&cursor, kd) || (*cursor != '\0')) return 0;
    *ki = second;
    return 3;
}

void User_Uart_Process(void)
{
    char packet[UART2_PID_PACKET_SIZE];
    uint8_t packet_len;
    uint32_t primask;
    float kp = 0.0f;
    float ki = 0.0f;
    float kd = 0.0f;
    int parsed;

    if (!uart2_packet_ready) return;

    // 短暂关中断复制完整包；耗时的浮点解析在恢复中断后进行。
    primask = __get_PRIMASK();
    __disable_irq();
    packet_len = uart2_packet_len;
    if (packet_len >= UART2_PID_PACKET_SIZE) {
        packet_len = UART2_PID_PACKET_SIZE - 1U;
    }
    memcpy(packet, uart2_rx_buf, packet_len);
    packet[packet_len] = '\0';
    uart2_packet_ready = 0U;
    if (!primask) __enable_irq();

    // 兼容两种格式："Kp,Kd"（PD）或 "Kp,Ki,Kd"（PID）。
    parsed = User_Uart_ParsePID(packet, &kp, &ki, &kd);
    if ((parsed == 2) || (parsed == 3)) {
        Task3_SetBallPID(kp, ki, kd);
    }
}

//中断接收
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t ucTemp;
	if (huart->Instance == FSUS_Usart.huartX->Instance)
	{
		ucTemp = rc1;
		RingBuffer_Push(FSUS_Usart.recvBuf, ucTemp);// 接收到数据放入缓冲区，不在中断具体处理数据
		HAL_UART_Receive_IT(FSUS_Usart.huartX, (uint8_t *)&rc1, 1);
	}
    else if (huart->Instance == USART2)
    {
        // 中断只组包：AA 55 为包头，0D 0A 为包尾，中间是 ASCII 参数。
        if (uart2_rx_state == 0U) {
            if (uart2_rx_byte == 0xAAU) {
                uart2_rx_state = 1U;
            }
        } else if (uart2_rx_state == 1U) {
            if (uart2_rx_byte == 0x55U) {
                uart2_rx_state = 2U;
                uart2_rx_len = 0U;
                // 上一包尚未被主循环取走时，整包丢弃，绝不覆盖。
                uart2_drop_packet = uart2_packet_ready ? 1U : 0U;
            } else if (uart2_rx_byte != 0xAAU) {
                uart2_rx_state = 0U;
            }
        } else if (uart2_rx_state == 2U) {
            if (uart2_rx_byte == 0x0DU) {
                uart2_rx_state = 3U;
            } else if (uart2_rx_byte == 0xAAU) {
                // 新包头可立即打断残缺包并重新同步。
                uart2_rx_state = 1U;
                uart2_rx_len = 0U;
                uart2_drop_packet = uart2_packet_ready ? 1U : 0U;
            } else {
                if (!uart2_drop_packet &&
                    (uart2_rx_len < (UART2_PID_PACKET_SIZE - 1U))) {
                    uart2_rx_buf[uart2_rx_len++] = uart2_rx_byte;
                } else if (!uart2_drop_packet) {
                    // 超长包继续接收到包尾，但标记为丢弃以便可靠重同步。
                    uart2_drop_packet = 1U;
                }
            }
        } else {
            if (uart2_rx_byte == 0x0AU) {
                if (!uart2_drop_packet && (uart2_rx_len > 0U)) {
                    uart2_rx_buf[uart2_rx_len] = '\0';
                    uart2_packet_len = uart2_rx_len;
                    uart2_packet_ready = 1U;
                }
                uart2_rx_state = 0U;
            } else if (uart2_rx_byte == 0xAAU) {
                uart2_rx_state = 1U;
                uart2_rx_len = 0U;
                uart2_drop_packet = uart2_packet_ready ? 1U : 0U;
            } else {
                // 0D 后不是 0A，判定当前帧损坏并重新等待包头。
                uart2_rx_state = 0U;
                uart2_rx_len = 0U;
                uart2_drop_packet = 0U;
            }
        }
        HAL_UART_Receive_IT(&huart2, &uart2_rx_byte, 1);
    }
}

// 错误处理回调，防止因接线抖动或波特率不匹配导致的 UART 假死
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        uart2_rx_state = 0U;
        uart2_rx_len = 0U;
        uart2_drop_packet = 0U;
        // 发生错误时，清除错误标志并重新开启接收
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_PEFLAG(huart);
        HAL_UART_Receive_IT(&huart2, &uart2_rx_byte, 1);
    }
    else if (huart->Instance == UART4)
    {
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_PEFLAG(huart);
        RPiVision_ErrorCallback(&g_rpi_vision, huart);
    }
    else if (huart->Instance == FSUS_Usart.huartX->Instance)
    {
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_PEFLAG(huart);
        HAL_UART_Receive_IT(FSUS_Usart.huartX, (uint8_t *)&rc1, 1);
    }
}
