#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#define FB_WIDTH  1280
#define FB_HEIGHT 720

#define MAP_WIDTH 120
#define MAP_HEIGHT 120

void init_cells();
void update_all_cells();
void draw_all_cells();

int sum_cells_in_area(int x, int y);
int get_pos(int x, int y);

int old_cells[MAP_WIDTH * MAP_HEIGHT];
int cells[MAP_WIDTH * MAP_HEIGHT];

NWindow* win;
Framebuffer fb;

int main(int argc, char* argv[])
{
    win = nwindowGetDefault();
	//I have no idea what I'm doing and copied this from the simplegfx example
	framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
	framebufferMakeLinear(&fb);	

	init_cells();
	
    while (appletMainLoop())
    {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS)
            break; 

		memcpy(old_cells, cells, sizeof(cells));
		update_all_cells();
		draw_all_cells();

    }

	framebufferClose(&fb);
	
    return 0;
}

void update_all_cells()
{
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			int s = sum_cells_in_area(x, y);
			
			if (s == 3)
				cells[y * MAP_WIDTH + x] = 1;
			else if (s == 2)
				cells[y * MAP_WIDTH + x] = get_pos(x, y);
			else
				cells[y * MAP_WIDTH + x] = 0;
		}
	} 
}

void init_cells()
{
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			if (rand() % 2 == 0)
				cells[y * MAP_WIDTH + x] = 1;
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
			if (x + ofs_x > 0 && x + ofs_x < MAP_WIDTH
			 && y + ofs_y > 0 && y + ofs_y < MAP_HEIGHT)
				if (get_pos(x + ofs_x, y + ofs_y) > 0 && !(ofs_x == 0 && ofs_y == 0))
					sum++;
		}
	} 
	
	return sum;
}

void draw_all_cells()
{
	u32 stride;
	u32* framebuf = (u32*) framebufferBegin(&fb, &stride);
	
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			u32 pos = y * stride / sizeof(u32) + x;	
			
			if (get_pos(x, y) > 0)
				framebuf[pos] = 0xFFFFFFFF;
			else
				framebuf[pos] = 0x00000000;
		}
	}
	
	framebufferEnd(&fb);
}

//convert xy to one-dimensional array location in cells[]
int get_pos(int x, int y)
{
	return old_cells[y * MAP_WIDTH + x];
}