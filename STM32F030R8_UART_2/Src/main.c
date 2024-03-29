/**
  ******************************************************************************
  * @file    main.c
  * @author  Auto-generated by STM32CubeIDE
  * @version V1.0
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f030x6.h"

#ifndef USART2_BASE
 #define USART2_BASE           (APBPERIPH_BASE + 0x00004400)
#endif

#ifndef USART2
  #define USART2              ((USART_TypeDef *) USART2_BASE)
#endif

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void ClockConfig(void);
void UARTConfig(void);
void UARTSend(unsigned int x);

int main(void)
{
	unsigned int i;

	ClockConfig(); // set 48MHz clock
	UARTConfig();

	for(i = 0; ; i++)
	    {
	        UARTSend(0x30 + (i & 7)); // make counter value to ascii-number (add number start and take 3 bits of counter) and send via UART
	    }
}

/**
  * @brief  Configures clock.
  *         Source is the internal oscillator with 8MHz.
  *         That value is halfed to 4MHz.
  *         PLL in enabled and multiplies clock by 12, so 48MHz is output.
  *         PLL output is set as system clock.
  * @param  None
  * @retval None
  */
void ClockConfig(void) {
	RCC->CFGR |= RCC_CFGR_PLLMUL_3 | RCC_CFGR_PLLMUL_1;  // set pll multiplier to x(10 + 2) (has to be set before pll enable)
	RCC->CR |= RCC_CR_PLLON;         // enable pll (to increase clock from 8MHz to 48MHz)
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC); // set internal oscillator (8MHz/2) as source for pll
	RCC->CFGR &= ~(RCC_CFGR_PPRE_2); // prescaler for APB clock is x1
	RCC->CFGR &= ~(RCC_CFGR_HPRE_3); // prescaler for HCLK clock is x1
	RCC->CFGR |= RCC_CFGR_SW_1;      // pll as source for SYSCLK
}

/**
  * @brief  Configures UART.
  *         First peripheral clocks are enabled.
  *         Alternate functions are set for pins.
  *         With this only transmitting is enabled.
  *         Baudrate is 19200.
  * @param  None
  * @retval None
  */
void UARTConfig() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // enable GPIOA

	RCC->APB1ENR |= 1<<17; // enable UART2 peripheral

	GPIOA->MODER &= ~(GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0); // with next line, set PA2 and PA3 to 10 = Alternate function mode
	GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1;

	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_2 | GPIO_OTYPER_OT_3); // output push-pull

	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR2_0 | GPIO_OSPEEDR_OSPEEDR2_1 | GPIO_OSPEEDR_OSPEEDR3_0 | GPIO_OSPEEDR_OSPEEDR3_1; // high speed outputs

	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_0 | GPIO_PUPDR_PUPDR3_1); // no push, no pull

	GPIOA->AFR[0] &= ~(0xF<<GPIO_AFRL_AFSEL2_Pos | 0xF<<GPIO_AFRL_AFSEL3_Pos); // with next line, set alternate function AF1
	GPIOA->AFR[0] |= 1<<GPIO_AFRL_AFSEL2_Pos | 1<<GPIO_AFRL_AFSEL3_Pos;

	USART2->BRR = 2500; // 19200 Baud @ 48MHz

	USART2->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE; // enable uart and enable transmitter. To receive also enable
}

/**
  * @brief  Transmits using UART2.
  * @param  x : data to transmit
  * @retval None
  */
void UARTSend(unsigned int x)
{
    while (1) { // wait until last transmission is over
    	if (USART2->ISR & USART_ISR_TXE) { // if transmit-register is empty
    		break; // break while loop
    	}
    }

    USART2->TDR = x; // put new data in transmission register
}
