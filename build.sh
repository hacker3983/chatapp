#!/bin/bash
echo "Building chatapp..."
gcc main.c app.c app_strings.c chatsock.c message_list.c inputbox.c -lSDL2 -lSDL2_ttf -o main
echo "Running chatapp..."
./main