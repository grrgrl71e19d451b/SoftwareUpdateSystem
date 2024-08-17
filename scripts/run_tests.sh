#!/bin/bash

# Funzione per eseguire il test di invio e ricezione di pacchetti multipli
test_multiple_packages() {
    echo "1) Esecuzione del test di invio e ricezione di pacchetti multipli..."
    ./bin/clear_packages
    ./bin/clear_logs
    sleep 2
    ./bin/client_upload commands/upload_multiple.cmd
    wait
    ./bin/client_download commands/download_multiple.cmd
    wait
    echo "Controlla i log nei file logs/server_log.log e logs/client_download_log.log"
    echo "Test di invio e ricezione di pacchetti multipli completato."
}

# Funzione per eseguire il test della gestione delle versioni
test_version_management() {
    echo "2) Esecuzione del test di gestione delle versioni dei pacchetti..."
    ./bin/clear_packages
    ./bin/clear_logs
    sleep 2
    ./bin/client_upload commands/upload_versions.cmd
    wait
    ./bin/client_download commands/download_versions.cmd
    echo "Controlla i log nei file logs/server_log.log e logs/client_download_log.log per i dettagli."
    echo "Test di gestione delle versioni completato."
}

# Funzione per eseguire il tentativo di caricamento delle stesse versioni dei pacchetti
test_version_control_repeat() {
    echo "3) Esecuzione del test di controllo della versione con caricamento ripetuto..."
    ./bin/clear_packages
    ./bin/clear_logs
    sleep 2
    ./bin/client_upload commands/upload_multiple.cmd
    wait
    ./bin/client_download commands/download_multiple.cmd
    wait
    ./bin/client_upload commands/upload_multiple.cmd
    echo "Controlla i log nei file logs/server_log.log e logs/client_upload_log.log per i dettagli."
    echo "Test di controllo della versione con caricamento ripetuto completato."
}

# Funzione per eseguire il test di accesso a una risorsa condivisa
test_shared_resource_access() {
    echo "4) Esecuzione del test di accesso a una risorsa condivisa..."
    ./bin/clear_packages
    ./bin/clear_logs
    sleep 2
    echo "Avvio client upload..."
    ./bin/client_upload commands/upload_single.cmd &
    wait
    echo "Avvio client download concorrente sulla stessa risorsa..."
    ./bin/client_download commands/download_single.cmd &
    ./bin/client_download commands/download_single.cmd &
    ./bin/client_download commands/download_single.cmd &
    ./bin/client_download commands/download_single.cmd &
    wait
    echo "Controlla i log nei file logs/server_log.log e logs/client_download_log.log"
    echo "Test di accesso a una risorsa condivisa completato."
}

# Funzione per eseguire il test di accesso concorrente di più client al server
test_concurrent_access() {
    echo "5) Esecuzione del test di accesso concorrente di più client al server..."
    ./bin/clear_packages
    ./bin/clear_logs
    sleep 2
    echo "Avvio client upload..."
    ./bin/client_upload commands/upload_multiple_1.cmd &
    ./bin/client_upload commands/upload_multiple_2.cmd &
    wait
    echo "Avvio client download..."
    ./bin/client_download commands/download_multiple_1.cmd &
    ./bin/client_download commands/download_multiple_2.cmd &
    wait
    echo "Controlla i log nei file logs/server_log.log e logs/client_download_log.log"
    echo "Test di accesso concorrente di più client completato."
}

# Funzione per eseguire il test di upload e download concorrente
test_concurrent_upload_download() {
    echo "6) Esecuzione del test di upload e download concorrente..."
    ./bin/clear_packages
    ./bin/clear_logs
    sleep 2
    echo "Avvio client upload..."
    ./bin/client_upload commands/upload_multiple_1.cmd &
    ./bin/client_upload commands/upload_multiple_2.cmd &
    wait
    echo "Avvio client upload e download..."
    ./bin/client_upload commands/upload_multiple_3.cmd &
    ./bin/client_download commands/download_multiple_1.cmd &
    ./bin/client_download commands/download_multiple_2.cmd &
    wait
    echo "Controlla i log nei file logs/server_log.log e logs/client_download_log.log"
    echo "Test di upload e download concorrente completato."
} 

# Attende che l'utente prema un tasto per iniziare i test
echo 
echo "Premi un tasto per iniziare il test di invio e ricezione di pacchetti multipli..."
echo 
read -n 1 -s -r
test_multiple_packages

# Attende che l'utente prema un tasto per iniziare il test successivo
echo 
echo "Premi un tasto per continuare con il test di gestione delle versioni dei pacchetti..."
echo 
read -n 1 -s -r
test_version_management

# Attende che l'utente prema un tasto per iniziare il test successivo
echo 
echo "Premi un tasto per continuare con il test di controllo della versione con caricamento ripetuto..."
echo 
read -n 1 -s -r
test_version_control_repeat

# Attende che l'utente prema un tasto per iniziare il test successivo
echo 
echo "Premi un tasto per continuare con il test di accesso a una risorsa condivisa..."
echo 
read -n 1 -s -r
test_shared_resource_access

# Attende che l'utente prema un tasto per iniziare il test successivo
echo 
echo "Premi un tasto per continuare con il test di accesso concorrente di più client al server..."
echo 
read -n 1 -s -r
test_concurrent_access

# Attende che l'utente prema un tasto per iniziare il test di upload e download concorrente
echo 
echo "Premi un tasto per continuare con il test di upload e download concorrente..."
echo 
read -n 1 -s -r
test_concurrent_upload_download

echo 
echo "Tutti i test sono stati completati."

