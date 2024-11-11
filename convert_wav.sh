#!/bin/bash

# This is because emscripten throws the following error on certain
# wav files:
# No conversion available for these rates
# We just convert all files to ogg

# Function to convert a WAV file to 44.1 kHz and overwrite the original
convert_wav() {
  ffmpeg -i "$1" -codec:a libvorbis "${1%.wav}.ogg"
  rm "$1"
}

# Find all WAV files in the current directory
wav_files=$(find ./share/sounds/ -name "*.wav")

# Convert each WAV file
for wav_file in $wav_files; do
  convert_wav "$wav_file"
done