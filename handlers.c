#include "handlers.h"

#include "safe.h"

// we don't really do anything for the simplest kind of sprite
// except rendering the sprite
int handler_sprite_init(struct sprite *s) {
  assert_not_null(1, s);
  return 0;
}

int handler_sprite_frame(struct sprite *s) { return handler_sprite_init(s); }

int handler_sprite_hit(struct sprite *s) { return handler_sprite_init(s); }

int handler_sprite_destroy(struct sprite *s) { return handler_sprite_init(s); }
