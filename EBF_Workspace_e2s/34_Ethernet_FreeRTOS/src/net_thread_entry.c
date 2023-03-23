//#include "net_thread.h"
//                /* Net Thread entry function */
//                /* pvParameters contains TaskHandle_t */
//                void net_thread_entry(void * pvParameters)
//                {
//                    FSP_PARAMETER_NOT_USED(pvParameters);

//                    /* TODO: add your own code here */
//                    while(1)
//                    {
//                        vTaskDelay(1);
//                    }
//                }


#include "net_thread.h"
#include "net_user_app.h"


extern char *domain_name;
extern char *remote_ip_address;

extern uint8_t ucMACAddress[ 6 ];
extern uint8_t ucIPAddress[ 4 ];
extern uint8_t ucNetMask[ 4 ];
extern uint8_t ucGatewayAddress[ 4 ];
extern uint8_t ucDNSServerAddress[ 4 ];

extern ping_data_t ping_data;
uint32_t  usrPingCount  = RESET_VALUE;
static uint32_t usr_print_ability = RESET_VALUE;



/* Net Thread entry function */
/* pvParameters contains TaskHandle_t */
void net_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    
    BaseType_t status = pdFALSE;
    fsp_pack_version_t version = {RESET_VALUE};
    
    
    /* 调试串口初始化 */
    Debug_UART4_Init();
    printf("Hello! FreeRTOS-Plus-TCP\r\n"
           "这是一个以太网 DNS & Ping 示例，请把网线接到开发板和路由器进行联网\r\n");
    
    /* 获取 FSP 库版本 */
    R_FSP_VersionGet(&version);
    /* 打印 FSP 库版本 */
    printf("Flex Software Pack Version  %d.%d.%d\r\n", 
            version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    
    printf("\r\n预置ipconfig:");
    print_ipconfig();
    
    /* FreeRTOS IP Initialization: This init initializes the IP stack  */
    status = FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
    if(pdFALSE == status)
    {
        printf("FreeRTOS_IPInit Failed\r\n"
               "Returned Error Code: 0x%x\r\n", status);
        __asm("BKPT #0\n"); /* trap upon the error  */
    }
    
    
    /* TODO: add your own code here */
    while(1)
    {
        /* Check if Both the Ethernet Link and IP link are UP */
        if(SUCCESS == isNetworkUp())
        {
            /* usr_print_ability is added to avoid multiple UP messages or Down Messages repeating*/
            if(!(PRINT_UP_MSG_DISABLE & usr_print_ability))
            {
                printf("\r\nNetwork is Up\r\n");
                usr_print_ability |= PRINT_UP_MSG_DISABLE;
            }

            if(!(PRINT_NWK_USR_MSG_DISABLE & usr_print_ability))
            {
#if( ipconfigUSE_DHCP != 0 )
                /* Display the New IP credentials obtained from the DHCP server */
                updateDhcpResponseToUsr();
#endif
                /* Updated IP credentials on to the RTT console */
                printf("更新ipconfig:");
                print_ipconfig();
                /* DNS lookup for the Domain name requested. This is Synchronous Activity */
                dnsQuerryFunc(domain_name);
            }

            if(!(PRINT_NWK_USR_MSG_DISABLE & usr_print_ability))
            {
                printf("\r\nPinging %s:\r\n\r\n",(char *)remote_ip_address);
            }

            while (usrPingCount < USR_PING_COUNT)
            {
                /* Send a ICMP Ping request to the requested IP address
                 * USR_PING_COUNT (100) is used in this Example Project
                 * For Continuous testing the count can be increased to bigger number
                 */
                status =  vSendPing((char *)remote_ip_address);
                if(status != pdFALSE)
                {
                    ping_data.sent++;
                    printf("!");    // Ping请求发送成功
                }
                else
                {
                    ping_data.lost++;
                    printf(".");
                }
                usrPingCount++;
                /* Add some delay between Pings */
                vTaskDelay(10);
            }

            if(!(PRINT_NWK_USR_MSG_DISABLE & usr_print_ability))
            {
                print_pingResult();
                usr_print_ability |= PRINT_NWK_USR_MSG_DISABLE;
            }
        }
        else
        {
            if(!(PRINT_DOWN_MSG_DISABLE & usr_print_ability))
            {
                 printf("\r\nNetwork is Down");
                 usr_print_ability |= PRINT_DOWN_MSG_DISABLE;
            }
            else
            {
                 printf(".");
            }
        }
        vTaskDelay(100);
    }
}

