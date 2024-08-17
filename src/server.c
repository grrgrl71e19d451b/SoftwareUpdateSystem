#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <glob.h>
#include <sys/file.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../include/common.h"
#include "../include/logger.h"

#define PORT 8080
#define MAX_CLIENTS 10

typedef struct {
    int socket;
    struct sockaddr_in address;
} Client;

// Crea una directory se non esiste
void create_directory_if_not_exists(const char *dir_path) {
    struct stat st = {0};
    if (stat(dir_path, &st) == -1) {
        if (mkdir(dir_path, 0700) == -1) {
            perror("Errore nella creazione della directory");
            exit(EXIT_FAILURE);
        }
    }
}

// Gestisce le richieste del client
void handle_client(Client *client) {
    int sock = client->socket;
    struct sockaddr_in addr = client->address;
    free(client);

    pid_t client_pid = getpid();
    int operation;
    recv(sock, &operation, sizeof(int), 0);

    if (operation == 1) { // Upload del pacchetto
        Package package;
        recv(sock, package.name, MAX_PACKAGE_NAME, 0);
        recv(sock, package.version, MAX_PACKAGE_VERSION, 0);
        recv(sock, &package.content_size, sizeof(int), 0);

        package.content = malloc(package.content_size);
        if (package.content == NULL) {
            perror("Errore di allocazione della memoria");
            close(sock);
            return;
        }
        recv(sock, package.content, package.content_size, 0);

        char file_path[512];
        snprintf(file_path, sizeof(file_path), "server_package/%s_%s.zip", package.name, package.version);

        char existing_file_pattern[512];
        snprintf(existing_file_pattern, sizeof(existing_file_pattern), "server_package/%s_*.zip", package.name);

        glob_t glob_result;
        glob(existing_file_pattern, 0, NULL, &glob_result);

        int replace_package = 1;

        if (glob_result.gl_pathc > 0) {
            char *existing_file_path = glob_result.gl_pathv[0];
            char *existing_version_start = strrchr(existing_file_path, '_') + 1;
            char *existing_version_end = strrchr(existing_file_path, '.');
            char existing_version[MAX_PACKAGE_VERSION];
            strncpy(existing_version, existing_version_start, existing_version_end - existing_version_start);
            existing_version[existing_version_end - existing_version_start] = '\0';

            if (compare_versions(package.version, existing_version) <= 0) {
                replace_package = 0;
            } else {
                remove(existing_file_path);
            }
        }
        globfree(&glob_result);

        if (replace_package) {
            int file_desc = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (file_desc != -1) {
                if (flock(file_desc, LOCK_EX) == 0) {
                    FILE *file = fdopen(file_desc, "wb");
                    if (file != NULL) {
                        fwrite(package.content, 1, package.content_size, file);
                        fclose(file);
                        log_operation("Upload", package.name, package.version, &addr, client_pid);  // Chiamata alla funzione di logging
                        int response = 1; // Upload accettato
                        send(sock, &response, sizeof(int), 0); // Conferma al client
                    } else {
                        perror("Impossibile salvare il pacchetto");
                    }
                    flock(file_desc, LOCK_UN);
                } else {
                    perror("Impossibile acquisire il lock sul file");
                }
                close(file_desc);
            } else {
                perror("Impossibile aprire il file per scrittura");
            }
        } else {
            log_existing_package(package.name, package.version, &addr, client_pid);  // Chiamata alla funzione di logging
            int response = 0; // Upload rifiutato
            send(sock, &response, sizeof(int), 0); // Rifiuto al client
        }

        free(package.content);
    } else if (operation == 2) { // Download del pacchetto
        Package package;
        recv(sock, package.name, MAX_PACKAGE_NAME, 0);
        recv(sock, package.version, MAX_PACKAGE_VERSION, 0);

        char file_path[512];
        snprintf(file_path, sizeof(file_path), "server_package/%s_%s.zip", package.name, package.version);

        FILE *file = fopen(file_path, "rb");
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            package.content_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            package.content = malloc(package.content_size);
            if (package.content == NULL) {
                perror("Errore di allocazione della memoria");
                fclose(file);
                close(sock);
                return;
            }
            fread(package.content, 1, package.content_size, file);
            fclose(file);

            send(sock, &package.content_size, sizeof(int), 0);
            send(sock, package.content, package.content_size, 0);

            free(package.content);
            log_operation("Download", package.name, package.version, &addr, client_pid);  // Chiamata alla funzione di logging
        } else {
            package.content_size = 0;
            send(sock, &package.content_size, sizeof(int), 0);
            log_operation("Pacchetto non trovato", package.name, package.version, &addr, client_pid);  // Chiamata alla funzione di logging
        }
    }

    close(sock);
}

int main() {
    // Crea le directory necessarie
    create_directory_if_not_exists("logs");
    create_directory_if_not_exists("server_package");
    create_directory_if_not_exists("client_download_package");
    create_directory_if_not_exists("client_upload_package");

    setvbuf(stdout, NULL, _IONBF, 0);

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind fallito");
        exit(EXIT_FAILURE);
    }
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen fallito");
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto sulla porta %d...\n", PORT);

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len)) != -1) {
        Client *client = malloc(sizeof(Client));
        client->socket = client_sock;
        client->address = client_addr;

        if (fork() == 0) { // Processo figlio
            close(server_sock);
            handle_client(client);
            exit(EXIT_SUCCESS);
        } else {
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}
