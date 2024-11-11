#include "file_chooser.h"

#include "safe.h"

#ifdef __EMSCRIPTEN__
#include "emscripten_compatibility.h"

static bool _file_chooser_closed = false;
static char *_filename = NULL;

// javascript function to handle file selection
EM_ASYNC_JS(char *, file_chooser_open, (), {
  async function read_file(file) {
    return new Promise(function(resolve, reject) {
      const reader = new FileReader();
      reader.onload = function() { resolve(reader.result); };
      reader.onerror = function() { reject(reader.error); };
      reader.readAsText(file);
    });
  }

  async function file_chooser_async(resolve, reject) {
    var input = document.createElement('input');
    input.type = 'file';
    input.accept = '.level';
    input.style.display = 'none';

    input.onchange = async function(e) {
      var file = e.target.files[0];
      if (file) {
        var filename = file.name; // convert this to a C string
        // length in bytes
        var n_filename = lengthBytesUTF8(filename) + 1;
        var filename_on_wasm_heap = _malloc(n_filename);
        // copy JS string to wasm heap
        stringToUTF8(filename, filename_on_wasm_heap, n_filename);

        // clean up the input element
        document.body.removeChild(input);

        // save file and its contents into MEMFS
        const reader = new FileReader();
        const level_content = await read_file(file);
        FS.writeFile(filename, level_content);
        console.log("saved level to filename {}", filename);

        resolve(filename_on_wasm_heap);
      } else {
        resolve(null); // send back NULL
      }
    };

    // if the user cancelled the file dialog
    input.oncancel = function() {
      // still clean it up
      document.body.removeChild(input);
      resolve(null); // send back NULL
    };

    document.body.appendChild(input);
    input.click();
  }

  async function file_chooser() {
    return new Promise(
        function(resolve, reject) { file_chooser_async(resolve, reject); });
  }

  const filename_on_wasm_heap = await file_chooser();
  return filename_on_wasm_heap;
});

#elif defined(_WIN32)

char *file_chooser_open(void) {
  LOG_ERROR("Unsupported on Windows");
  return NULL;
}

#else // not emscripten or windows
#include <gtk/gtk.h>
#include <stdbool.h>

char *file_chooser_open(void) {
  static bool gtk_initialized = false;

  // if GTK is not initialized, initialize it
  if (!gtk_initialized) {
    if (!gtk_init_check(NULL, NULL)) {
      LOG_ERROR("could not initialize GTK for file chooser dialog");
      return NULL;
    }

    gtk_initialized = true;
  }

  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel",
      GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

  GtkFileFilter *filter_level = gtk_file_filter_new();
  gtk_file_filter_set_name(filter_level, "Level Files");
  gtk_file_filter_add_pattern(filter_level, "*.level");

  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter_level);

  gint response = gtk_dialog_run(GTK_DIALOG(dialog));

  char *filename = NULL;

  if (response == GTK_RESPONSE_ACCEPT) {
    // get the filename - this allocates new memory in the heap
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  }

  gtk_widget_destroy(dialog);
  return filename;
}
#endif
