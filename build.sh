#!/bin/bash

echo "Compilando utils..."
cd utils || exit
make clean
make
cd ..

echo "Compilando storage..."
cd storage || exit
make clean
make
cd ..

echo "Compilando placa..."
cd placa || exit
make clean
make
cd ..

echo "Compilando planificador..."
cd planificador || exit
make clean
make
cd ..

echo "Compilando core..."
cd core || exit
make clean
make
cd ..

echo "Build completo ✔"
