/**
  ******************************************************************************
  * STM32L4 Bootloader
  ******************************************************************************
  * @author Akos Pasztor
  * @file   bootloader.c
  * @brief  Bootloader implementation
  *	        This file contains the functions of the bootloader. The bootloader 
  *	        implementation uses the official HAL library of ST.
  * @see    Please refer to README for detailed information.
  ******************************************************************************
  * Copyright (c) 2017 Akos Pasztor.                    https://akospasztor.com
  ******************************************************************************
**/

#include "stm32l4xx.h"
#include "bootloader.h"

/* Private typedef -----------------------------------------------------------*/
typedef void (*pFunction)(void);

/* Private variables ---------------------------------------------------------*/
static uint32_t flash_ptr = APP_ADDRESS;


/*** Check if application fits into user flash ********************************/
uint8_t Bootloader_CheckSize(uint32_t appsize)
{
    return ((FLASH_BASE + FLASH_SIZE - APP_ADDRESS) >= appsize) ? BL_OK : BL_SIZE_ERROR;
}

/*** Get flash protection status **********************************************/
uint32_t Bootloader_GetProtectionStatus(void)
{
    uint32_t protection = BL_FLASH_PROT_NONE;
    
    FLASH_OBProgramInitTypeDef OptionsBytesStruct1 = {0},
                               OptionsBytesStruct2 = {0},
                               OptionsBytesStruct3 = {0},
                               OptionsBytesStruct4 = {0};

    HAL_FLASH_Unlock();

    OptionsBytesStruct1.WRPArea = OB_WRPAREA_BANK1_AREAA;
    OptionsBytesStruct1.PCROPConfig = FLASH_BANK_1;
    OptionsBytesStruct2.WRPArea = OB_WRPAREA_BANK1_AREAB;
    OptionsBytesStruct2.PCROPConfig = FLASH_BANK_1;
    OptionsBytesStruct3.WRPArea = OB_WRPAREA_BANK2_AREAA;
    OptionsBytesStruct3.PCROPConfig = FLASH_BANK_2;
    OptionsBytesStruct4.WRPArea = OB_WRPAREA_BANK2_AREAB;
    OptionsBytesStruct4.PCROPConfig = FLASH_BANK_2;
    
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct1);
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct2);
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct3);
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct4);

    HAL_FLASH_Lock();

    /* Check PCROP areas */
    if(OptionsBytesStruct1.PCROPEndAddr > OptionsBytesStruct1.PCROPStartAddr)
    {
        /* check if user area are included inside this range */
        if(OptionsBytesStruct1.PCROPStartAddr >= APP_ADDRESS)
        {
            protection |= BL_FLASH_PROT_PCROP;
        }
    }

    if(OptionsBytesStruct3.PCROPEndAddr > OptionsBytesStruct3.PCROPStartAddr)
    {
        /* check if user area are included inside this range */
        if (OptionsBytesStruct3.PCROPStartAddr >= APP_ADDRESS)
        {
            protection |= BL_FLASH_PROT_PCROP;
        }
    }

    /* check WRP */
    if(OptionsBytesStruct1.WRPEndOffset > OptionsBytesStruct1.WRPStartOffset)
    {
        /* check if area is inside the WRP Range */
        if((OptionsBytesStruct1.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE) >= APP_ADDRESS)
        {
            protection |= BL_FLASH_PROT_WRP;
        }
    }

    if(OptionsBytesStruct2.WRPEndOffset > OptionsBytesStruct2.WRPStartOffset)
    {
        /* check if area is inside the WRP Range */
        if((OptionsBytesStruct2.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE) >= APP_ADDRESS)
        {
            protection |= BL_FLASH_PROT_WRP;
        }
    }

    if(OptionsBytesStruct3.WRPEndOffset > OptionsBytesStruct3.WRPStartOffset)
    {
        /* check if area is inside the WRP Range */
        if((OptionsBytesStruct3.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE + FLASH_PAGE_SIZE * FLASH_PAGE_NBPERBANK) >= APP_ADDRESS)
        {
            protection |= BL_FLASH_PROT_WRP;
        }
    }

    if (OptionsBytesStruct4.WRPEndOffset > OptionsBytesStruct4.WRPStartOffset)
    {
        /* check if area is inside the WRP Range */
        if((OptionsBytesStruct4.WRPStartOffset * FLASH_PAGE_SIZE + FLASH_BASE + FLASH_PAGE_SIZE * FLASH_PAGE_NBPERBANK) >= APP_ADDRESS)
        {
            protection |= BL_FLASH_PROT_WRP;
        }
    }

    if(OptionsBytesStruct4.RDPLevel != OB_RDP_LEVEL_0)
    {
        protection |= BL_FLASH_PROT_RDP;
    }

    return protection;
}

