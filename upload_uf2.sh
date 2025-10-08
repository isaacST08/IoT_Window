#! /usr/bin/env bash

set -e

RPI_pico_mount="/mnt/RPIpico"

# Get the path of the uf2 file to upload
uf2_path=$1

# Check the path is to a .uf2 file
if [[ "$uf2_path" != *".uf2" ]]; then
  printf "%s is not a path to a valid .uf2 file.\nAborting.\n" "$uf2_path"
fi

# Mount the rpi pico
mount --label RPI-RP2 "$RPI_pico_mount"

# Copy the .uf2 file to the RPI pico
cp "$uf2_path" "$RPI_pico_mount"

