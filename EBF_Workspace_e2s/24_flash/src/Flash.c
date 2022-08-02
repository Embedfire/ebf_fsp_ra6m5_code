#include "Flash.h"


void Flash_Init()
{
	R_QSPI_Open(Flash.p_ctrl, Flash.p_cfg);
}