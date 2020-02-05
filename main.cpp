/// Snake in C-style challange
#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <cstdlib>

#define VISUAL_RATE 60  // Hz
#define GAME_RATE   4 // Hz
#define FIELD_SIZE  12  // a x a
#define FOOD_CHANCE 15
#define SNAKE_INIT  6

enum FIELD_OBJECTS
{
  FIELD,
  HEAD,
  TAIL_HORISONTAL,
  TAIL_VERTICAL,
  FOOD
};

enum GRAPHICS
{
    G_HEAD       = 111,
    G_HORIZONTAL = 95,  // 95
    G_VERTICAL   = 124,
    G_FIELD      = 46,
    G_FOOD       = 42
};

enum DIRECTION
{
    UP = 65, // Key codes in Curses
    DOWN,
    RIGHT,
    LEFT
};

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

void Snake_init(Snake* s)
{
    s->size = 1;
    Snake_chunk *h = new Snake_chunk();
    h->x = FIELD_SIZE/2;
    h->y = FIELD_SIZE/2;
    h->dir = RIGHT;
    h->next = NULL;

    s->head = h;
    s->tail = h;
}

bool Snake_add(Snake* s)
{
    Snake_chunk *sc = new Snake_chunk();
    sc->next = NULL;

    switch (s->tail->dir)
    {
    case RIGHT:
        sc->dir = RIGHT;
        sc->x = s->tail->x - 1;
        sc->y = s->tail->y;
        break;
    case LEFT:
        sc->dir = LEFT;
        sc->x = s->tail->x + 1;
        sc->y = s->tail->y;
        break;
    case UP:
        sc->dir = UP;
        sc->x = s->tail->x;
        sc->y = s->tail->y + 1;
        break;
    case DOWN:
        sc->dir = DOWN;
        sc->x = s->tail->x;
        sc->y = s->tail->y - 1;
        break;
    default:
        break;
    }

    if(sc->x<0 || sc->y<0)  // if tail out of borders
    {                       // then don't add new chunk
        delete sc;
        return false;
    }

    s->size++;
    s->tail->next = sc;
    s->tail = sc;

    return true;
}

Snake_chunk* Snake_at(Snake* s, unsigned index)
{
    if(!s)
        return NULL;

    if(s->size == 0)
    {
        Snake_init(s);
        return s->head;
    }

    if(s->size == 1 || index == 0)  //
        return s->head;

    if(index >= s->size)            // Index exceeding check
        index = s->size - 1;

    Snake_chunk* sc = s->head;

    for(unsigned i=0; i<index; i++)
        sc = sc->next;

    return sc;
}


void play();                                 // Basic function
void field(int field[][FIELD_SIZE]);         // Fill background
bool food(Snake* s, Food* fd);         // Get food coordinates
bool snake_controller(Snake *s, int input);  // Drive snake according to input
void join(int f[][FIELD_SIZE], Snake* s);    // Join field and snake
void join_food(int f[][FIELD_SIZE], Food *fd);
void draw(int f[][FIELD_SIZE]);              // Draw game's window
bool eat(Snake* s, int x, int y);

bool snake_controller(Snake *s, int input)
{
    if(input==UP || input==DOWN || input==RIGHT || input==LEFT) // Rewrite direction of chunks according to input
    {
        for(unsigned i=0; i<s->size; i++)
        {
          int temp_dir = Snake_at(s, i)->dir;
          Snake_at(s, i)->dir = input;
          input = temp_dir;
        }
    }
    else                                                        // Just update direction of chunks
    {
        input = s->head->dir;
        for(unsigned i=0; i<s->size; i++)
        {
          int temp_dir = Snake_at(s, i)->dir;
          Snake_at(s, i)->dir = input;
          input = temp_dir;
        }
    }

    bool result = true;
    for(unsigned i=0; i<s->size; i++)                           // Rewrite chunks' coordinates according to direction
    {
        int cur_dir = Snake_at(s, i)->dir;
        Snake_chunk* cur_chunk = Snake_at(s, i);

        switch (cur_dir)
        {
        case RIGHT:
            cur_chunk->x += 1;
            if(cur_chunk->x >= FIELD_SIZE)
                result = false;
            break;
        case LEFT:
            cur_chunk->x -= 1;
            if(cur_chunk->x < 0)
                result = false;
            break;
        case UP:
            cur_chunk->y -= 1;
            if(cur_chunk->y < 0)
                result = false;
            break;
        case DOWN:
            cur_chunk->y += 1;
            if(cur_chunk->y >= FIELD_SIZE)
                result = false;
            break;
        default:
            break;
        }
    }

    for(unsigned i=1; i<s->size; i++)           // Self-crossing's check
    {
        if(s->head->x == Snake_at(s,i)->x &&
           s->head->y == Snake_at(s,i)->y)
            return false;

    }

    return result;
}

void field(int f[][FIELD_SIZE])
{
    for(int i=0; i<FIELD_SIZE; i++)
        for(int j=0; j<FIELD_SIZE; j++)
            f[i][j] = FIELD;
}

