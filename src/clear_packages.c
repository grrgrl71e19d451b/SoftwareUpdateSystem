#include <stdio.h>
#include <stdlib.h>

void clear_directory(const char *dir) {
    char command[512];
    // Comando per svuotare la directory
    snprintf(command, sizeof(command), "rm -rf %s/*", dir);
    // Esegue il comando
    int result = system(command);
    // Verifica il risultato dell'operazione
    if (result == 0) {
        printf("La cartella %s è stata svuotata.\n", dir);
    } else {
        printf("Errore nel svuotare la cartella %s.\n", dir);
    }
}

int main() {
    const char *client_download_dir = "client_download_package";  // Cartella client
    const char *server_package_dir = "server_package";            // Cartella server

    // Svuota le cartelle specificate
    clear_directory(client_download_dir);
    clear_directory(server_package_dir);

    return 0;
}
