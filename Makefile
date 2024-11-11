SUBDIR=level-editor

.PHONY: build-all
build-all: build build-$(SUBDIR) build-web build-web-$(SUBDIR)

.PHONY: build
build: setup
	meson compile -C build

.PHONY: build-web
build-web: setup-web
	meson compile -C build-web

.PHONY: build-web-$(SUBDIR)
build-web-$(SUBDIR): setup-web
	make -C $(SUBDIR) build-web && cd $(SUBDIR) && mv dist/index.html dist/level-editor.html && mv dist/* ../build-web/ 

.PHONY: build-web-all
build-web-all: build-web build-web-level-editor

OS_NAME := $(shell uname -s)

EXE_FILE := "lily"
DIST_FILE := "lily-unknown.zip"

.PHONY: dependencies-$(SUBDIR)
dependencies-$(SUBDIR):
	make -C $(SUBDIR) dependencies

.PHONY: build-$(SUBDIR)
build-$(SUBDIR):
	make -C $(SUBDIR) build

.PHONY: run-$(SUBDIR)
run-$(SUBDIR):
	make -C $(SUBDIR) run

.PHONY: dependencies
ifeq ($(OS_NAME), Darwin)
$(info Architecture: $(OS_NAME))
DIST_FILE = "lily-darwin.zip"
dependencies:
	brew install python3 meson ninja sdl2 sdl2_ttf sdl2_mixer emscripten gtk+3
else ifeq ($(OS_NAME), Linux)
$(info Architecture: $(OS_NAME))
DIST_FILE = "lily-linux.zip"
.PHONY: update-apt-get
update-apt-get:
	sudo apt-get update
dependencies: update-apt-get
	sudo apt-get install -y meson ninja-build libsdl2-dev libsdl2-2.0-0 libmikmod-dev libfishsound1-dev libsmpeg-dev liboggz2-dev libflac-dev libfluidsynth-dev libsdl2-mixer-dev libsdl2-mixer-2.0-0 libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0 libasound2-dev libpulse-dev libgbm-dev libwayland-dev libxkbcommon-dev libgtk-3-dev
else ifeq ($(OS), Windows_NT)
$(info Architecture: $(OS))
DIST_FILE = "lily-windows.zip"
EXE_FILE = "lily.exe"
dependencies:
	echo "dependencies target not implemented for windows yet"
else
dependencies:
	echo "dependencies target only implemented for Linux and Mac OS for now"
endif

.PHONY: setup-dist-web
setup-dist-web: build-web-all
	rm -rf dist-web && mkdir dist-web && cp build-web/*.html dist-web/ && cp build-web/*.js dist-web/ && cp build-web/*.wasm dist-web/

.PHONY: dist-web 
dist-web: setup-dist-web
	cd dist-web && zip -r ../lily-web.zip .

.PHONY: setup-dist
setup-dist: build
	mkdir -p dist && cp build/$(EXE_FILE) dist/ && cp -r share dist/ && cp -r levels dist/

.PHONY: dist
dist: setup-dist
	cd dist && zip -r ../$(DIST_FILE) .

.PHONY: setup
setup:
	meson setup build

.PHONY: setup-debug
setup-debug:
	meson setup --buildtype=debug build-debug

.PHONY: setup-web
setup-web:
	meson setup --cross-file emscripten.ini build-web

.PHONY: debug
debug: setup-debug
	meson compile -C build-debug

.PHONY: run
run: build
	./build/lily

.PHONY: clean
clean:
	rm -rf build
	rm -rf build-debug
	rm -rf build-web
	rm -rf dist
	rm -rf $(DIST_FILE)
	rm -rf dist-web
	rm -rf lily-web.zip

.PHONY: clean-$(SUBDIR)
clean-$(SUBDIR):
	cd $(SUBDIR) && make clean

.PHONY: clean-all
clean-all: clean-$(SUBDIR) clean

.PHONY: test
ifeq ($(OS_NAME), Linux)
# --interactive doesn't seem to be on the linux version of meson
# available on apt-get
test: build
	meson test -C build
else
test: build
	meson test -C build --interactive
endif

.PHONY: test-$(SUBDIR)
test-$(SUBDIR):
	make -C $(SUBDIR) test

.PHONY: test-all
test-all: test test-$(SUBDIR)