#!/usr/bin/env bash
gcc -o auto_paren.so auto_paren.c $(yed --print-cflags) $(yed --print-ldflags)
