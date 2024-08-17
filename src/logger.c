#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// Scrive un messaggio di log su file
void log_to_file(const char *filename, const char *log_entry) {
    FILE *file = fopen(filename, "a");
    if (file) {
        fprintf(file, "%s\n", log_entry);
        fflush(file); // Forza la scrittura immediata
        fclose(file);
    } else {
        perror("Errore nell'apertura del file di log");
    }
}

// Registra un'operazione di upload o download
void log_operation(const char *operation, const char *package_name, const char *package_version, struct sockaddr_in *client_addr, pid_t client_pid) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr->sin_port);

    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "[%s:%d] Processo %d: %s: %s %s", client_ip, client_port, client_pid, operation, package_name, package_version);

    printf("%s\n", log_entry);
    fflush(stdout); // Forza la scrittura immediata a video

    log_to_file("logs/server_log.log", log_entry);
}

// Registra che una versione più recente del pacchetto esiste già
void log_existing_package(const char *package_name, const char *package_version, struct sockaddr_in *client_addr, pid_t client_pid) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_addr->sin_port);

    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "[%s:%d] Processo %d: Una versione più recente del pacchetto %s %s esiste già", client_ip, client_port, client_pid, package_name, package_version);

    printf("%s\n", log_entry);
    fflush(stdout); // Forza la scrittura immediata a video

    log_to_file("logs/server_log.log", log_entry);
}
