/*********************************************************************************************/

【*】 程序简介 
-工程名称：DTC_Use_UART
-实验平台: 野火_瑞萨启明6M5开发板
-e2 studio版本：
-FSP固件库版本：4.0.0


【 ！】功能简介：
使用 CANFD0 的测试模式

【 ！】实验操作：
将开发板通过type-c线连接至电脑，并打开串口调试助手，打开对应的COM口，
编译并下载程序到开发板并复位，按下SW2进行测试模式切换，按下SW3进行数据传输

【 ！】切换测试模式：
内部回环模式：
    err = g_canfd_on_canfd.modeTransition(  //内部回环
            &g_canfd0_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_INTERNAL);
外部回环模式：
    err = g_canfd_on_canfd.modeTransition(  //外部回环
            &g_canfd0_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_EXTERNAL);
            
/*********************************************************************************************/

【*】 引脚分配
LED：
LED灯的三个阴极接到6M5三个引脚上，LED低电平亮。

	LED_R<--->P601
	LED_B<--->P602
	LED_G<--->P604
	
UART4：
野火_瑞萨启明6M5开发板上的UART4的两个引脚已在开发板上连接USB转串口的对应引脚，
直接连接type-c接口到电脑即可。

	UART4_RXD<--->P511
	UART4_TXD<--->P512
	
canfd0：
canfd0的2个引脚已经连接到can收发器上了，对外引脚为 CAN0_H 和 CAN0_L

	CAN0_RXD<--->P402
	CAN0_TXD<--->P401
	

【*】 接线
内部回环模式：
    无需另外接线
外部回环模式：
	- 如果引脚有外接can收发器，需要在 CAN_H 和 CAN_L 之间加上一个120欧姆的电阻(开发板上已经连接了)。
	 （不需要另外将 H L 连接）
	- 如果没有外接can收发器，则需要将 CAN_TX 和 CAN_RX 引脚连接

/*********************************************************************************************/
【*】 联系我们

-野火官网  :https://embedfire.com
-野火论坛  :http://www.firebbs.cn
-野火商城  :https://yehuosm.tmall.com/
-野火资料下载中心：http://doc.embedfire.com/products/link

/*********************************************************************************************/

