/**
 * @file pwm.c
 * @author Matthew Hardenburgh
 * @brief demonstration code for the pwm peripheral. 
 * @copyright copyright (c) 2026 Matthew Hardenburgh, all rights reserved
 * @date 4/29/26
*/
#include <stdint.h>

#define VGA_CTL_REG 0x80001500
#define VGA_OUTPUT_ENABLE_REG 0x80001504
#define VGA_PIXEL_ADDR 0x80001508
#define VGA_PIXEL_DATA 0x8000150C

#define FB_ROW 240
#define FB_COL 320


uint32_t readReg(uint32_t address)
{
    return *(volatile unsigned *)address;
}

void writeReg(uint32_t address, uint32_t value)
{
    *(volatile unsigned *)address = value;
}

initPeripheral()
{
    // enable VGA
    writeReg(VGA_CTL_REG, 0x00000001);
}

int main()
{
    initPeripheral();
    while(1)
    {
        writeReg(VGA_CTL_REG, 0x00000003);
        for(int iIter = 0; iIter < FB_ROW; iIter++)
        {
            // row by column
            for(int jIter = 0; jIter < FB_COL; jIter++)
            {
                
                writeReg(VGA_PIXEL_ADDR, (iIter<<12 | jIter));
                if((iIter + jIter)&1)
                {
                    writeReg(VGA_PIXEL_DATA, 0x00000F0F);
                }
                else
                {
                    writeReg(VGA_PIXEL_DATA, 0x000000F0);
                }
            }
        }
        writeReg(VGA_CTL_REG, 0x0000003);
        
    }
}