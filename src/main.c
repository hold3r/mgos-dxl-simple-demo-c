#include "mgos.h"
#include "mgos_jsar.h"

#define DYN_STATUS_OK (0)
#define UARTno    (0)
#define LED_PIN   (2)
#define RGB_RED   (26)
#define RGB_GREEN (27)
#define RGB_BLUE  (28)
#define RGB_ID (0x15)

// communication baudrate
const long unsigned int baudrate = 57600;
 
/* Create Dynamixel device */ 
DynamixelDevice * rgb = NULL;

/* User UART callback */
void user_cb(uint16_t len, uint8_t *data, void *userdata)
{

	mgos_uart_write(UARTno, data, len);
	(void) userdata;
}

/* Led task - green led on board */
static void led_task(void *arg)
{
  mgos_gpio_toggle(LED_PIN);
  (void) arg;
}

/* RGB task - read, modify and write from/to sensor*/
static void rgb_task(void *arg) 
{
	static uint8_t temp = 0, r = 0, g = 0, b = 0;
	temp = mgos_dxl_read(rgb, RGB_RED);
	if (mgos_dxl_status(rgb) == DYN_STATUS_OK) r = temp;

	temp = mgos_dxl_read(rgb, RGB_GREEN);
	if (mgos_dxl_status(rgb) == DYN_STATUS_OK) g = temp;

	temp = mgos_dxl_read(rgb, RGB_BLUE);
	if (mgos_dxl_status(rgb) == DYN_STATUS_OK) b = temp;

	LOG(LL_INFO, ("LED: %d:%d:%d", r, g, b));

	if (r) {  
		r = 0; 
		b = 2;
	} else if (b) { 
		b = 0; 
		g = 2;
	} else { 
		b = 0; 
		r = 2; 
		g = 0; 
	}

	mgos_dxl_write(rgb, RGB_RED, r);
	mgos_dxl_write(rgb, RGB_GREEN, g);
	mgos_dxl_write(rgb, RGB_BLUE, b);

	(void) arg;
}


enum mgos_app_init_result mgos_app_init(void) 
{
	/* Start dxl interface */
	mgos_dxl_master_begin(baudrate);

	/* Creat sensor */
	rgb = mgos_dxl_module_create(RGB_ID);

	/* Init sensor */
	mgos_dxl_init(rgb);

	/* Set callback for UART */
	mgos_dxl_setUserUartCb(user_cb, NULL);

	/* Set tasks */
  	mgos_set_timer(500 , MGOS_TIMER_REPEAT, rgb_task, NULL);
	mgos_set_timer(333 , MGOS_TIMER_REPEAT, led_task, NULL);

	return MGOS_APP_INIT_SUCCESS;
}












