// circular_buffer.h
// Handles initializing, freeing, and adding data to the circular buffer
#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

typedef struct circular_buffer
{
    double *buffer; 
    int head;       // index of the head (write)
    int tail;       // index of the tail (read)
    int size;       // size of the buffer
    long long count; // # of samples scanned in total (could be bigger than buffer size)
} circular_buffer;

// Initialize circular buffer
void buffer_init(circular_buffer *cb, int bufferSize);\

// Free the buffer 
void buffer_free(circular_buffer *cb);

// Add data to the circular buffer
void buffer_AddData(circular_buffer *cb, double item);

#endif