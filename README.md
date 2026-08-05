# Parrot

A modern, lightweight music player written in C++ with a focus on polished desktop playback. The application aims to provide a fast, responsive UI and high-quality audio playback with features similar to modern players (Spotify, iTunes) for local music collections and optional streaming support.

## Features

- Library management
  - Scan local folders and import audio files (MP3, FLAC, WAV, etc.)
  - Read and display metadata (title, artist, album, track number, year, genre)
  - Album art display and caching

- Playback
  - Gapless playback and crossfade support (configurable)
  - Playback controls: play/pause, next/previous, seek, shuffle, repeat
  - Per-track and global volume control and mute
  - Smooth, low-latency audio output

- Playlists
  - Create, edit, save and reorder playlists

- Search & Sorting
  - Fast search across tracks, albums, and artists
  - Sort by many properties including title, artist, album, date, duration

## Quick build & run (typical CMake workflow)

Install required development dependencies (example for a Debian/Ubuntu system — adapt to your OS and actual deps):
- build-essential, cmake, git
- libqt6-dev (or libqt5-dev), libtag1-dev, libsndfile1-dev, libportaudio2-dev, libcurl4-openssl-dev

Clone and build:
```bash
git clone https://github.com/amirmohsen1384/parrot.git
cd parrot
mkdir build && cd build
cmake ..
cmake --build . --config Release
