#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Elimina i file di log (.log) nella directory specificata
void clear_log_files(const char *dir) {
    char command[512];
    // Costruisce il comando per eliminare i file .log nella directory
    snprintf(command, sizeof(command), "rm -f %s/*.log", dir);
    // Esegue il comando
    int result = system(command);
    // Verifica il risultato dell'operazione
    if (result == 0) {
        printf("Tutti i file di log nella cartella %s sono stati eliminati.\n", dir);
    } else {
        printf("Errore nell'eliminare i file di log nella cartella %s.\n", dir);
    }
}

int main() {
    const char *logs_dir = "logs";  // Directory dei file di log

    // Elimina i file di log nella directory specificata
    clear_log_files(logs_dir);

    return 0;
}
