#include "message.h"

#include "base.h"
#include "fps.h"
#include "safe.h"
#include "sound.h"

static const Sint64 DELAY = 250;

enum {
  CONTINUE_MSG_LEN = 6,
  // Maximum amount of monospaced characters that can fit in one game message,
  // as we determined experimentally minus the len
  MESSAGE_BUFFER_LEN = 146,
  // The maximum messages we can deliver to the user in one go. After 4 messages
  // in one go, it gets a bit tedious for a game.
  MAX_MESSAGE_LEN = MESSAGE_BUFFER_LEN * 4,
  // Number of messages the backup buffer can hold
  BACKUP_BUFFER_MESSAGES = 3,
};

static const char CONTINUE_MSG[CONTINUE_MSG_LEN] = "\n ... ";

struct message *message_create(void) {
  struct message *m = malloc(sizeof(struct message));
  if (m == NULL) {
    goto error_out;
  }

  m->m = (char *)calloc(1, MESSAGE_BUFFER_LEN + CONTINUE_MSG_LEN + 1);
  if (m->m == NULL) {
    goto m_error_out;
  }

  m->buffer =
      (char *)calloc(1, 1 + MESSAGE_BUFFER_LEN * BACKUP_BUFFER_MESSAGES);
  if (m->buffer == NULL) {
    goto m_m_error_out;
  }

  return m;

m_m_error_out:
  free(m->m);
m_error_out:
  free(m);
error_out:
  LOG_ERROR("message creation failed");
  return NULL;
}

void message_destroy(struct message **pm) {
  assert_not_null(1, pm);
  struct message *m = *pm;
  assert_not_null(1, m);
  free(m->m);
  free(m->buffer);
  free(m);
  *pm = NULL;
}

void message_set(struct message *msg, const char *m, const bool blocking) {
  assert_not_null(2, msg, m);
  msg->blocking = blocking;

  size_t len = strnlen(m, MAX_MESSAGE_LEN);
  size_t min_len = strnlen(m, MESSAGE_BUFFER_LEN);

  // if the previous message is empty AND the current message is empty, do *not*
  // play the sound effect
  if (strnlen(msg->m, 1) > 0 || min_len > 0) {
    sound_play(SOUND_MESSAGE, CHANNEL_UI);
  }

  strlcpy(msg->m, m, min_len + 1);

  // if messager is longer than what msg->m can hold, buffer the rest of the
  // message
  if (min_len < len) {
    strlcat(msg->m, CONTINUE_MSG, MESSAGE_BUFFER_LEN + CONTINUE_MSG_LEN + 1);
    strlcpy(msg->buffer, m + min_len, MAX_MESSAGE_LEN + 1);
  }
}

bool message_block(const struct message *msg) {
  assert_not_null(2, msg, msg->m);

  if (!msg->blocking) {
    return false;
  }

  return strnlen(msg->m, 1) > 0;
}

void message_iterate(struct message *msg, const bool pressed) {
  static Sint64 respond = DELAY;

  const Uint64 dt = fps_frame_time();
  respond -= dt;

  if (respond <= 0 && pressed) {
    respond = DELAY;
    bool buffer_empty = strnlen(msg->buffer, 1) == 0;

    if (!buffer_empty) {
      strlcpy(msg->m, msg->buffer, MESSAGE_BUFFER_LEN + 1);
      strlcpy(msg->buffer, msg->buffer + MESSAGE_BUFFER_LEN,
              MAX_MESSAGE_LEN + 1);

      buffer_empty = strnlen(msg->buffer, 1) == 0;
      if (!buffer_empty) {
        strlcat(msg->m, CONTINUE_MSG,
                MESSAGE_BUFFER_LEN + CONTINUE_MSG_LEN + 1);
      }
    } else {
      strlcpy(msg->m, "", MESSAGE_BUFFER_LEN + 1);
    }
  }
}
