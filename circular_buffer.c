#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "circular_buffer.h"

// Reference: https://stackoverflow.com/questions/827691/how-do-you-implement-a-circular-buffer-in-c
// initialize the circular buffer
void buffer_init(circular_buffer *cb, int bufferSize, size_t sz)
{
    cb->buffer = malloc(bufferSize * sz);
    if (cb->buffer == NULL)
    {
        printf("the buffer is NULL\n");
    }

    cb->head = cb->buffer;
    cb->tail = cb->tail;
    cb->buffer_end = (char *)cb->buffer + bufferSize * sz;
    cb->length = bufferSize;
    cb->sz = sz;
    cb->count = 0;
    cb->full = false;
    return;
}

// clean out the buffer
void buffer_free(circular_buffer *cb)
{
    free(cb->buffer);
    cb->head = NULL;
    cb->tail = NULL;
    return;
}

// add data to the light sample buffer
void buffer_AddData(circular_buffer *cb, void *item)
{
    // if buffer is full
    if (cb->count == cb->length)
    {
        // reset the head pointer
        cb->head = cb->buffer;
    }
    // copy the new item in the head buffer
    memcpy(cb->head, item, cb->sz);
    // printf("data: %d\n", *cb->head);
    cb->head = (char *)cb->head + cb->sz;
    if (cb->head == cb->buffer_end)
        cb->head = cb->buffer;
    cb->count++;
    return;
}