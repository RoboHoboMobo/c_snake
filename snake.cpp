// Snake structure code
#include "game.h"

void Snake_init(Snake* s) // Snake struct initialization
{
    s->size = 1;
    Snake_chunk *h = new Snake_chunk(); // head chunk
    h->x = FIELD_SIZE/2;
    h->y = FIELD_SIZE/2;
    h->dir = RIGHT;
    h->next = NULL;

    s->head = h;
    s->tail = h;
}

bool Snake_add(Snake* s) // Add new chunk to Snake
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

    if(sc->x<0 || sc->y<0)  // If tail out of borders
    {                       // then don't add new chunk
        delete sc;
        return false;
    }

    s->size++;
    s->tail->next = sc;
    s->tail = sc;

    return true;
}

Snake_chunk* Snake_at(Snake* s, unsigned index) // Get chunk by index
{
    if(!s)
        return NULL;

    if(s->size == 0)
    {
        Snake_init(s);
        return s->head;
    }

    if(s->size == 1 || index == 0)
        return s->head;

    if(index >= s->size)            // Index exceeding check
        index = s->size - 1;

    Snake_chunk* sc = s->head;

    for(unsigned i=0; i<index; i++)
        sc = sc->next;

    return sc;
}
