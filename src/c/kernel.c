#include "kernel.h"

void kernel_main(void) 
{
	terminal_initialize();

	write("/* **************************************************** */\n/*                                                      */\n/*                                  :::      ::::::::   */\n/*   KFS1                         :+:      :+:    :+:   */\n/*                              +:+ +:+         +:+     */\n/*   By: ");
	terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK));	
	write("eedy");
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	write(" && ");
	terminal_setcolor(vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK));	
	write("anvicent");
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	write("     +#+  +:+       +#+        */\n/*                          +#+#+#+#+#+   +#+           */\n/*                               #+#    #+#             */\n/*                              ###   ########.fr       */\n/*                                                      */\n/* **************************************************** */\n");
	while(1){
		keyboard_handler();
	}
}