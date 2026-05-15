/**
 * @file Character_Display.c
 * @author Matthew Hardenburgh (Revised by Jose Ramirez)
 * @brief Shows numbers 0 through 9 on a display using the vga peripheral.
 * @copyright copyright (c) 2026 Matthew Hardenburgh, all rights reserved
 * @date 5/14/26
*/
#include <stdint.h>

#define VGA_CTL_REG           0x80001500
#define VGA_OUTPUT_ENABLE_REG 0x80001504
#define VGA_PIXEL_ADDR        0x80001508
#define VGA_PIXEL_DATA        0x8000150C

#define FB_ROW 240
#define FB_COL 320

#define COLOR_BLACK 0x000
#define COLOR_BLUE  0x00F
#define COLOR_RED   0xF00

#define CHAR_W 5
#define CHAR_H 7
#define SCALE  3

uint32_t readReg(uint32_t address)
{
    return *(volatile uint32_t *)address;
}

void writeReg(uint32_t address, uint32_t value)
{
    *(volatile uint32_t *)address = value;
}

void initPeripheral(void)
{
    writeReg(VGA_CTL_REG, 0x00000001);
}

void delay_loop(void)
{
    volatile uint32_t dummy = 0;
    for(uint32_t i = 0; i < 100000; i++)
        dummy = (dummy + 1) >> 1;
}

void putPixel(uint32_t row, uint32_t col, uint16_t color)
{
    if(row < FB_ROW && col < FB_COL)
    {
        writeReg(VGA_PIXEL_ADDR, (row << 12) | col);
        writeReg(VGA_PIXEL_DATA, color);
    }
}

void clearScreen(uint16_t color)
{
    writeReg(VGA_CTL_REG, 0x00000003);

    for(uint32_t row = 0; row < FB_ROW; row++)
    {
        for(uint32_t col = 0; col < FB_COL; col++)
        {
            putPixel(row, col, color);
        }
    }

    writeReg(VGA_CTL_REG, 0x00000001);
}

static const uint8_t font5x7[][7] = {
    // 0
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00},

    // 1
    {0x11,0x11,0x11,0x1F,0x01,0x01,0x01},

    // 2
    {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E},

    // 3
    {0x11,0x19,0x15,0x13,0x11,0x11,0x11},

    // 4
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},

    // e
    {0x00,0x0E,0x11,0x1F,0x10,0x11,0x0E},

    // s
    {0x00,0x0F,0x10,0x0E,0x01,0x1E,0x00},

    // x
    {0x00,0x11,0x0A,0x04,0x0A,0x11,0x00},

    // y
    {0x00,0x11,0x11,0x0F,0x01,0x11,0x0E},
};

int fontIndex(char c)
{
    switch(c)
    {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        default:  return 0;
    }
}

void drawChar(int x, int y, char c, uint16_t color)
{
    int idx = fontIndex(c);

     writeReg(VGA_CTL_REG, 0x00000003);
    for(int row = 0; row < CHAR_H; row++)
    {
        uint8_t bits = font5x7[idx][row];

        for(int col = 0; col < CHAR_W; col++)
        {
            if(bits & (1 << (CHAR_W - 1 - col)))
            {
                for(int sy = 0; sy < SCALE; sy++)
                {
                    for(int sx = 0; sx < SCALE; sx++)
                    {
                        putPixel(y + row*SCALE + sy,
                                 x + col*SCALE + sx,
                                 color);
                    }
                }
            }
        }
    }
    writeReg(VGA_CTL_REG, 0x00000001);
}

void drawText(int x, int y, const char *text, uint16_t color)
{
    while(*text)
    {
        drawChar(x, y, *text, color);
        x += (CHAR_W + 1) * SCALE;
        text++;
    }
}

int main(void)
{
    const char Numbers [] = {'0','1','2','3','4','5','6','7','8','9'};

    const int textWidth  = 11 * (CHAR_W + 1) * SCALE;
    const int textHeight = CHAR_H * SCALE;
    const int y = (FB_ROW - textHeight) / 2;
    int idx = 0;

    initPeripheral();

    while(1)
    {
        for(int x = -textWidth; x < FB_COL; x += 2)
        {
            clearScreen(COLOR_RED);
            drawChar(x, y, Numbers[idx], COLOR_BLACK);
            if (idx < 9)
                idx++;
            else
                idx = 0;
            delay_loop();
        }
    }
}
