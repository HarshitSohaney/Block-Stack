#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "numbers.h"
#include "backgrounds.h"
#include "blockStackFunctions.h"

#define ABS(x) (((x) > 0) ? (x) : -(x))
#define FPGA_CHAR_BASE        0xC9000000

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
int BOX_LEN = 40;
#define BOX_H 40
#define MAX_BOXES 5
#define FALSE 0
#define TRUE 1

volatile int pixel_buffer_start; // global variable
const int main_background[240][320];
const int background2[240][320];
const int gameOver_background[240][320];
const int brick[40][40];
const int home_page[240][320];

int zero[20][20];
int ones[20][20];
int two[20][20];
int three[20][20];
int four[20][20];
int five[20][20];
int six[20][20];
int seven[20][20];
int eight[20][20];
int nine[20][20];
int scoreboard[17][50];
int scores[2] = {0};

int num_boxes = 1;
void clear_screen();
void wait_for_vsync();
void draw_boxes(int x1, int x2, int y1, int erase);
void display_game_over();
void display_home();
int frequency = 60;
int firstRound = 1; 
	
int main(void){
	int amount_cut = 0;
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
  	volatile int * ps2_ptr = (int *)0xFF200100;
	int boxThreshold = 2;
	int ps2Input, ps2Valid, ps2Key, done = 0;

    // Declare variables
	int x_pos[100], x_pos2[MAX_BOXES] = {0}, y_pos[100]= {0};
	int y_old[MAX_BOXES] = {0}, y_old2[MAX_BOXES] = {0}; 
	int x_old[MAX_BOXES] = {0}, x_old2[MAX_BOXES] = {0};
	//srand(time(NULL)); 

	/* set front pixel buffer to start of FPGA On-chip memory */
	*(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
	// back buffer
	/* now, swap the front/back buffers, to set the front buffer location */
	wait_for_vsync();
	/* initialize a pointer to the pixel buffer, used by drawing functions */
	pixel_buffer_start = *pixel_ctrl_ptr;
	display_home(); 
	/* set back pixel buffer to start of SDRAM memory */
	*(pixel_ctrl_ptr + 1) = 0xC0000000;
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	display_home();
	
	int i = 0;
	int last_line = RESOLUTION_Y - BOX_H;
	bool secondFirstRound = false;
	int start = 0;
	do{
		ps2Input = *ps2_ptr;
		ps2Key = ps2Input & 0xFF;
	}while(ps2Key != 0x29);

	wait_for_vsync();
	pixel_buffer_start = *pixel_ctrl_ptr;
	clear_screen(); 
	/* set back pixel buffer to start of SDRAM memory */
	*(pixel_ctrl_ptr + 1) = 0xC0000000;
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen();
	bool firstIt = true;
	int score = 0; 
	while(!done){
		//draws falling box 
		if(i==0)
		   x_pos[0] = rand() % (RESOLUTION_X - BOX_LEN);
		else if(firstIt)
			x_pos[i] = rand() % (x_pos[i-1] + BOX_LEN + 75 + 1 - (x_pos[i-1] - 75)) +  x_pos[i-1] - 75 ;
		
	  do{
		//erase box drawn at last iteration
		   for(int j = i; j < num_boxes + i; j++) {
				draw_boxes(x_pos[j], x_pos[j] + BOX_LEN, y_old2[j], TRUE);
				draw_boxes(x_old2[j], x_old2[j] + BOX_LEN, y_old2[j], TRUE);
				draw_boxes(x_old[j], x_old[j] + BOX_LEN, y_old[j], TRUE);
		   }
		//draw box
		 for(int j = i; j < num_boxes + i; j++)
			 if(y_pos[j-1] > y_pos[j-2] - frequency)
				draw_boxes(x_pos[j], x_pos[j] + BOX_LEN, y_pos[j], FALSE);
		
		//code for updating location of box
		for(int j = i; j < num_boxes+i; j++) {
			y_old2[j] = y_old[j];
			y_old[j] = y_pos[j];	  

			ps2Input = *ps2_ptr;
			ps2Key = ps2Input & 0xFF;

			if(ps2Input & 0x8000 == 0x8000) { // Check 15th bit for validity
				x_old2[j] = x_old[j];
				x_old[j] = x_pos[j];
				//x_pos2[i] = x_pos[i] + BOX_LEN;
				if (ps2Key == 0x6B && x_pos[j] - 1 > 0) {  // Left Arrow
						  x_pos[j] = x_pos[j] - 2;
							ps2Key = 0;
				}

				if (ps2Key == 0x75 && x_pos[i] + 1 < RESOLUTION_X) {// Right arrow
							x_pos[j] = x_pos[j] + 2;
							ps2Key = 0;
				}
				
				int RVAL = ps2Input & 0xFFFF0000;
				while(RVAL > 0) {
					ps2Input = *(ps2_ptr);
					RVAL = ps2Input & 0xFFFF0000;
				}
					
			}

			//update locations
			if(y_pos[j-1] > y_pos[j-2] - frequency)
				y_pos[j] = y_pos[j] + 1;
		}
		  wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		  wait_for_vsync();
		  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	} while(y_pos[i] < last_line);
	//we have reached the largest y-distance
		
	if(x_pos2[i] == 0)
		x_pos2[i] = BOX_LEN + x_pos[i]; 
	if(i > 0){	
		if(x_pos[i] + BOX_LEN < x_pos[i-1] || x_pos[i] > x_pos2[i-1]){ //out of bounds 
			done = 1;
		}
		else if ( (x_pos[i] < x_pos[i-1]) && (x_pos[i] + BOX_LEN > x_pos2[i-1])) {
			//draw_boxes(x_pos[i], x_pos2[i], y_pos[i], TRUE);
			x_pos[i] = x_pos[i-1];
			x_pos2[i] = x_pos2[i-1];
			amount_cut = x_pos[i-1] - x_pos[i] + x_pos[i] + BOX_LEN - x_pos2[i-1];
		}
		else if(x_pos[i] < x_pos[i-1]){
			//if leftmost x is less than leftmost x of prev iteration
			//draw_boxes(x_pos[i], x_pos2[i], y_pos[i], TRUE);
			x_pos2[i] = x_pos[i] + BOX_LEN;
			x_pos[i] = x_pos[i-1];
			amount_cut = x_pos[i-1] - x_pos[i];
		}
		else if(x_pos[i] + BOX_LEN > x_pos2[i-1]){
			//if rightmost x is more than rightmost x of prev iteration
			//draw_boxes(x_pos[i], x_pos2[i], y_pos[i], TRUE);
			x_pos2[i] = x_pos2[i-1];	
			amount_cut = x_pos[i] + BOX_LEN -  x_pos2[i-1];			
			//x_pos is already set at correct location 
		}
	}

	x_old2[i] = x_old[i];
	x_old[i] = x_pos[i];
	if(!done){ //redraw box with part cut off 
		clear_screen();
		for(int j = 0; j < i+1; j++){
				draw_boxes(x_pos[j], x_pos2[j], y_pos[j], FALSE);			
		}
		wait_for_vsync(); 
		pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	  
		BOX_LEN = BOX_LEN - amount_cut;
		amount_cut = 0;		
	}
	int numBoxes = MAX_BOXES - 2;
	//redraws boxes and shifts everything up if reaches 50% of display
		score++;
		if(score >= 10){
			scores[1] = score % 10; 
			scores[0] = score/10;
		}
		else{ 
			scores[1] = score; 
			scores[0] = 0; 
		}
		for(int num = 0; num < 2; num++){
			//this prints the number
			int shift = RESOLUTION_X - 40 + num * 20; 
			for(int i = 0; i < 20; i++){
				for(int j = 0; j < 20; j++){
					if(scores[num] == 0)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = zero[j][i];
					else if(scores[num] == 1)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = ones[j][i];
					else if(scores[num] == 2)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = two[j][i];
					else if(scores[num] == 3)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = three[j][i];					
					else if(scores[num] == 4)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = four[j][i];
					else if(scores[num] == 5)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = five[j][i];
					else if(scores[num] == 6)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = six[j][i];
					else if(scores[num] == 7)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = seven[j][i];
					else if(scores[num] == 8)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = eight[j][i];
					else if(scores[num] == 9)
						*(short int *)(pixel_buffer_start + ((j + 10) << 10) + ((i + shift) << 1)) = nine[j][i];
				
				}
			}
		}
	if(i == numBoxes){
			//reset starting position
		if(!firstRound) 
			secondFirstRound = true;
		 	
		firstIt = false;
			if(frequency < 100)
				frequency = frequency + 10;
			
		 
		x_pos[0] = x_pos[numBoxes-1];
		x_pos2[0] = x_pos2[numBoxes-1];
		y_pos[0] = RESOLUTION_Y - BOX_H; 
		x_pos[1] = x_pos[numBoxes];
		x_pos2[1] = x_pos2[numBoxes];
		y_pos[1] = y_pos[0] - BOX_H; 
		boxThreshold = 2;
		if(!firstRound) {
			boxThreshold = 3;
			x_pos[2] = x_pos[numBoxes+1];
			x_pos2[2] = x_pos2[numBoxes+1];
			y_pos[2] = y_pos[1] - BOX_H - 40;    
		}

		for(int j = boxThreshold; j < MAX_BOXES; j++){
			x_pos[j] = rand() % (x_pos[0] + BOX_LEN + 40 + 1 - (x_pos[0] - 40))  +  x_pos[0] - 40;
			x_pos2[j] = 0; 
			y_pos[j] = 0; 
		}
		firstRound = 0;
		if(!firstRound) 
			score--;
		i = 0; 
		   num_boxes = 2;
		last_line = RESOLUTION_Y - BOX_H; 
		clear_screen();
		draw_boxes(x_pos[0], x_pos2[0], y_pos[0], FALSE);			
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	}
		i++;
		last_line = y_pos[i-1] - BOX_H;
		if(done){
			firstRound = 1; 
			display_game_over(); 
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
			pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			firstIt = true;
			BOX_LEN = 40;
			frequency = 60;
			//take in R for reset
			while(done){
				ps2Input = *ps2_ptr;
				ps2Key = ps2Input & 0xFF;
				
				if(ps2Input & 0x8000 == 0x8000 && ps2Key == 0x2D) { // Check 15th bit for validity
					done = 0;
					for(int j = 0; j < 100; j++){
						x_pos[j] = rand() % (x_pos[j-1] + BOX_LEN + 50 + 1 - x_pos[j-1] - 50) + x_pos[j-1] - 50;
						x_pos2[j] = 0; 
						y_pos[j] = 0; 
					}
					last_line = RESOLUTION_Y - BOX_H; 
					i = 0; 
					num_boxes = 1;
					score = 0 ; 
					clear_screen();
					wait_for_vsync(); // swap front and back buffers on VGA vertical sync
					pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
					clear_screen();
					
					wait_for_vsync(); // swap front and back buffers on VGA vertical sync
					pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
				}
			}	
		}
	}
}
void wait_for_vsync() {
	volatile int * pixel_ctrl_ptr = (int *) 0xFF203020; // pixel controller
	register int status;

	*pixel_ctrl_ptr = 1; // start the synchronization process

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0) {
    	status = *(pixel_ctrl_ptr + 3);
	}
}
void display_home(){
	for(int x = 0; x < RESOLUTION_X; x++){
        for(int y = 0; y < RESOLUTION_Y; y++){
    		*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = home_page[y][x];
        }
    }
}
void display_game_over(){
	for(int x = 0; x < RESOLUTION_X; x++){
        for(int y = 0; y < RESOLUTION_Y; y++){
    		*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = gameOver_background[y][x];
        }
    }
}
//Clear screen
void clear_screen(){
    if(firstRound){
		for(int x = 0; x < RESOLUTION_X; x++){
        	for(int y = 0; y < RESOLUTION_Y; y++){
				*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = main_background[y][x];
        	}
    	}
	}
	else{
		for(int x = 0; x < RESOLUTION_X; x++){
        	for(int y = 0; y < RESOLUTION_Y; y++){
				*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = background2[y][x];
        	}
    	}
	}
}

void draw_boxes(int x1, int x2, int y1, int erase){
	if(firstRound){
		for(int x = x1; x <= x2; x++){
			for(int y = y1; y < y1 + BOX_H; y++){
				if(erase)
					*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = main_background[y][x];
				else
					*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = brick[y-y1][x-x1];
			}
		}
	}
	else{
		for(int x = x1; x <= x2; x++){
			for(int y = y1; y < y1 + BOX_H; y++){
				if(erase)
					*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = background2[y][x];
				else
					*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = brick[y-y1][x-x1];
			}
		}
	}
}
