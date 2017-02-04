    /*
 * CS 1550: Graphics library skeleton code for Qemu VM
 * WARNING: This code is the minimal implementation of the project 1.
 *          It is not intended to serve as a reference implementation.
 *          Following project guidelines, a complete implementation
 *          for this project will contain ~300 lines or more.
 * (c) Mohammad H. Mofrad, 2017
 */

#include "library.h"

int fid;
int fid1 = 1;
size_t size;
color_t *address;

void init_graphics()
{
    /* Open fb file descriptor */
    fid = open("/dev/fb0", O_RDWR);
    if(fid == -1)
    {
        perror("Error opening /dev/fb0");
        exit(1);
    }

    struct fb_var_screeninfo var_info;
    ioctls(fid, FBIOGET_VSCREENINFO, &var_info);
    struct struct fb_fix_screeninfo fix_info;
    ioctls(fid, FBIOGET_FSCREENINFO, &fix_info);

    size = var_info.yres_virtual * fix_info.line_length;
    address = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fid, 0);
    if(address == (void *) -1)
    {
        perror("Error mapping memory");
        exit(1);
    }

    int ter = open("/dev/tty0", O_RDWR);
    struct termios terminal;
    ioctls(ter, TCGETS, &terminal);
    terminal.ICANON = 0;
    terminal.ECHO = 0;
    ioctls(ter, TCSETS, &terminal);
}

void draw_line(color_t c)
{
    /* Print a single line */
    color_t off_p = 0;
    for(off_p =0; off_p < size; off_p++)
    {
        *(address + off_p) = RMASK(c) | GMASK(c) | BMASK(c);
        /*
          printf("Address(0x%08x), Color(0x%04x) B(0x%04x), G(0x%04x), R(0x%04x) \n",
                (address + off_p), *(address + off_p), BMASK(c), GMASK(c), RMASK(c));
        */
    }
}

void sleep_ms(unsigned ms)
{
   nanosleep( ms * 1000000, NULL);
}

void clear_screen()
{
    write(fid1, "\033[2J", 4);  /* This will do the trick for fid1*/
}

void exit_graphics()
{
    int ter = open("/dev/tty0", O_RDWR);
    struct termios terminal;
    ioctls(ter, TCGETS, &terminal);
    terminal.ICANON = 1;
    terminal.ECHO = 1;
    ioctls(ter, TCSETS, &terminal);

    close(ter);

    if(munmap(address, size) == -1)
    {
        perror("Error unmapping memory");
        exit(1);
    }

    if(!close(fid))
    {
        exit(0);
    }
    else
    {
        perror("Error closing /dev/fb0");
        exit(1);
    }
}
