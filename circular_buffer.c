#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "circular_buffer.h"

// Reference: https://stackoverflow.com/questions/827691/how-do-you-implement-a-circular-buffer-in-c
// initialize the circular buffer
void buffer_init(circular_buffer *cb, int bufferSize)
{
    cb->buffer = malloc(sizeof(double) * (bufferSize + 1));
    if (cb->buffer == NULL)
    {
        printf("the buffer is NULL\n");
    }

    // head and tail are initialized as index 0
    cb->head = 0;
    cb->tail = 0;
    cb->size = bufferSize;
    cb->count = 0;
    return;
}

// clean out the buffer
void buffer_free(circular_buffer *cb)
{
    free(cb->buffer);
    // cb->head = NULL;
    // cb->tail = NULL;
    return;
}

// add data to the light sample buffer
void buffer_AddData(circular_buffer *cb, double item)
{
    // if buffer has looped
    if (cb->count % cb->size == 0)
    {
        // reset the head index to 0
        cb->head = 0;
    }
    
    cb->buffer[cb->head] = item;
    cb->head = (cb->head + 1) % (cb->size); 
    cb->count++;
    return;
}