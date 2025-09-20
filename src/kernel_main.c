
#include <stdint.h>
#include "rprintf.h"

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

struct termbuf {
    //what to write
    char ascii; 
    //text color
    char color;  
};

//80x25 chars
#define COLS 80
#define ROWS 25

static struct termbuf *const VRAM = (struct termbuf *)0xB8000; 

//track cursor position
static int x = 0;
static int y = 0;

//clear screen with all spaces
static void clear_screen(void) {
    for(int r = 0; r < ROWS; r++) {
        for(int c = 0; c < COLS; c++) {
            //index
            int i = r * COLS + c;
            //set the vram index to an empty space
            VRAM[i].ascii = ' ';
            //set vram index color to a base
            VRAM[i].color = 0x07; 
        }
    }
    //reset cursor position
    x = 0;
    y = 0;
}


//Scroll one line if cursor moves past the last row
static void scroll(void) {
    //if y isn't >= number of rows we don't need to scroll
    if (y < ROWS) {
        return;
    }

    //Move rows 
    for(int r = 1; r < ROWS; r++) {
        for(int c = 0; c < COLS; c++) {
            int src = r * COLS + c; 
            //destination is one row up from src
            int dst = (r-1) * COLS + c;
            VRAM[dst] = VRAM[src];
        }
    }

    //clear the last row to make space
    for(int c = 0; c < COLS; c++) {
        int i = (ROWS - 1) * COLS + c; 
        //set the ascii to empty space
        VRAM[i].ascii = ' ';
        //set color to base
        VRAM[i].color = 0x07;
    }

    //place cursor at the beginning of the last row 
    x = 0;
    y = ROWS - 1;
}

//write one char and advance cursor
int putc(int ch) {

    //handle return
    if (ch == '\r') {  
        x = 0;
        return ch;
    }

    //handle new line
    if (ch == '\n') { 
        x = 0;
        y++;
        scroll();
        return ch;
    }

    //index to write at
    int i = y * COLS + x;

    //set the ascii and color
    VRAM[i].ascii = (char)ch;
    VRAM[i].color = 0x07;

    //Move cursor forward
    if(++x >= COLS) {
        x = 0;
        y++;
        scroll();
    }

    return ch;

}

void main() {
    clear_screen();

    for(int i = 0; i < 25; i ++) {
        esp_printf(putc, "Hello this is a test of the putc func\r\n");

        esp_printf(putc, "numbers and chars work: %d, char:%c\r\n", 123, 'A');
    }

    for(;;) {}
}
