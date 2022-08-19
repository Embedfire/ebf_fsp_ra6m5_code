/*
 * keyboard.c
 *
 *  Created on: 2022年8月1日
 *      Author: admin
 */

#define BOOT_PROTOCOL (0)
#define USB_FS_DEVICE_ADDRESS_1    (1)

#include "keyboard.h"
#include "debug_uart.h"

#define SET_PROTOCOL               (USB_HID_SET_PROTOCOL | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)

void USB_Init(void)
{
    R_USB_Open(Keyboard_USB.p_ctrl, Keyboard_USB.p_cfg);
}



static usb_status_t     event;
usb_event_info_t event_info;
static uint16_t buffer_size;
static uint8_t buffer[20];
static uint32_t setup_data;

static void Send_Pressed_Key(void);
static void set_protocol (usb_instance_ctrl_t * p_ctrl, uint8_t protocol, uint8_t device_address);
void USB_Event_Deal(void)
{
    Keyboard_USB.p_api->eventGet(&event_info, &event);  //event code get
    switch(event)
    {
        case USB_STATUS_CONFIGURED:
            R_USB_HHID_MaxPacketSizeGet(Keyboard_USB.p_ctrl, &buffer_size, USB_HID_IN, event_info.device_address);
            set_protocol (Keyboard_USB.p_ctrl, BOOT_PROTOCOL, event_info.device_address);
            break;
        case USB_STATUS_READ_COMPLETE:
            Send_Pressed_Key();
            R_USB_Read(Keyboard_USB.p_ctrl, buffer, buffer_size, event_info.device_address);
            break;
        case USB_STATUS_REQUEST_COMPLETE:
            if (USB_HID_SET_PROTOCOL == (event_info.setup.request_type & USB_BREQUEST))
            {
                R_USB_Read(Keyboard_USB.p_ctrl, buffer, buffer_size, event_info.device_address);
            }
            break;
        default:
            break;
    }

}

//将按下的按键发送给串口
static uint8_t qwerty_Code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
static void Send_Pressed_Key(void)
{
    if(buffer[2] == 0)return;
    printf("Press key: ");
    for (uint32_t i = 2; i < buffer_size; i++)  //只能显示字母A-Z和数字0-9
    {
        printf("%c",qwerty_Code[buffer[i]-4]);
    }
    printf("\r\n");
}

static void set_protocol (usb_instance_ctrl_t * p_ctrl, uint8_t protocol, uint8_t device_address)
{
    usb_setup_t setup;
    setup.request_type   = SET_PROTOCOL;                                                  /* bRequestCode:SET_PROTOCOL, bmRequestType */
    setup.request_value  = protocol;                                                      /* wValue: Protocol Type */
    setup.request_index  = 0x0000;                                                        /* wIndex:Interface */
    setup.request_length = 0x0000;                                                        /* wLength:Zero */
    R_USB_HostControlTransfer(p_ctrl, &setup, (uint8_t *) &setup_data, device_address);   /* Request Control transfer */
}
