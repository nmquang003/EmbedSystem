#include "flash_if.h"
#include "stm32f4xx_hal.h"

HAL_StatusTypeDef FLASH_EraseSector6(void)
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError;

    // Mở khóa cho phép ghi Flash
    HAL_FLASH_Unlock();

    // Cấu hình thông tin xóa
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector        = FLASH_SECTOR_6;  // Sector 6
    EraseInitStruct.NbSectors     = 1;               // Số sector cần xóa
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3; // Điện áp 2.7~3.6V

    // Tiến hành xóa
    status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

    // Khóa Flash sau khi xóa
    HAL_FLASH_Lock();

    return status;
}

HAL_StatusTypeDef FLASH_WriteGameScore(const uint16_t *scoreArr, uint32_t length)
{
    HAL_StatusTypeDef status;
    uint32_t address = FLASH_USER_START_ADDR; // Bắt đầu ghi từ địa chỉ này

    // Mở khóa Flash trước khi ghi
    HAL_FLASH_Unlock();

    // Ghi từng phần tử của mảng
    for(uint32_t i = 0; i < length; i++)
    {
        // Ở STM32F4, hàm HAL_FLASH_Program hỗ trợ kiểu:
        // - FLASH_TYPEPROGRAM_BYTE
        // - FLASH_TYPEPROGRAM_HALFWORD (16 bit)
        // - FLASH_TYPEPROGRAM_WORD (32 bit)
        // - FLASH_TYPEPROGRAM_DOUBLEWORD (64 bit)

        // Ở đây ta ghi HALFWORD (16 bit)
        status = HAL_FLASH_Program(
            FLASH_TYPEPROGRAM_HALFWORD,
            address,
            scoreArr[i]
        );

        if(status != HAL_OK)
        {
            // Nếu ghi lỗi, khóa Flash và trả về
            HAL_FLASH_Lock();
            return status;
        }

        address += 2; // Tăng địa chỉ thêm 2 byte cho lần ghi tiếp theo
    }

    // Khóa Flash sau khi ghi
    HAL_FLASH_Lock();
    return HAL_OK;
}


void FLASH_ReadGameScore(uint16_t *destArr, uint32_t length)
{
    uint32_t address = FLASH_USER_START_ADDR;

    for(uint32_t i = 0; i < length; i++)
    {
        // Ép kiểu về con trỏ 16-bit rồi lấy giá trị
        destArr[i] = *(__IO uint16_t *)address;
        address += 2;
    }
}
