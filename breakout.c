//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

//height and width of a tile
#define TILE_HEIGHT 10
#define TILE_WIDTH 35.6

//height and width of paddle
#define PADDLE_HEIGHT 8
#define PADDLE_WIDTH 40

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
GLabel initLifeboard(GWindow window, int lives);
void updateLifeboard(GWindow window, GLabel label, int lives);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));
    
    // initialize ball's velocity
    double vx = drand48();
    double vy = -4.0 * drand48();
    
    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);
    
    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);
    
    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

	// instantiate lifeboard, on the window's down-right corner
    GLabel life_board = initLifeboard(window, lives);

    // number of points initially
    int points = 0;
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {	
		
		waitForClick();
			
		// forever
		while (true)
		{	
			move( ball, vx, vy );
			GEvent event = getNextEvent(MOUSE_EVENT);
				
			if (event != NULL)
			{
				if (getEventType(event) == MOUSE_MOVED)
				{
					// set paddle coordinares
					double x = getX(event) - getWidth(paddle) / 2;
					double y = HEIGHT - PADDLE_HEIGHT - getHeight(ball);
						
					// checking if paddle goes out of the window's limits
					if (x < 0)
						x = 0;
					else if (x >= (WIDTH - PADDLE_WIDTH))
						x = WIDTH - PADDLE_WIDTH;	
						
					// update paddle location on window
					setLocation(paddle, x, y);
				}
			}
			
			//bouncing
			if (getX(ball) + getWidth(ball) >= getWidth(window) || (getX(ball) <= 0 ))
			{
				vx = -vx;
				pause(10);
			}
			if (getY(ball) <= 0 )
			{
				vy = -vy;
				pause(10);
			}
			if (getY(ball) > getY(paddle) ) // if the ball hits the bottom edge
			{
				lives--;
				updateLifeboard(window, life_board, lives);
				setLocation(ball, WIDTH/2-2, HEIGHT/2-2);
				waitForClick();
				break;
			}
				
				
						
			// checking for collition
			GObject object = detectCollision( window, ball );
					
			if (object != NULL )
			{
				// if the stricken object is a GRect object 
				if (strcmp(getType(object), "GRect") == 0)
				{
					// if it's the paddle
					if (object == paddle)
					{
						// change the ball's direction
						vy = -vy;
						pause(10);
					}
							
					// if it's a brick
					else
					{
						// add point
						points ++;
						updateScoreboard(window, label, points);
								
						// reduce existing bricks
						bricks --;
						vy = -vy;
						vx = -vx;
						pause(10);
															
						// remove brick
						removeGWindow( window, object);
					}
				}
			} // non_NULL objects
			pause(5);
		} // forever loop
	} // lives & bricks
	
	// ball on the center when game's over
	initBall(window);
	
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // TODO
    //initialize the beginning coordinares
    float space = 4;
    float x = 0;
    float y = 0;
    char *color = " ";
    for (int i = 0; i < ROWS; i++ )
    {
		//change the color for each row
		switch ( i ) {
			case 0:
				y = 20;
				color = "RED";
				break;
			case 1:
				color = "ORANGE";				
				break;
			case 2:
				color = "YELLOW";
				break;
			case 3:
				color = "GREEN";
				break;
			case 4:
				color = "BLUE";
				break;
			default:
				printf("Something went wrong with the number of rows.");
				break;
		}
		
			
		for ( int j = 0; j < COLS; j++ )
		{
			if (j == 0)
				x = 4;
			else
				x = x + space + TILE_WIDTH;
				
			GRect tile = newGRect(x, y, TILE_WIDTH, TILE_HEIGHT);
			setFilled(tile, true);
			setColor(tile, color);
			add(window, tile);
		}
		y = y + space + TILE_HEIGHT;
	}
			
    
    
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // TODO
    
    //designing the ball and setting it in place
	float radius = 2;
    GOval ball = newGOval(WIDTH/2 - radius, HEIGHT/2 - radius, 12, 12);
    
    // colouring the ball
    setFilled(ball, true);
    setColor(ball, "BLACK");
    add(window, ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // TODO
    //calculate paddle's coordinares
    double paddle_x = WIDTH/2 - PADDLE_WIDTH/2;
    double paddle_y = HEIGHT - PADDLE_HEIGHT;
    	    
    //create and place the paddle
    GRect paddle = newGRect(paddle_x, paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT);
    
    //color the paddle
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    add(window, paddle);
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // TODO
    double x, y;
    GLabel scoreboard = newGLabel("0");
    
    setFont(scoreboard, "SansSerif-25");
    x = (getWidth(window) - getWidth(scoreboard)) / 2;
    y = (getHeight(window) + getFontAscent(scoreboard)) / 2;
    setLocation(scoreboard, x, y);
    add(window, scoreboard);
    return scoreboard;
}

/**
 * Instantiates, configures, and returns label for lifeboard.
 */
 GLabel initLifeboard(GWindow window, int lives)
 {
	 double x, y;
	 char str[12];
	 sprintf( str, "%i", lives );
	 GLabel lifeboard = newGLabel(str);
	 
	 setFont(lifeboard, "SansSerif-18");
	 x = getWidth(window) - 20.0 ;
	 y = getHeight(window) - 10.0;
	 setColor(lifeboard, "RED");
	 setLocation(lifeboard, x, y);
	 add(window, lifeboard);
	 return lifeboard;
}

/**
 * Updates lifeboard
 */
void updateLifeboard(GWindow window, GLabel label, int lives)
{
	char s[12];
	sprintf(s, "%i", lives);
	setLabel(label, s);
	
	// alocate space for label on the window
	double x = getWidth(window) - 10.0 ;
	double y = getHeight(window) - 10.0;
	setLocation(label, x, y);	
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