/*** Set flash write protection ***********************************************/
uint32_t Bootloader_SetWriteProtection(uint32_t protection)
{
    FLASH_OBProgramInitTypeDef OptionsBytesStruct = {0};
    HAL_StatusTypeDef ret = HAL_ERROR;

    ret = HAL_FLASH_Unlock();
    ret |= HAL_FLASH_OB_Unlock();

    /* Bank 1 */
//    OptionsBytesStruct.OptionType = OPTIONBYTE_WRP;
//    OptionsBytesStruct.WRPArea = OB_WRPAREA_BANK1_AREAA;
//    if(protection & BL_FLASH_PROT_WRP)
//    {
//        /* Enable the WRP protection for all flash BANK1 except for Bootloader */
//        OptionsBytesStruct.WRPStartOffset = /*(APP_ADDRESS - FLASH_BASE) / FLASH_PAGE_SIZE;*/ 0x00;
//        OptionsBytesStruct.WRPEndOffset = FLASH_PAGE_NBPERBANK - 1;
//    }
//    else
//    {
//        /* Remove all WRP protection */
//        OptionsBytesStruct.WRPStartOffset = 0xFF;
//        OptionsBytesStruct.WRPEndOffset = 0x00;
//    }
//    ret |= HAL_FLASHEx_OBProgram(&OptionsBytesStruct);

    /* Area B is not used */
//    OptionsBytesStruct.OptionType = OPTIONBYTE_WRP;
//    OptionsBytesStruct.WRPArea = OB_WRPAREA_BANK1_AREAB;
//    OptionsBytesStruct.WRPStartOffset = 0xFF;
//    OptionsBytesStruct.WRPEndOffset = 0x00;
//    ret |= HAL_FLASHEx_OBProgram(&OptionsBytesStruct);

    /* Bank 2 */
    OptionsBytesStruct.OptionType = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPArea = OB_WRPAREA_BANK2_AREAA;
    if(protection & BL_FLASH_PROT_WRP)
    {
        /* Enable the WRP protection for all flash BANK2 */
        OptionsBytesStruct.WRPStartOffset = 0x00;
        OptionsBytesStruct.WRPEndOffset = FLASH_PAGE_NBPERBANK - 1;
    }
    else
    {
        /* Remove all WRP protection */
        OptionsBytesStruct.WRPStartOffset = 0xFF;
        OptionsBytesStruct.WRPEndOffset = 0x00;
    }
    ret |= HAL_FLASHEx_OBProgram(&OptionsBytesStruct);

    /* Area B is not used */
    OptionsBytesStruct.OptionType = OPTIONBYTE_WRP;
    OptionsBytesStruct.WRPArea = OB_WRPAREA_BANK2_AREAB;
    OptionsBytesStruct.WRPStartOffset = 0x00;
    OptionsBytesStruct.WRPEndOffset = 0xFF;
    ret |= HAL_FLASHEx_OBProgram(&OptionsBytesStruct);
    
    /* RDP */
//    OptionsBytesStruct.OptionType = OPTIONBYTE_RDP;
//    OptionsBytesStruct.RDPLevel = OB_RDP_LEVEL_0;
//    ret |= HAL_FLASHEx_OBProgram(&OptionsBytesStruct);
    
    if(ret == HAL_OK)
    {
        //HAL_FLASH_OB_Launch();
    }
    
    ret = HAL_FLASH_OB_Lock();
    ret = HAL_FLASH_Lock();

    return(ret == HAL_OK ? BL_OK : BL_PROTECTION_ERROR);
}

