#!/bin/bash

echo "Iniciando storage con Helgrind..."
gnome-terminal -- bash -c "cd storage && valgrind --tool=helgrind ./bin/storage storage.config; exec bash"

echo "Iniciando placa con Helgrind..."
gnome-terminal -- bash -c "cd placa && valgrind --tool=helgrind ./bin/placa placa.config; exec bash"

echo "Iniciando planificador con Helgrind..."
gnome-terminal -- bash -c "cd planificador && valgrind --tool=helgrind ./bin/planificador planificador.config pseudocodigos; exec bash"

echo "Iniciando core con Helgrind..."
gnome-terminal -- bash -c "cd core && valgrind --tool=helgrind ./bin/core core.config id_core; exec bash"
