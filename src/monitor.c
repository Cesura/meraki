#include "monitor.h"

u16int *video_memory = (u16int *)0xB8000;

// Stores the cursor position.
u8int cursor_x = 0;
u8int cursor_y = 0;

// Updates the hardware cursor
static void move_cursor()
{
    // The screen is 80 characters wide...
    u16int cursorLocation = cursor_y * 80 + cursor_x;
    outb(0x3D4, 14);
    outb(0x3D5, cursorLocation >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursorLocation);
}

// Scrolls the text on the screen up by one line
static void scroll()
{

    u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    u16int blank = 0x20 /* space */ | (attributeByte << 8);

    // Row 25 is the end, this means we need to scroll up
    if(cursor_y >= 25)
    {
        int i;
        for (i = 0*80; i < 24*80; i++)
        {
            video_memory[i] = video_memory[i+80];
        }

        for (i = 24*80; i < 25*80; i++)
        {
            video_memory[i] = blank;
        }
        
	cursor_y = 24;
    }
}

// Print single character
void kputc(char c)
{
    u8int backColour = 0;
    u8int foreColour = 15;

    u8int  attributeByte = (backColour << 4) | (foreColour & 0x0F);
    u16int attribute = attributeByte << 8;
    u16int *location;

    // Handle backspace
    if (c == 0x08 && cursor_x)
    {
        cursor_x--;
    }

    // Handle tab
    else if (c == 0x09)
    {
        cursor_x = (cursor_x+8) & ~(8-1);
    }

    // Handle carriage return
    else if (c == '\r')
    {
        cursor_x = 0;
    }

    // Handle newline by moving cursor back to left and increasing the row
    else if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
    }

    // Handle any other printable character.
    else if(c >= ' ')
    {
        location = video_memory + (cursor_y*80 + cursor_x);
        *location = c | attribute;
        cursor_x++;
    }

    // Check if we need newline
    if (cursor_x >= 80)
    {
        cursor_x = 0;
        cursor_y ++;
    }

    scroll();
    move_cursor();

}

// Clears the screen, by copying lots of spaces to the framebuffer.
void kclear()
{
    u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    u16int blank = 0x20 /* space */ | (attributeByte << 8);

    int i;
    for (i = 0; i < 80*25; i++)
    {
        video_memory[i] = blank;
    }

    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}

// Outputs an ASCII string to the monitor
void kprint(char *c)
{
    int i = 0;
    while (c[i])
    {
        kputc(c[i++]);
    }
}

void kputhex(u32int n)
{
    s32int tmp;

    kprint("0x");

    char noZeroes = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }
    
        if (tmp >= 0xA)
        {
            noZeroes = 0;
            kputc(tmp-0xA+'a');
        }
        else
        {
            noZeroes = 0;
            kputc(tmp+'0');
        }
    }
  
    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        kputc(tmp-0xA+'a');
    }
    else
    {
        kputc(tmp+'0');
    }

}

void kputdec(u32int n)
{

    if (n == 0)
    {
        kputc('0');
        return;
    }

    s32int acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    kprint(c2);

}
