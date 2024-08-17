#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../include/common.h"
#include "../include/logger.h"

#define PORT 8080

// Carica un pacchetto al server
void upload_package(int sock, const char *filepath) {
    Package package;
    char name[MAX_PACKAGE_NAME];
    char version[MAX_PACKAGE_VERSION];

    extract_name_and_version(filepath, name, version);

    strcpy(package.name, name);
    strcpy(package.version, version);

    FILE *file = fopen(filepath, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        package.content_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        package.content = malloc(package.content_size);
        if (package.content == NULL) {
            printf("Processo %d: Errore di allocazione della memoria\n", getpid());
            fclose(file);
            return;
        }
        fread(package.content, 1, package.content_size, file);
        fclose(file);

        int operation = 1;
        if (send(sock, &operation, sizeof(int), 0) < 0 ||
            send(sock, package.name, MAX_PACKAGE_NAME, 0) < 0 ||
            send(sock, package.version, MAX_PACKAGE_VERSION, 0) < 0 ||
            send(sock, &package.content_size, sizeof(int), 0) < 0 ||
            send(sock, package.content, package.content_size, 0) < 0) {
            printf("Processo %d: Errore nell'invio dei dati\n", getpid());
            free(package.content);
            return;
        }

        free(package.content);

        int response;
        recv(sock, &response, sizeof(int), 0); // Ricevi la risposta del server
        char log_entry[512];
        if (response == 1) {
            snprintf(log_entry, sizeof(log_entry), "Processo %d: Pacchetto caricato: %s %s", getpid(), package.name, package.version);
            printf("%s\n", log_entry);
            log_to_file("logs/client_upload_log.log", log_entry);  // Chiamata alla funzione di logging
        } else {
            snprintf(log_entry, sizeof(log_entry), "Processo %d: Upload rifiutato per il pacchetto: %s %s (versione più recente esistente)", getpid(), package.name, package.version);
            printf("%s\n", log_entry);
            log_to_file("logs/client_upload_log.log", log_entry);  // Chiamata alla funzione di logging
        }
    } else {
        char log_entry[512];
        snprintf(log_entry, sizeof(log_entry), "Processo %d: Impossibile aprire il file: %s", getpid(), filepath);
        printf("%s\n", log_entry);
        log_to_file("logs/client_upload_log.log", log_entry);  // Chiamata alla funzione di logging
    }
}

// Processa le istruzioni di upload da un file
void process_upload_instructions(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Impossibile aprire il file di istruzioni\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char filepath[512];
        if (sscanf(line, "upload %s", filepath) == 1) {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                printf("Processo %d: Errore nella creazione del socket\n", getpid());
                continue;
            }
            struct sockaddr_in server_addr;
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(PORT);
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                printf("Processo %d: Errore nella connessione al server\n", getpid());
                close(sock);
                continue;
            }

            upload_package(sock, filepath);
            close(sock);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <file_di_istruzioni>\n", argv[0]);
        return 1;
    }

    // Disabilita il buffering per stdout
    setvbuf(stdout, NULL, _IONBF, 0);

    process_upload_instructions(argv[1]);
    return 0;
}