/*** Erase flash **************************************************************/
uint8_t Bootloader_Erase(void)
{
    uint32_t NbrOfPages = 0;
    uint32_t PageError = 0;
    FLASH_EraseInitTypeDef pEraseInit;
    HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASH_Unlock();

    /* Get the number of page to  erase */
    NbrOfPages = (FLASH_BASE + FLASH_SIZE);
    NbrOfPages = (NbrOfPages - APP_ADDRESS) / FLASH_PAGE_SIZE;

    if(NbrOfPages > FLASH_PAGE_NBPERBANK)
    {
        pEraseInit.Banks = FLASH_BANK_1;
        pEraseInit.NbPages = NbrOfPages % FLASH_PAGE_NBPERBANK;
        pEraseInit.Page = FLASH_PAGE_NBPERBANK - pEraseInit.NbPages;
        pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

        NbrOfPages = FLASH_PAGE_NBPERBANK;
    }

    if(status == HAL_OK)
    {
        pEraseInit.Banks = FLASH_BANK_2;
        pEraseInit.NbPages = NbrOfPages;
        pEraseInit.Page = FLASH_PAGE_NBPERBANK - pEraseInit.NbPages;
        pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
    }

    HAL_FLASH_Lock();

    if(status != HAL_OK)
    {
        /* Error occurred while page erase */
        return BL_ERASE_ERROR;
    }

    return BL_OK;
}

/*** Initialize flash for programming *****************************************/
void Bootloader_FlashInit(void)
{
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPTVERR);
    HAL_FLASH_Lock();
    
    /* Reset Flash destination address */
    flash_ptr = APP_ADDRESS;
    
    HAL_FLASH_Unlock();
}

/*** Program 64bit data into flash ********************************************/
uint8_t Bootloader_FlashNext(uint64_t data)
{
    if( !(flash_ptr <= (FLASH_BASE + FLASH_SIZE - 8)) || (flash_ptr < APP_ADDRESS) )
    {
        HAL_FLASH_Lock();
        return BL_WRITE_ERROR;
    }
    
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_ptr, data) == HAL_OK)      
    {
        /* Check the written value */
        if(*(uint64_t*)flash_ptr != data)
        {
            /* Flash content doesn't match source content */
            HAL_FLASH_Lock();
            return BL_WRITE_ERROR;
        }   
        /* Increment Flash destination address */
        flash_ptr += 8;
    }
    else
    {
        /* Error occurred while writing data in Flash memory */
        HAL_FLASH_Lock();
        return BL_WRITE_ERROR;
    }
    
    return BL_OK;
}

/*** Finish flash programming *************************************************/
void Bootloader_FlashEnd(void)
{    
    HAL_FLASH_Lock();
}

/*** Verify checksum of application *******************************************/
uint8_t Bootloader_VerifyChecksum(void)
{
#if USE_CHECKSUM
    CRC_HandleTypeDef CrcHandle;
    volatile uint32_t calculatedCrc = 0;
    
    __HAL_RCC_CRC_CLK_ENABLE();
    CrcHandle.Instance = CRC;
    CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;
    CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;
    CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
    CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_WORDS;
    if(HAL_CRC_Init(&CrcHandle) != HAL_OK)
    {    
        return BL_CHKS_ERROR;
    }
    
    calculatedCrc = HAL_CRC_Calculate(&CrcHandle, (uint32_t*)APP_ADDRESS, APP_SIZE);
    
    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();
    
    if( (*(uint32_t*)CRC_ADDRESS) != calculatedCrc)
    {
        return BL_CHKS_ERROR;
    }
#endif
    return BL_OK;
}

/*** Check for application in user flash **************************************/
uint8_t Bootloader_CheckForApplication(void)
{
    return ( ((*(__IO uint32_t*)APP_ADDRESS) & 0x2FFE0000) == 0x20000000 ) ? BL_OK : BL_NO_APP;
}

/*** Jump to application ******************************************************/
void Bootloader_JumpToApplication(void)
{
    uint32_t  JumpAddress = *(__IO uint32_t*)(APP_ADDRESS + 4);
    pFunction Jump = (pFunction)JumpAddress;
    
    HAL_RCC_DeInit();
    HAL_DeInit();
    
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    
#if SET_VECTOR_TABLE
    SCB->VTOR = APP_ADDRESS;
#endif
    
    __set_MSP(*(__IO uint32_t*)APP_ADDRESS);
    Jump();
}

/*** Jump to System Memory (ST Bootloader) ************************************/
void Bootloader_JumpToSysMem(void)
{
    uint32_t  JumpAddress = *(__IO uint32_t*)(SYSMEM_ADDRESS + 4);
    pFunction Jump = (pFunction)JumpAddress;
    
    HAL_RCC_DeInit();
    HAL_DeInit();
    
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    
    __set_MSP(*(__IO uint32_t*)SYSMEM_ADDRESS);
    Jump();
    
    while(1);
}

