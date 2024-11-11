#ifndef FILE_CHOOSER_H
#define FILE_CHOOSER_H

// -- a cross platform file chooser dialog --

// file_chooser_open opens a file chooser dialog box, lets the user pick a file,
// and returns the filename (or null on error). The user is responsible for
// freeing the filename.
char *file_chooser_open(void);

#endif // FILE_CHOOSER_H
