#!/bin/bash

G_SLICE=always-malloc G_DEBUG=gc-friendly valgrind --tool=memcheck --leak-check=yes --leak-resolution=high --num-callers=20 $1
