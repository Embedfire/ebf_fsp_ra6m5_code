/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/

/* Access to peripherals and board defines. */
#include "bsp_api.h"
#include "r_ether_phy.h"

#include "hal_data.h"
#include "stdio.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/


/* LAN8720 */
#define ETHER_PHY_REG_SPECIAL_CONTROL_INDICATION    27
#define ETHER_PHY_AMDIXCTRL_DISABLE                 (1u<<15)
#define ETHER_PHY_CH_SELECT                         (0u<<13)

/* Standard PHY Registers */
#define ETHER_PHY_REG_CONTROL                   (0)
#define ETHER_PHY_REG_STATUS                    (1)
#define ETHER_PHY_REG_IDENTIFIER1               (2)
#define ETHER_PHY_REG_IDENTIFIER2               (3)
#define ETHER_PHY_REG_AN_ADVERTISEMENT          (4)
#define ETHER_PHY_REG_AN_LINK_PARTNER           (5)
#define ETHER_PHY_REG_AN_EXPANSION              (6)


/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global function
 ***********************************************************************************************************************/
void ether_phy_targets_initialize(ether_phy_instance_ctrl_t * p_instance_ctrl);
bool ether_phy_targets_is_support_link_partner_ability(ether_phy_instance_ctrl_t * p_instance_ctrl,
                                                       uint32_t                    line_speed_duplex);
extern uint32_t ether_phy_read(ether_phy_instance_ctrl_t * p_instance_ctrl, uint32_t reg_addr);
extern void     ether_phy_write(ether_phy_instance_ctrl_t * p_instance_ctrl, uint32_t reg_addr, uint32_t data);

/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: ether_phy_targets_initialize
 * Description  : PHY-LSI specific initialization processing
 * Arguments    : p_api_ctrl -
 *                    Ethernet channel number
 * Return Value : none
 ***********************************************************************************************************************/
void ether_phy_targets_initialize (ether_phy_instance_ctrl_t * p_instance_ctrl)
{
    uint32_t reg;

    /* HP Auto-MDIX */
    /* Read the special control/status indications register (Register Index: 27) */
    reg = ether_phy_read(p_instance_ctrl, ETHER_PHY_REG_SPECIAL_CONTROL_INDICATION);
    /* Disable Auto-MDIX and set MDI (TX=transmits, RX=receives) */
    reg |= (uint16_t) (ETHER_PHY_AMDIXCTRL_DISABLE | ETHER_PHY_CH_SELECT);
    /* Write the settings back to the special control/status indications register (Register Index: 27) */
    ether_phy_write(p_instance_ctrl, ETHER_PHY_REG_SPECIAL_CONTROL_INDICATION, reg);

#ifdef ETHERNET_PHY_DEBUG
    //Dump PHY registers for debugging purpose
    lan8720DumpPhyReg(p_instance_ctrl);
#endif
}

/***********************************************************************************************************************
 * Function Name: ether_phy_targets_is_support_link_partner_ability
 * Description  : Check if the PHY-LSI connected Ethernet controller supports link ability
 * Arguments    : p_instance_ctrl -
 *                    Ethernet control block
 *                line_speed_duplex -
 *                    Line speed duplex of link partner PHY-LSI
 * Return Value : bool
 ***********************************************************************************************************************/
bool ether_phy_targets_is_support_link_partner_ability (ether_phy_instance_ctrl_t * p_instance_ctrl,
                                                        uint32_t                    line_speed_duplex)
{
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
    FSP_PARAMETER_NOT_USED(line_speed_duplex);

    /* This PHY-LSI supports half and full duplex mode. */
    return true;
}                                      /* End of function ether_phy_targets_is_support_link_partner_ability() */
