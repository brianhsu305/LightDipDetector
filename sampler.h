// sampler.h
// Module to sample light levels in the background (thread).
// It provides access to a reading history of configurable
// length, the average light level, and the number of samples taken.
#ifndef _SAMPLER_H_
#define _SAMPLER_H_

// sleep for X amount of time
void sleepForMs(long long delayInMs);

// get the reading from the light sensor
int getVoltageReading(char* voltagePath);

//print related data
void Sampler_printData();

//detect for the number of dips
void Sampler_dipDetection(void);

// Initialize Sampler sampling pthread
void Sampler_samplingThreadInit(void);
void *Sampler_samplingThreadFunc(void);

// Initialize Sampler Print data pthread
void Sampler_printThreadInit(void);
void *Sampler_printThreadFunc(void);

// Begin/end the background thread which samples light levels.
void Sampler_startSampling(void);
void Sampler_stopSampling(void);

// Set/get the maximum number of samples to store in the history.
void Sampler_setHistorySize(int newSize);
int Sampler_getHistorySize(void);

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `length` to be the
// number of elements in the returned array (output-only parameter). // The calling code must call free() on the returned pointer.
// Note: provides both data and size to ensure consistency.
double *Sampler_getHistory(int *length);

// Returns how many valid samples are currently in the history.
// May be less than the history size if the history is not yet full.
int Sampler_getNumSamplesInHistory();

// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void);

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void);


#endif