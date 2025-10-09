#!/usr/bin/env bash

echo Compiling... &&
	mpicc -o out ./main.c &&
	echo Running... &&
	mpirun -n 6 out
