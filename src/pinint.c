#include <stdint.h>
#include <chip.h>
#include <pinint.h>
#include <uart.h>
#include <i2c.h>
#include <imu.h>
#include <fonction.h>
#include <led.h>
#include <math.h>

extern const uint8_t imu_address;
extern volatile uint16_t values[7];
extern volatile uint8_t function;
extern volatile uint8_t ask_for_new_value;
extern volatile uint8_t read_available;
/*volatile uint32_t countGraph = 0;*/

void Init_PININT()
{
	//Ser pin dir as input
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, GPIO_PININT_PORT, GPIO_PININT);
	//Configure pin as GPIO with pullup
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIN_ID, (IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_DIGMODE_EN));
	// Configure channel interrupt as edge sensitive and rising edge interrupt
	Chip_GPIO_SetupPinInt(LPC_GPIO, GPIO_PININT_PORT, GPIO_PININT, GPIO_INT_RISING_EDGE);

	// Enable GPIO pin intrerrupt
	Chip_GPIO_EnableInt(LPC_GPIO, GPIO_PININT_PORT, (1 << GPIO_PININT));

	//Set register as 0
	IMU_Read_Values();

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ(PININT_NVIC_NAME);
	NVIC_SetPriority(PININT_NVIC_NAME,1);
	NVIC_EnableIRQ(PININT_NVIC_NAME);
	/*countGraph = 0;*/
}

void PININT_IRQ_HANDLER(void)
{
	uint8_t per;
	Chip_GPIO_ClearInts(LPC_GPIO, GPIO_PININT_PORT, (1 << GPIO_PININT));
	if (read_available == 1) {
		IMU_Read_Values();
	} else { // read_available == 0
		ask_for_new_value = 1;
	}
	per = mult_per(function, values);

	/*UART_PutINT(countGraph);
	UART_PutCHAR(',');
	UART_PutINT(values[5]);
	UART_PutSTR("\r\n");
	countGraph++;*/

	Set_TIMER_Match(0,per,PERIOD_RESET_LED);
}