bool food(Snake* s, Food *fd)
{
    srand(time(NULL));
    int chance = rand() % 101;

    if(chance < 100-FOOD_CHANCE)
        return false;

    int occup_x[s->size];
    int occup_y[s->size];

    for(unsigned i=0; i<s->size; i++)
    {
        occup_x[i] = Snake_at(s, i)->x;
        occup_y[i] = Snake_at(s, i)->y;
    }

    bool in_loop = true;
    while(in_loop)
    {
        fd->x  = (rand()+1) % FIELD_SIZE;

        for(unsigned i=0; i<s->size; i++)
        {
            if(fd->x != occup_x[i])
                in_loop = false;
            else
            {
                in_loop = true;
                break;
            }
        }
    }

    srand(time(NULL));
    in_loop = true;
    while(in_loop)
    {
        fd->y  = (rand()+2) % FIELD_SIZE;

        for(unsigned i=0; i<s->size; i++)
        {
            if(fd->y != occup_y[i])
                in_loop = false;
            else
            {
                in_loop = true;
                break;
            }
        }
    }

    return true;
}

bool eat(Snake* s, Food *fd)
{
    if(s->head->x==fd->x && s->head->y==fd->y)
    {
        Snake_add(s);
        return true;
    }

    return false;
}

void join(int f[][FIELD_SIZE], Snake *s)
{
    field(f);
    for(unsigned i=0; i<s->size; i++)
    {
        Snake_chunk *cur_ch = Snake_at(s, i);
        int x   = cur_ch->x;
        int y   = cur_ch->y;
        int dir = cur_ch->dir;

        if(i == 0)
            f[y][x] = HEAD;
        else
        {
            if(dir==LEFT || dir== RIGHT)
                f[y][x] = TAIL_HORISONTAL;
            else {
                f[y][x] = TAIL_VERTICAL;
            }
        }
    }
}

void join_food(int f[][FIELD_SIZE], Food *fd)
{
    f[fd->y][fd->x] = FOOD;
}

void draw(int f[][FIELD_SIZE])
{
    for(int i=0; i<FIELD_SIZE; i++)
        for(int j=0; j<FIELD_SIZE; j++)
        {
            switch (f[i][j]) {
            case HEAD:
                f[i][j] = G_HEAD;
                break;
            case TAIL_HORISONTAL:
                f[i][j] = G_HORIZONTAL;
                break;
            case TAIL_VERTICAL:
                f[i][j] = G_VERTICAL;
                break;
            case FOOD:
                f[i][j] = G_FOOD;
                break;
            default:
                f[i][j] = G_FIELD;
                break;
            }

        }

}

void play()
{
    int f[FIELD_SIZE][FIELD_SIZE];
    field(f);

    Snake sn;
    Snake_init(&sn);

    for(int i=0; i<SNAKE_INIT-1; i++)
        Snake_add(&sn);

    ulong period_t = 1.0/GAME_RATE * 1000000; // in us

/// NCurses block
    initscr(); // init curses mode
    clear();
    noecho();
    raw();     // raw mode for keyboard

    bool in_loop = true;
    unsigned score = 0;
    int ch_buff = 0;
    bool fd_on_fld = false;
    bool sn_ctrl   = true;

    struct timespec start_t, now_t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_t);

    while (in_loop)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &now_t);

        if((ulong)((now_t.tv_nsec - start_t.tv_nsec)/1000 + now_t.tv_sec - start_t.tv_sec) >= period_t
            && sn_ctrl)
        {
            start_t = now_t;
            sn_ctrl = snake_controller(&sn, ch_buff);
            join(f, &sn);

            Food fd;
            if(!fd_on_fld)
                fd_on_fld = food(&sn, &fd);

            if(eat(&sn, &fd))
            {
                join(f, &sn);
                fd_on_fld = false;
                score++;
            }

            if(fd_on_fld)
                join_food(f,&fd);

            draw(f);
            clear();

            printw("\n");
            mvwprintw(stdscr, 0, FIELD_SIZE-6, "ZMEY ver.0.1\n");

            for(int i=0; i<FIELD_SIZE; i++)
            {
                for(int j=0; j<FIELD_SIZE; j++)
                    printw("%c ",f[i][j]);

                printw("\n");
            }

            printw("\nScore: %d\n", score);
        }

        timeout(0);              // no break after key stroke
        int ch = wgetch(stdscr);
        if(ch > -1)
            ch_buff = ch;

        if(ch == KEY_ENTER || ch == 10 || !sn_ctrl)
            in_loop = false;

        refresh();
    }

    cbreak();
    noraw();
    timeout(-1);
    mvwprintw(stdscr, FIELD_SIZE/2-1, FIELD_SIZE-5, "Game over!");
    mvwprintw(stdscr, FIELD_SIZE/2, FIELD_SIZE-6, "Press ENTER");

    refresh();


    wgetch(stdscr);


    endwin(); // quit curses mode


}


int main()
{
    play();

    //struct timeval stop, start;
    //gettimeofday(&start, NULL);
    //
    //for(long i=0; i<1000000; i++)
    //    pow(i,2);
    //for(long i=0; i<1000000; i++)
    //    pow(i,2);
    //for(long i=0; i<1000000; i++)
    //    pow(i,2);
    //for(long i=0; i<1000000; i++)
    //    pow(i,2);
    //for(long i=0; i<1000000; i++)
    //    pow(i,2);
    //for(long i=0; i<1000000; i++)
    //    pow(i,2);
    //
    //
    //
    //gettimeofday(&stop, NULL);
    //std::cout<<(double)(stop.tv_usec - start.tv_usec)/1000000 + stop.tv_sec - start.tv_sec<<std::endl;

    return 0;
}
