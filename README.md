# Sure Instinct

Source code to the SNES game "Sure Instinct" which has been written during the SNES game dev on itch.io back in 2021.

You can download the game on itch.io: https://bennysnesdev.itch.io/sure-instinct

The source code is not cleaned up and contains lot of dead code from previous works. It also might be difficult to compile for the first time. While the project has been developed for crossplatform use, it has only been compiled under linux yet and thus might be very difficult to compile using different systems.

Please note, that I can not give any help or support on compiling this project.

## Requirements

- [FMA Famicom Macro Assembler](https://github.com/BenjaminSchulte/fma) (tested with commit 1df2842d5746639fe4bb1492dfd20a363fbf6e98)
- [My bsnes-plus branch](https://github.com/BenjaminSchulte/bsnes-plus) (recommended if you want to debug the game)
- QT5 development packages

## Compile

To compile a SNES ROM you need to run the following commands. You might run into missing dependency errors or something depending on your system and compiler.

```
cd /path/to/this/sureinstinct.git/

./bin/libsft-compile

cd aoba-build
mkdir build
cd build
cmake ..
make
cd ../..

mkdir -p sureinstinct/obj
mkdir -p sureinstinct/dist

./bin/si-run
```

## Level editor

To modify any levels you might want to run the original level editor. The editor is unfinished, but provides the mandatory features to update maps.

```
./bin/si-editor
```

## Music editor

To modify the music or and sfx you can either use a regular text editor or use the music studio:

```
cd aoba-musicstudio
mkdir build
cd build
cmake ..
make
cd ../..

./bin/start-musicstudio
```


## License

This project has been published under MIT license. See LICENSE.md