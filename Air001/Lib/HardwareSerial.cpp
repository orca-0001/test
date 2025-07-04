#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "TW32f0xx.h"
#include "HardwareSerial.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

#define RX_BUFFER_SIZE 64

struct ring_buffer
{
	unsigned char buffer[RX_BUFFER_SIZE];
	int head;
	int tail;
};

ring_buffer rx_buffer1 = {{0}, 0, 0};
ring_buffer rx_buffer2 = {{0}, 0, 0};

inline void store_char(unsigned char c, ring_buffer *rx_buffer)
{
	int i = (rx_buffer->head + 1) % RX_BUFFER_SIZE;

	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	if (i != rx_buffer->tail)
	{
		rx_buffer->buffer[rx_buffer->head] = c;
		rx_buffer->head = i;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt Handler                                                                                 	   */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void UART1_IRQHandler(void)
{
    uint8_t rec;
    if( ((USART1->SR) & USART_SR_RXNE) != RESET){
        rec = (uint8_t)(USART1->DR & (uint8_t)0x00FF);
        store_char(rec, &rx_buffer1);
    }
}

void UART2_IRQHandler(void)
{
    uint8_t rec;
    if( ((USART2->SR) & USART_SR_RXNE) != RESET){
        rec = (uint8_t)(USART2->DR & (uint8_t)0x00FF);
        store_char(rec, &rx_buffer2);
    }
}

#ifdef __cplusplus
}
#endif

HardwareSerial::HardwareSerial(ring_buffer *rx_buffer, USART_TypeDef *UART)
{
	_port = UART;
	_rx_buffer = rx_buffer;
}

void HardwareSerial::begin(uint32_t speed)
{
	if (_port == USART1)
	{
		UART1_Init(speed);
	}else if(_port == USART2){
        UART2_Init(speed);
    }
}

int HardwareSerial::available(void)
{
	return (RX_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % RX_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
	if (_rx_buffer->head == _rx_buffer->tail)
	{
		return -1;
	}
	else
	{
		return _rx_buffer->buffer[_rx_buffer->tail];
	}
}

int HardwareSerial::read(void)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (_rx_buffer->head == _rx_buffer->tail)
	{
		return -1;
	}
	else
	{
		unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
		_rx_buffer->tail = (_rx_buffer->tail + 1) % RX_BUFFER_SIZE;
		return c;
	}
}

void HardwareSerial::flush()
{
	// don't reverse this or there may be problems if the RX interrupt
	// occurs after reading the value of rx_buffer_head but before writing
	// the value to rx_buffer_tail; the previous value of rx_buffer_head
	// may be written to rx_buffer_tail, making it appear as if the buffer
	// don't reverse this or there may be problems if the RX interrupt
	// occurs after reading the value of rx_buffer_head but before writing
	// the value to rx_buffer_tail; the previous value of rx_buffer_head
	// may be written to rx_buffer_tail, making it appear as if the buffer
	// were full, not empty.
	_rx_buffer->head = _rx_buffer->tail;
}

/*
void HardwareSerial::end()
{
  // wait for transmission of outgoing data
  while (_tx_buffer_head != _tx_buffer_tail)
    ;
  
  // clear any received data
  _rx_buffer_head = _rx_buffer_tail;
}*/

size_t HardwareSerial::write(uint8_t ch)
{
	if (_port == USART1)
	{
		UART1_SendCh(ch);
	}else if (_port == USART2){
        UART2_SendCh(ch);
    }
	return 1;
}

// Preinstantiate Objects //////////////////////////////////////////////////////
HardwareSerial Serial1(&rx_buffer1, USART1);
HardwareSerial Serial2(&rx_buffer2, USART2);
