#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

typedef struct circular_buffer
{
    int *buffer;
    void *head;
    void *tail;
    void* buffer_end;
    int length;
    int sz;
    int count; // # of samples scanned (could be bigger than buffer size)
    bool full;
} circular_buffer;

void buffer_init(circular_buffer *cb, int bufferSize, size_t sz);
void buffer_free(circular_buffer *cb);
void buffer_AddData(circular_buffer *cb, void* item);

#endif