#include "bsp_flash.h"
#include "string.h"


//static uint32_t GetSector(uint32_t Address);
static uint32_t GetSectorSize(uint32_t Sector);

HAL_StatusTypeDef  STM32_Flash_Erase(uint32_t Sector, uint32_t NbSectors)
{
	
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t SectorError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	
	HAL_FLASH_Unlock();
	
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = Sector;
  EraseInitStruct.NbSectors = NbSectors;
	
	status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	
	HAL_FLASH_Lock();
	
	return status;
}


HAL_StatusTypeDef STM32_Flash_Write(uint32_t Address, uint8_t *pData, uint16_t Len)
{
	uint32_t SectorError = 0;
//	uint32_t PageAddress_Start, PageAddress_End;
	HAL_StatusTypeDef status = HAL_ERROR;
	
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t Address_End;
	
	uint32_t FirstSector = 0, NbOfSectors = 0;
	
	Address_End = Address + Len;
	
	if((Address >= FLASH_USER_START_ADDR) && (Address_End <= FLASH_USER_END_ADDR))
	{
		HAL_FLASH_Unlock();
		
		for(uint16_t index = 0 ; index < Len ; )
		{
			status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address, *((uint16_t*)pData));
			if( status !=  HAL_OK)
			{
				return status;
			}
			else
			{
				pData += 2;
				Address += 2;
				index += 2;
			}
		}
		HAL_FLASH_Lock();
	}
	else
	{
		return status;
	}
	return HAL_OK;
}

HAL_StatusTypeDef STM32_Flash_EraseWrite(uint32_t Address, uint8_t *pData, uint16_t Len)
{
	uint32_t SectorError = 0;
//	uint32_t PageAddress_Start, PageAddress_End;
	HAL_StatusTypeDef status = HAL_ERROR;
	
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t Address_End;
	
	uint32_t FirstSector = 0, NbOfSectors = 0;
	
	Address_End = Address + Len;
	
	if((Address >= FLASH_USER_START_ADDR) && (Address_End <= FLASH_USER_END_ADDR))
	{
		HAL_FLASH_Unlock();
		
		FirstSector = GetSector(Address);		
		NbOfSectors = GetSector(Address + Len) - FirstSector + 1;
			
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;
		EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		EraseInitStruct.Sector = FirstSector;
		EraseInitStruct.NbSectors = NbOfSectors;
		
		status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
		
		if( status !=  HAL_OK)
		{
			return status;
		}
		
		for(uint16_t index = 0 ; index < Len ; )
		{
			status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address, *((uint16_t*)pData));
			if( status !=  HAL_OK)
			{
				return status;
			}
			else
			{
				pData += 2;
				Address += 2;
				index += 2;
			}
		}
		HAL_FLASH_Lock();
	}
	else
	{
		return status;
	}
	return HAL_OK;
}

HAL_StatusTypeDef STM32_Flash_Read(uint32_t Address, uint8_t *pData, uint16_t Len)
{
	memcpy(pData,(uint8_t*)Address,Len);
	return HAL_OK;
}

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
   else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_SECTOR_11;
  }

  return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
static uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;

  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}






