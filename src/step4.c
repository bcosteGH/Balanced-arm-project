#include <chip.h>
#include <i2c.h>
#include <imu.h>
#include <pinint.h>
#include <uart.h>
#include <fonction.h>
#include <motor.h>
#include <calibrate.h>
#include <timer.h>

#define REPLAY
volatile uint8_t function = 1;							//Useful for communication between IMU handler and main loop
volatile int16_t values[7] = {0,0,0,0,0,0,0};			//Values of the IMU
const byte MOTHER_KEY[32] = "12345678912345678912345678912345";

volatile float kp = 17;
volatile float ki = 0;
volatile float kd = 0;
volatile float alpha = 0.9;
volatile uint8_t first = 1;

uint16_t nounce = 0;

int main()
{
	int ret;
	byte MESSAGE[64];
	uint8_t init_system = 1;
	uint8_t UART_connected = 0;
	byte DERIVATE_KEY[32];
	while (init_system) {
		Chip_GPIO_Init(LPC_GPIO);			//Init chip
		Init_UART();						//Init UART
		Init_Motor();						//Init both motors
		init_system = Init_IMU();			//Init IMU (and I2C in a row)
		//Init_TIMER();
		//init_system = 0;
		
	}
	calculate_offset(ACC_X_OFF, ACC_Z_OFF, GYRO_Y_OFF);
	Motor_Start();
	Init_PININT();
	//TIMER_Start();
	while(1) {	//main loop -> reading UART
		
		#ifdef CRYPT
		if (!UART_connected) {
			if (!UART_Handshake(MOTHER_KEY, DERIVATE_KEY)) {
				UART_connected = 1;	
			}
		} else {
			ret = 0;
			ret = UART_ReceiveSTR_Signed(MESSAGE, DERIVATE_KEY);
			if (ret != 0){
				UART_SendSTR_Signed(MESSAGE, ret, DERIVATE_KEY, 0);
			}
		}
		#endif
		#ifdef REPLAY
		UART_Read_PID();
		#endif
	}
	return(0);
}
