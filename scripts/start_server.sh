#!/bin/bash

# Pulisce e ricompila il progetto
make clean
make

# Pulisce le cartelle dei pacchetti e dei log
./bin/clear_packages
./bin/clear_logs

# Chiude il server se è già in esecuzione
pkill -x "server" 2>/dev/null || true

# Avvia il server in background
./bin/server &
SERVER_PID=$!

echo "Server avviato con PID $SERVER_PID"
echo "Premi Ctrl+C per terminare il server."

# Mantiene il processo in esecuzione per permettere la terminazione manuale
trap 'echo "Terminazione del server..."; kill $SERVER_PID; exit 0' SIGINT
wait $SERVER_PID

