#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

typedef struct circular_buffer
{
    double *buffer;
    int head;
    int tail;
    int size;
    long long count; // # of samples scanned (could be bigger than buffer size)
} circular_buffer;

void buffer_init(circular_buffer *cb, int bufferSize);
void buffer_free(circular_buffer *cb);
void buffer_AddData(circular_buffer *cb, double item);

#endif