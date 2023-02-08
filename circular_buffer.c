#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "circular_buffer.h"

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
    if (cb->head % cb->size == 0)
    {
        // reset the head index to 0
        cb->head = 0;
    }

    cb->buffer[cb->head] = item;
    cb->head = (cb->head + 1) % (cb->size);
    cb->count++;
    return;
}

void buffer_resize(circular_buffer *cb, int length)
{
    double *tempBuffer = malloc(sizeof(double) * (length + 1));
    cb->head = cb->size;
    cb->tail = cb->size;
    // if then new size > the original size
    if (length >= cb->size) // length 5 cbsize 3
    {
        int numOfSamples = cb->size;
        if (numOfSamples >= cb->count)
        {
            numOfSamples = cb->count;
        }

        for (int i = 0; i < numOfSamples; i++)
        {
            tempBuffer[i] = cb->buffer[i];
        }
    }
    // if the new size <= the original size
    else
    {
        for (int i = 0; i < length; i++)
        {
            tempBuffer[i] = cb->buffer[i + cb->size - length];
        }
        cb->head = 0;
        cb->tail = 0;
    }
    cb->size = length;

    // Copy the new buffer data to the circular buffer struct
    free(cb->buffer);
    cb->buffer = tempBuffer;
}

int getNumSamples(circular_buffer *cb)
{
    int num = 0;
    for (int i = 0; i < cb->size; i++)
    {
        if (cb->buffer[i] != 0)
        {
            num++;
        }
    }
    return num;
}

void buffer_printData(circular_buffer *cb)
{
    for (int i = 0; i < cb->size; i++)
    {
        printf("%1.f ", cb->buffer[i]);
    }
    // printf("\nhead = %d, count = %d, numSamples = %d\n", cb->head, cb->count, getNumSamples(cb));
}

// int main()
// {
//     circular_buffer cb;
//     buffer_init(&cb, 3);
//     buffer_AddData(&cb, 1);
//     buffer_printData(&cb);
//     buffer_AddData(&cb, 2);
//     buffer_printData(&cb);
//     buffer_AddData(&cb, 3);
//     buffer_printData(&cb);
//     buffer_AddData(&cb, 4);
//     buffer_printData(&cb);
//     buffer_resize(&cb, 5);
//     buffer_printData(&cb);
//     buffer_AddData(&cb, 5);
//     buffer_printData(&cb);
//     buffer_AddData(&cb, 6);
//     buffer_printData(&cb);
//     buffer_resize(&cb, 3);
//     buffer_printData(&cb);
// }