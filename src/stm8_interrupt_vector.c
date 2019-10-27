/*	BASIC INTERRUPT VECTOR TABLE FOR STM8 devices
 *	Copyright (c) 2007 STMicroelectronics
 */

typedef void @far (*interrupt_handler_t)(void);

struct interrupt_vector {
	unsigned char interrupt_instruction;
	interrupt_handler_t interrupt_handler;
};

@far @interrupt void NonHandledInterrupt (void)
{
	/* in order to detect unexpected events during development, 
	   it is recommended to set a breakpoint on the following instruction
	*/
	return;
}

extern void _stext();     /* startup routine */
extern @far @interrupt void Timer2Interrupt(void);
extern @far @interrupt void I2CMaster_Int_I2CInterruptHandler(void);
extern @far @interrupt void UART1_RxInterruptHandler(void);
extern @far @interrupt void UART1_TxInterruptHandler(void);

struct interrupt_vector const _vectab[] = {
	{0x82, (interrupt_handler_t)_stext}, /* reset */
	{0x82, NonHandledInterrupt}, /* trap  */
	{0x82, NonHandledInterrupt}, /* tli  */
	{0x82, NonHandledInterrupt}, /* flash  */
	{0x82, NonHandledInterrupt}, /* dma01  */
	{0x82, NonHandledInterrupt}, /* dma23  */
	{0x82, NonHandledInterrupt}, /* rtc  */
	{0x82, NonHandledInterrupt}, /* extief/pvd  */
	{0x82, NonHandledInterrupt}, /* extibg  */
	{0x82, NonHandledInterrupt}, /* extidh  */
	{0x82, NonHandledInterrupt}, /* exti0  */
	{0x82, NonHandledInterrupt}, /* exti1  */
	{0x82, NonHandledInterrupt}, /* exti2 */
	{0x82, NonHandledInterrupt}, /* exti3 */
	{0x82, NonHandledInterrupt}, /* exti4 */
	{0x82, NonHandledInterrupt}, /* exti5 */
	{0x82, NonHandledInterrupt}, /* exti6 */
	{0x82, NonHandledInterrupt}, /* exti7 */
	{0x82, NonHandledInterrupt}, /* lcd */
	{0x82, NonHandledInterrupt}, /* clk tim1 dac */
	{0x82, NonHandledInterrupt}, /* adc */
	{0x82, Timer2Interrupt}, /* tim2 usart2tx */
	{0x82, NonHandledInterrupt}, /* tim2cc usart2rx  */
	{0x82, NonHandledInterrupt}, /* tim3 usart3tx */
	{0x82, NonHandledInterrupt}, /* tim3cc usart3rx */
	{0x82, NonHandledInterrupt}, /* tim1upd */
	{0x82, NonHandledInterrupt}, /* tim1cc */
	{0x82, NonHandledInterrupt}, /* tim4upd */
	{0x82, NonHandledInterrupt}, /* spi1eot */
	{0x82, UART1_TxInterruptHandler}, /* usart1tx tim5 */
	{0x82, UART1_RxInterruptHandler}, /* usart1rx tim5cc */
	{0x82, I2CMaster_Int_I2CInterruptHandler}, /* i2c spi2 */
};