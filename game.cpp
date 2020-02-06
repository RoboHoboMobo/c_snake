// Game functions code
#include "game.h"

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

    int occup_x[s->size];                 // Coordinates of occupied cells by Snake
    int occup_y[s->size];

    for(unsigned i=0; i<s->size; i++)
    {
        occup_x[i] = Snake_at(s, i)->x;
        occup_y[i] = Snake_at(s, i)->y;
    }

    bool in_loop = true;
    while(in_loop)
    {
        fd->x  = rand() % FIELD_SIZE; // Get coordinate by random()

        for(unsigned i=0; i<s->size; i++)
        {
            if(fd->x != occup_x[i])       // Break loop if not occupaed
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
        fd->y  = rand() % FIELD_SIZE;

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
    int f[FIELD_SIZE][FIELD_SIZE];         // Field declaration
    field(f);                              // Define Field

    Snake sn;
    Snake_init(&sn);

    for(int i=0; i<SNAKE_INIT-1; i++)      // Add chunks according to Snake length
        Snake_add(&sn);

    ulong period_t = 1.0/GAME_RATE * 1000; // Time period in ms

/// NCurses block
    initscr();                             // Init curses mode
    clear();
    noecho();                              // Don't display input characters
    raw();                                 // Raw mode for keyboard

    bool in_loop = true;
    unsigned score = 0;                    // Game scores
    int ch_buff = 0;                       // Input caracter buffer
    bool fd_on_fld = false;                // Food on field flag
    bool sn_ctrl   = true;                 // Aux flag for main loop

    struct timespec start_t, now_t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_t);

    while (in_loop)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &now_t);

        if((ulong)((now_t.tv_nsec - start_t.tv_nsec)/1000000 +
                   (now_t.tv_sec - start_t.tv_sec)*1000) >= period_t &&
                   sn_ctrl)
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
            mvwprintw(stdscr, 0, FIELD_SIZE-6, "ZMEY ver.0.2\n"); // Field header

            for(int i=0; i<FIELD_SIZE; i++)                       // Game field
            {
                for(int j=0; j<FIELD_SIZE; j++)
                    printw("%c ",f[i][j]);

                printw("\n");
            }

            printw("\nScore: %d\n", score);
        }

        timeout(0);                                               // No break after key stroke
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
