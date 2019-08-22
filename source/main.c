#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#define FB_WIDTH  1280
#define FB_HEIGHT 720

#define MAX_WIDTH  1280
#define MAX_HEIGHT 720
#define MAX_SCALE  80
#define MAX_MAP_SIZE (MAX_WIDTH * MAX_HEIGHT)

//functions
void init_map();
void init_cells();
void update_all_cells();
void draw_all_cells();
void swap_offset();

int sum_cells_in_area(int x, int y);

void set_pixel(int x, int y, int rgba8888);
void set_pos(int x, int y, int value);
int get_pos(int x, int y);

//variables
int map_width;  //x size
int map_height; //y size
int map_scale;  //zoom on screen
int map_offset; //which half of array is being written to

int cells[MAX_MAP_SIZE * 2]; //max map size * 2

//stuff I don't really understand
NWindow* win;
Framebuffer fb;

u32 stride;
u32* framebuf;

int main(int argc, char* argv[])
{
    win = nwindowGetDefault();
	//I have no idea what I'm doing and copied this from the simplegfx example
	framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
	framebufferMakeLinear(&fb);	
	
	init_map();
	init_cells();
	
    while (appletMainLoop())
    {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS)
            break; 
		
		update_all_cells();
		draw_all_cells();
		swap_offset(); //flip array
		
    }

	framebufferClose(&fb);
	
    return 0;
}

void update_all_cells()
{
	for (int y = 0; y < map_height; y++)
	{
		for (int x = 0; x < map_width; x++)
		{
			int s = sum_cells_in_area(x, y);
			
			if (s == 3)
				set_pos(x, y, 1);
			else if (s == 2)
				set_pos(x, y, get_pos(x, y));
			else
				set_pos(x, y, 0);
		}
	} 
}

//initialize map
void init_map()
{
	map_width  = 192;
	map_height = 128;
	map_scale  = 2;
	map_offset = 0;
}

//initializes cells randomly, at the given map size
//requires initialized map before use.
void init_cells()
{
	for (int y = 0; y < map_height; y++)
	{
		for (int x = 0; x < map_width; x++)
		{
			set_pos(x, y, rand() % 2);
			swap_offset();
			set_pos(x, y, get_pos(x, y));
			swap_offset();
		}
	}
}

//Returns the sum of cells within the area around the current cell.
//NOTE: checks for validity of edges, but not the provided location.
int sum_cells_in_area(int x, int y)
{
	int sum = 0;
	
	for (int ofs_x = -1; ofs_x <= 1; ofs_x++)
	{
		for (int ofs_y = -1; ofs_y <= 1; ofs_y++)
		{
			if (x + ofs_x > 0 && x + ofs_x < map_width
			 && y + ofs_y > 0 && y + ofs_y < map_height)
				if (get_pos(x + ofs_x, y + ofs_y) > 0 && !(ofs_x == 0 && ofs_y == 0))
					sum++;
		}
	} 
	
	return sum;
}

//draws the cells to the screen
//some variables here could probably be moved to/from this block.
void draw_all_cells()
{
	framebuf = (u32*) framebufferBegin(&fb, &stride);
	
	for (int y = 0; y < map_height; y++)
	{
		for (int x = 0; x < map_width; x++)
		{
			set_pixel(x, y, get_pos(x, y) * 0xFFFFFFFF);
		}
	}
	
	framebufferEnd(&fb);
}

void swap_offset()
{
	map_offset = (map_offset == 0) ? MAX_MAP_SIZE : 0;
}

//sets a pixel in the framebuffer.
//make sure to initialize framebuffer first...
void set_pixel(int x, int y, int rgba8888)
{
	u32 pos = y * stride / sizeof(u32) + x;	
	
	framebuf[pos] = rgba8888;
}

//sets the cell at the given position.
void set_pos(int x, int y, int value)
{
	cells[y * MAX_WIDTH + x + map_offset] = value; 
}

//get cell at position.
int get_pos(int x, int y)
{
	return cells[y * MAX_WIDTH + x + (MAX_MAP_SIZE - map_offset)];
}