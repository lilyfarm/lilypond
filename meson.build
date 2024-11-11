project(
  'lily',
  version: '0.0.1',
  license: 'GNU GPL v3',
  default_options: ['warning_level=3', 'werror=true'],
)

if host_machine.system() == 'windows'
# We are seeing issues with libharfbuzz on windows with a C compiler
  add_languages('cpp')
  add_languages('c')
  override_options = ['cpp_std=c++11']
  link_language='cpp'
else
  add_languages('c')
  override_options = ['c_std=c11']
  link_language='c'
endif



# Decide whether to use static or dynamic linking
use_static = true

# Static compilation seems to be broken for Linux
# TODO: find a way to fix this
if host_machine.system() == 'linux'
  use_static = false
endif

sources = [
  'game.c',
  'base.c',
  'array.c',
  'handlers.c',
  'handlers_item.c',
  'handlers_spider.c',
  'handlers_water.c',
  'handlers_helper.c',
  'handlers_ghost.c',
  'handlers_platform.c',
  'handlers_spring.c',
  'handlers_skeleton.c',
  'render.c',
  'level.c',
  'message.c',
  'fps.c',
  'scene.c',
  'scene_game.c',
  'scene_intro.c',
  'scene_end.c',
  'scene_menu.c',
  'scene_none.c',
  'scene_options.c',
  'scene_text_utils.c',
  'scene_acknowledgements.c',
  'sound.c',
  'player.c',
  'util.c',
  'camera.c',
  'safe.c',
  'token.c',
  # Compatibility -- there are no-ops if we are not on the right platform
  'emscripten_compatibility.c',
  'windows_compatibility.c',
  'linux_compatibility.c',
  'sprite.c',
  'sprite_type.c',
  'state.c',
  'default_levels.c',
  'file_chooser.c',
]

global_dependencies = []
global_link_args = []

if host_machine.system() != 'windows'
  cc = meson.get_compiler('c')
  is_emscripten = cc.get_id() == 'emscripten'
else
  cpp = meson.get_compiler('cpp')
  is_emscripten = false
  global_link_args += ['-static']
endif

# If we are building for the web, no need to build tests etc. We do all
# our testing locally. To build for the web, use the Makefile command:
# make build-web
# otherwise, just use
# make build
if is_emscripten
  sources += ['main.c']

  em_args = [
    '-s', 'USE_SDL=2',
    '-s', 'USE_SDL_TTF=2',
    '-s', 'USE_SDL_MIXER=2',
    '-s', 'WASM=1',
    '-s', 'ALLOW_MEMORY_GROWTH=1',
    '-s', 'ASYNCIFY',

    # Embed the required files
    '--embed-file=../share@share',
    '--embed-file=../levels@levels',

    # Custom shell file for the game
    '--shell-file=../web_entrypoint.html',
  ]

  # Create a custom target for the HTML output
  custom_target(
    'lily_html',
    input: sources,
    output: 'lily.html',
    command: [cc.cmd_array(), em_args, '@INPUT@', '-o', '@OUTPUT@'],
    build_by_default: true,
  )
else
  # Check if we're on macOS
  if host_machine.system() == 'darwin'
    # To remove the warning
    # ld: warning: ignoring duplicate libraries: '-lobjc'
    global_link_args += ['-Wl,-no_warn_duplicate_libraries']
  endif

  if use_static
    message('Attempting to use static libraries')
    sdl2_dep = dependency('sdl2', static: true, required: false)
    sdl2_ttf_dep = dependency('SDL2_ttf', static: true, required: false)
    sdl2_mixer_dep = dependency('SDL2_mixer', static: true, required: false)
    gtk3 = dependency('gtk+-3.0', required: false, static: true)


    if not sdl2_dep.found() or not sdl2_ttf_dep.found() or not sdl2_mixer_dep.found() or not gtk3.found()
      error(
        'Static libraries not found. Please install static versions of SDL2, SDL2_mixer, SDL2_ttf and GTK+ 3, or set use_static=false',
      )
    endif

    add_project_arguments('-DSDL_STATIC_LIB', language: 'c')
  else
    message('Using dynamic libraries')
    sdl2_dep = dependency('sdl2', required: true)
    sdl2_ttf_dep = dependency('SDL2_ttf', required: true)
    sdl2_mixer_dep = dependency('SDL2_mixer', required: true)
    gtk3 = dependency('gtk+-3.0', required: true)
  endif

  global_dependencies += [sdl2_dep, sdl2_ttf_dep, sdl2_mixer_dep]

  # having trouble getting gtk 3 to work on windows
  if host_machine.system() != 'windows'
    global_dependencies += [gtk3]
  endif

  if host_machine.system() == 'linux'
    xcb = dependency('xcb', required: true)
    flac = dependency('flac', required: true)
    global_dependencies += [xcb, flac]
  endif

  # Create the executable
  executable(
    'lily',
    sources + ['main.c'],
    dependencies: global_dependencies,
    # Remove any duplicate libraries
    link_args: global_link_args,
    override_options: override_options,
    link_language: link_language)

  # Print the type of SDL2 dependency we're using
  message('SDL2 dependency type: ' + sdl2_dep.type_name())

  # Tests
  array_test = executable(
    'array_test',
    sources + ['array_test.c'],
    dependencies: global_dependencies,
    link_args: global_link_args,
    override_options: override_options,
    link_language: link_language)

  test('array test', array_test)

  level_test = executable(
    'level_test',
    sources + ['level_test.c'],
    dependencies: global_dependencies,
    link_args: global_link_args,
    override_options: override_options,
    link_language: link_language)

  test('level test', level_test)

  camera_test = executable(
    'camera_test',
    sources + ['camera_test.c'],
    dependencies: global_dependencies,
    link_args: global_link_args,
    override_options: override_options,
    link_language: link_language)

  test('camera test', camera_test)

  util_test = executable(
    'util_test',
    sources + ['util_test.c'],
    dependencies: global_dependencies,
    link_args: global_link_args,
    override_options: override_options,
    link_language: link_language)

  test('util test', util_test)

  fps_test = executable(
    'fps_test',
    sources + ['fps_test.c'],
    dependencies: global_dependencies,
    link_args: global_link_args,
    override_options: override_options,
    link_language: link_language)

  test('fps test', fps_test)
endif