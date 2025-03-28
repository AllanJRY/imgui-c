# TODO: accept args to build as RELEASE or DEV

clang -g3 -march=native -fsanitize=address -static-libasan \
 -Wall -Wextra -Wshadow -Wundef \
 -pedantic \
 -Iinclude -Isrc \
 -lkernel32 -luser32 -lgdi32 \
 -DDEBUG \
 src/main_linux.c -o output/main
