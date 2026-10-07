#!/bin/bash

echo "Iniciando storage con Valgrind..."
gnome-terminal -- bash -c "cd storage && valgrind --leak-check=full --show-leak-kinds=all ./bin/storage storage.config; exec bash"

echo "Iniciando placa con Valgrind..."
gnome-terminal -- bash -c "cd placa && valgrind --leak-check=full --show-leak-kinds=all ./bin/placa placa.config; exec bash"

echo "Iniciando planificador con Valgrind..."
gnome-terminal -- bash -c "cd planificador && valgrind --leak-check=full --show-leak-kinds=all ./bin/planificador planificador.config pseudocodigos; exec bash"

echo "Iniciando core con Valgrind..."
gnome-terminal -- bash -c "cd core && valgrind --leak-check=full --show-leak-kinds=all ./bin/core core.config id_core; exec bash"
