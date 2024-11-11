#ifndef MESSAGE_H
#define MESSAGE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// this file contains functionality for rendering and updating the game message
// (shown at the bottom of the screen)

// message is primarily used to store information regarding game messages,
// rendered with time-outs
struct message {
  // the current message to be displayed
  char *m;
  // the backup buffer
  char *buffer;
  // if blocking is set to true, the game is effectively "paused" until the
  // user dismisses the message
  bool blocking;
};

// message_create creates a new message with default values, returns NULL on
// error.
struct message *message_create(void);

// message_destroy frees the memory taken up by the message
void message_destroy(struct message **pm);

// message_iterate updates the timeout value of the message and "progresses" the
// message if `pressed` is true. This can be a key-press.
void message_iterate(struct message *msg, const bool pressed);

// message_set sets the current game message. It can be a maximum of
// MAX_MESSAGE_LEN characters. If it is larger than MESSAGE_BUFFER_LEN
// characters, the message gets buffered. All characters beyond MAX_MESSAGE_LEN
// are ignored. Set blocking to true if you want the game to effectively be
// "paused" while the message is being displayed.
void message_set(struct message *msg, const char *m, const bool blocking);

// message_block returns true if we currently have a message being
// displayed and we are in "blocking" mode. It returns false otherwise.
bool message_block(const struct message *msg);

#endif // MESSAGE_H
