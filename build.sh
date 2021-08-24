#!/bin/bash
gcc $(yed --print-cflags) $(yed --print-ldflags) -o auto_paren.so -g auto_paren.c
