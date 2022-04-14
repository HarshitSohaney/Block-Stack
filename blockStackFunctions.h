/* Header files for Block Stack game 
	create by Harshit Sohaney & Yunjia Hao
*/

/* clear screen to background */
void clear_screen();
/* count down the VGA to synchronize buffers */
void wait_for_vsync();
/* draw boxes */
void draw_boxes(int x1, int x2, int y1, int erase);
/* display game over screen */
void display_game_over();
/* display home screen */
void display_home();
/* polling to start game */
void start_game();