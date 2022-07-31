/**
  ******************************************************************************
  * @file            : usb_host.c
  * @brief           : This file implements the USB Host
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "gpio.h"

USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;


/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);


/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
	USBH_StatusTypeDef ret;
	/* Init host Library, add supported class and start the library. */
	ret = USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS);
	assert_param(USBH_OK == ret);

	ret = USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS);
	assert_param(USBH_OK == ret);

	ret = USBH_Start(&hUsbHostFS);
	assert_param(USBH_OK == ret);
}

/*
 * user callback definition
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
	switch(id)
	{
		case HOST_USER_SELECT_CONFIGURATION : {

		} break;

		case HOST_USER_DISCONNECTION : {
			Appli_state = APPLICATION_DISCONNECT;
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
		} break;

		case HOST_USER_CLASS_ACTIVE : {
			Appli_state = APPLICATION_READY;
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		} break;

		case HOST_USER_CONNECTION : {
			Appli_state = APPLICATION_START;
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		} break;

		default : {

		} break;
	}
}


