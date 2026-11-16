
# MP3 Tag Reader and Editor

# Project Overview
The MP3 Tag Reader and Editor is a C-based application that reads and modifies metadata stored in MP3 audio files using the ID3v2.3 format.

# Features
- Read MP3 metadata.
- Edit song title.
- Edit artist name.
- Edit album name.
- Edit release year.
- Edit genre.
- Resize the ID3 tag when updated metadata requires additional space.
- Preserve the original MP3 audio data.

# Technologies Used
- Programming Language: C
- Compiler: GCC
- Operating Environment: Linux / Ubuntu
- File Handling: Standard C file I/O
- Metadata Format: ID3v2.3

# Project Structure
- main.c – Main function and command-line argument handling.
- mp3.h – Function declarations and header file.
- mp3view.c – Reads and displays MP3 metadata.
- mp3edit.c – Edits MP3 metadata.
- Makefile – Automates compilation.

# Compilation
```bash
gcc -Wall -Wextra -std=c11 main.c mp3view.c mp3edit.c -o mp3tag
```

Or use:
```bash
make
```

# Usage

# View MP3 metadata
```bash
./mp3tag -v song.mp3
```

# Edit title
```bash
./mp3tag -e -t "New Song Title" song.mp3
```

# Edit artist
```bash
./mp3tag -e -a "Artist Name" song.mp3
```

# Edit album
```bash
./mp3tag -e -A "Album Name" song.mp3
```

# Edit year
```bash
./mp3tag -e -y "2026" song.mp3
```

# Edit genre
```bash
./mp3tag -e -m "Technology" song.mp3
```

# Supported Metadata Frames
| Frame ID | Description |
|---|---|
| TIT2 | Title |
| TPE1 | Artist |
| TALB | Album |
| TYER | Year |
| TCON | Genre |

# Limitations
- Supports ID3v2.3 files.
- Comment editing is not implemented.
- Adding metadata frames that do not already exist is not implemented.
- UTF-16 text encoding and advanced ID3 features are not fully supported.

# Author
Lokesh Sudeep Mandage
