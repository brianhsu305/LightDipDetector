// udp_sockets.h
// Take each packet as a command to respond to the sender
// UDP packets may have one or more packets containing the "return" message (plain text)
#ifndef _UDP_SOCKETS_H_
#define _UDP_SOCKETS_H_

// Initialize the UDP thread
void UDP_threadInit(void);

// Read the user input command
void UDP_readCommand(char *command);

// Return a brief summary/list of supported commands
void UDP_replyHelp(void);

// Return the total number of light samples taken so far
void UDP_replyCount(void);

// Return both the max size of the history, and the number of samples currently in the history
void UDP_replyLength(void);

// Return all the data samples in the history
// Multiple packets returned if the history is too big for one packet
void UDP_replyHistory(void);

// Return the N most recent Sample history value
// Display Error if N > number of samples currently in the history (invalid range of values)
void UDP_replyGet(int num);

// Return the number of Dips
void UDP_replyDips(void);

// Exit the program
// Close all open sockets, files, pipes, threads, and free dynamically allocated memory
void UDP_replyStop(void);

// Blank input repeats the previous command
void UDP_replyEnter(void);

// Unknown command will return a message indicating it's unknown
void UDP_unknownCommand(void);

#endif