#include "kernel.h"

void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void update_cursor(int x, int y)
{
    uint16_t pos = y * VGA_WIDTH + x;

    outb(0x3D4, 0x0F);             // On dit "je veux écrire dans le registre 0x0F (byte bas)"
    outb(0x3D5, (uint8_t)(pos));   // On écrit les 8 bits de poids faible

    outb(0x3D4, 0x0E);             // On dit "je veux écrire dans le registre 0x0E (byte haut)"
    outb(0x3D5, (uint8_t)(pos >> 8)); // On écrit les 8 bits de poids fort
}