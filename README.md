# tantrix

A simple search tool for tantrix solution wrritten in C.

# Warning this project works just enough to be commited !!

## Quick Start

This project uses [nob.h](https://github.com/tsoding/nob.h) by Tsoding for its build tool, check it out it's actually really cool.

### Prerequistites

A reasonnable C compiler (gcc, clang, msvc, ...)\
The [raylib](https://github.com/raysan5/raylib) graphic library.

### Bootstraping nob

The first time (and only the first time) you build this project you need to compile nob.c use your C compiler of choice:
```(bash)
$ cc nob.c -o nob
```

### Building and running tantrix

Every subsequent time you can just run
```(bash)
$ ./nob
```
This will recompile nob if needed and build the project \
You can then run tantrix with:
```(bash)
$ ./bin/main
```

## Usage

You can change the number of pieces the project tries to fit together with the `difficulty` variable in `src/main.c`. \
In the same file you can change the window size from the default `800x400` to you desired resolution. \
Within the application you can use WASD to move the camera around
