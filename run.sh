#!/bin/bash

echo "Iniciando storage..."
gnome-terminal -- bash -c "cd storage && ./bin/storage storage.config; exec bash"

#echo "Iniciando placa..."
#gnome-terminal -- bash -c "cd placa && ./bin/placa; exec bash"

echo "Iniciando planificador..."
gnome-terminal -- bash -c "cd planificador && ./bin/planificador planificador.config pseudocodigos; exec bash"

#echo "Iniciando core..."
#gnome-terminal -- bash -c "cd core && ./bin/core; exec bash"
