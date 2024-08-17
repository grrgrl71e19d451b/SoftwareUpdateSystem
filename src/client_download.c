#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../include/common.h"
#include "../include/logger.h"

#define PORT 8080
#define FILE_PATH_MAX 512

// Scarica il pacchetto dal server
void download_package(int sock, const char *filepath) {
    Package package;
    char name[MAX_PACKAGE_NAME];
    char version[MAX_PACKAGE_VERSION];

    extract_name_and_version(filepath, name, version);

    int operation = 2;
    if (send(sock, &operation, sizeof(int), 0) < 0 ||
        send(sock, name, MAX_PACKAGE_NAME, 0) < 0 ||
        send(sock, version, MAX_PACKAGE_VERSION, 0) < 0) {
        char log_entry[1024];
        snprintf(log_entry, sizeof(log_entry), "Processo %d: Errore nell'invio dei dati", getpid());
        printf("%s\n", log_entry);
        log_to_file("logs/client_download_log.log", log_entry);
        return;
    }

    if (recv(sock, &package.content_size, sizeof(int), 0) <= 0) {
        char log_entry[1024];
        snprintf(log_entry, sizeof(log_entry), "Processo %d: Errore nel ricevere la dimensione del contenuto", getpid());
        printf("%s\n", log_entry);
        log_to_file("logs/client_download_log.log", log_entry);
        return;
    }

    if (package.content_size > 0) {
        package.content = malloc(package.content_size);
        if (package.content == NULL) {
            char log_entry[1024];
            snprintf(log_entry, sizeof(log_entry), "Processo %d: Errore di allocazione della memoria", getpid());
            printf("%s\n", log_entry);
            log_to_file("logs/client_download_log.log", log_entry);
            return;
        }
        if (recv(sock, package.content, package.content_size, 0) <= 0) {
            char log_entry[1024];
            snprintf(log_entry, sizeof(log_entry), "Processo %d: Errore nel ricevere il contenuto", getpid());
            printf("%s\n", log_entry);
            free(package.content);
            log_to_file("logs/client_download_log.log", log_entry);
            return;
        }

        char client_file_path[FILE_PATH_MAX];
        snprintf(client_file_path, sizeof(client_file_path), "client_download_package/%s_%s.zip", name, version);
        FILE *file = fopen(client_file_path, "wb");
        if (file != NULL) {
            fwrite(package.content, 1, package.content_size, file);
            fclose(file);

            char log_entry[1024];
            snprintf(log_entry, sizeof(log_entry), "Processo %d: Pacchetto scaricato: %s %s", getpid(), name, version);
            printf("%s\n", log_entry);
            log_to_file("logs/client_download_log.log", log_entry);
        } else {
            char log_entry[1024];
            snprintf(log_entry, sizeof(log_entry), "Processo %d: Impossibile salvare il file: %s", getpid(), client_file_path);
            printf("%s\n", log_entry);
            log_to_file("logs/client_download_log.log", log_entry);
        }
        free(package.content);
    } else {
        char log_entry[1024];
        snprintf(log_entry, sizeof(log_entry), "Processo %d: Pacchetto non trovato: %s %s", getpid(), name, version);
        printf("%s\n", log_entry);
        log_to_file("logs/client_download_log.log", log_entry);
    }
}

// Legge e processa le istruzioni di download da un file
void process_download_instructions(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Impossibile aprire il file di istruzioni\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char filepath[512];
        if (sscanf(line, "download %s", filepath) == 1) {
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

            download_package(sock, filepath);
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

    setvbuf(stdout, NULL, _IONBF, 0);  // Disabilita il buffering per stdout

    process_download_instructions(argv[1]);
    return 0;
}
