#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <cstdlib>      // rand()

#define VISUAL_RATE 60  // Hz
#define GAME_RATE   4   // Hz
#define FIELD_SIZE  12  // a x a
#define FOOD_CHANCE 15  // Food appearance probability
#define SNAKE_INIT  6   // Snake's length

// Internal representation
enum FIELD_OBJECTS
{
  FIELD,
  HEAD,
  TAIL_HORISONTAL,
  TAIL_VERTICAL,
  FOOD
};

// Grahics' characters
enum GRAPHICS
{
    G_HEAD       = 111,
    G_HORIZONTAL = 95,  // 95
    G_VERTICAL   = 124,
    G_FIELD      = 46,
    G_FOOD       = 42
};

// Snake's directions
enum DIRECTION
{
    UP = 65, // Key codes in Curses
    DOWN,
    RIGHT,
    LEFT
};

// Linked list-style stuct
typedef struct Snake_chunk
{
    int x;
    int y;
    int dir;
    Snake_chunk* next;

} Snake_chunk;

typedef struct Snake
{
    unsigned size;
    Snake_chunk* head;
    Snake_chunk* tail;

} Snake;

typedef struct Food
{
    int x;
    int y;

} Food;

// Snake struct functions
void Snake_init(Snake* s);
bool Snake_add(Snake* s);
Snake_chunk* Snake_at(Snake* s, unsigned index);
void Snake_pop(Snake* s);                        // Remove last chunk
void Snake_delete(Snake* s);                     // Remove all chunks

// Game's functions
void play();                                   // Basic function
void field(int field[][FIELD_SIZE]);           // Fill background
bool food(Snake* s, Food* fd);                 // Make food
bool snake_controller(Snake *s, int input);    // Drive snake according to input
void join(int f[][FIELD_SIZE], Snake* s);      // Join field and snake maps
void join_food(int f[][FIELD_SIZE], Food *fd); // Join field and food maps
void draw(int f[][FIELD_SIZE]);                // Convert internal representation into graphics
bool eat(Snake* s, int x, int y);              // Remove food & increase snake




#endif // GAME_H
