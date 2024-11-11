#include "base.h"
#include "game.h"
#include "safe.h"
#include "state.h"

int main(int argc, char *argv[]) {
  SAFE_UNUSED(argc);
  SAFE_UNUSED(argv);

  // let's first set the state of the game to GAME_NOT_STARTED, so the
  // game_main_loop initialized the game
  g_prog.state = PROG_NOT_STARTED;

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(game_main_loop, 0, 1);
#else
  while (1) {
    game_main_loop();
  }
#endif

  return EXIT_SUCCESS;
}
