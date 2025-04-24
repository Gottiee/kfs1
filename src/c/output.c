#include "kernel.h"

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;
uint16_t vga_buffer[5][VGA_SIZE];
uint8_t actual_screen = 0;
size_t row[5];
size_t column[5];

void switch_screen(uint8_t screen)
{
    if (screen == actual_screen)
        return;
    memcpy(vga_buffer[actual_screen], terminal_buffer, VGA_SIZE * sizeof(uint16_t));
    memcpy(terminal_buffer, vga_buffer[screen], VGA_SIZE * sizeof(uint16_t));
    row[actual_screen] = terminal_row;
    column[actual_screen] = terminal_column;
    terminal_row = row[screen];
    terminal_column = column[screen];
    update_cursor(terminal_column, terminal_row);
    actual_screen = screen;
}

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_scroll()
{
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    terminal_row = VGA_HEIGHT - 1;
}

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
    for (int i = 0; i < 5; i++)
        memcpy(vga_buffer[i], terminal_buffer, VGA_SIZE * sizeof(uint16_t));
    memset(row, 0, sizeof(row));
    memset(column, 0, sizeof(column));
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        terminal_column++;
        if (terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
        }
    }
    if (terminal_row == VGA_HEIGHT) {
        terminal_scroll();
    }
    update_cursor(terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void write(const char* data)
{
	terminal_write(data, strlen(data));
}

void delete_char()
{
	if (terminal_column == 0)
	{
		terminal_row --;
		terminal_column = 79;
	}
	else 
	{
		terminal_column --;
	}
	terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    update_cursor(terminal_column, terminal_row);
}