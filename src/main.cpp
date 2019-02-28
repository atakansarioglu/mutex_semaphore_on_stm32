/**
 * @file      main.cpp
 * @author    Atakan S.
 * @date      01/01/2019
 * @version   1.0
 * @brief     Demo project for mutex and semaphore on Cortex-M3.
 *
 * @copyright Copyright (c) 2018 Atakan SARIOGLU ~ www.atakansarioglu.com
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#include <stm32f10x.h>
#include <stdbool.h>
#include "memory_semaphore.h"
#include "memory_mutex.h"

// Mutex and semaphore objects.
memory_exclusive::Mutex mutex;
memory_exclusive::Semaphore semaphore;

// SysTick interrupt handler.
extern "C" void SysTick_Handler(void) {
	// Unlock mutex.
	mutex.Unlock();

	// Give binary semaphore.
	semaphore.Give(semaphore.TryOnce);
}

void LedWrite(const int ledNumber, const bool ledOn){
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable peripheral clocks.
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	// Configure PC9 output pushpull mode.
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Select LED.
	switch(ledNumber){
	case 0:
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, (BitAction)ledOn);
		break;
	case 1:
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, (BitAction)ledOn);
		break;
	case -1:
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, (BitAction)ledOn);
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, (BitAction)ledOn);
		break;
	}
}

// The main routine.
int main(void) {
	// Start SysTick at 1Hz.
	SysTick_Config(SystemCoreClock/1);

	// Enable Interrupt.
	__enable_irq();

	// Loop.
	while(true){
		// Take semaphore and indicate status on led0.
		LedWrite(0, semaphore.Take(semaphore.TryAlways));

		// Lock mutex and indicate status on led1.
		LedWrite(1, !mutex.Lock(mutex.TryAlways));

		// Take semaphore and indicate status on led0.
		LedWrite(0, !semaphore.Take(semaphore.TryAlways));

		// Lock mutex and indicate status on led1.
		LedWrite(1, mutex.Lock(mutex.TryAlways));
	}

	return 0;
}
