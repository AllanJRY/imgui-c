:: TODO: accept args to build as RELEASE or DEV

clang -g3 -march=native^
 -Wall -Wextra -Wshadow -Wundef^
 -pedantic^
 -Iinclude -Isrc^
 -lkernel32 -luser32 -lgdi32^
 src/main_windows.c -o output/main.exe
