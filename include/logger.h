#ifndef LOGGER_H
#define LOGGER_H

#include <arpa/inet.h>
#include <sys/types.h>

// Funzione per scrivere una voce di log su file
void log_to_file(const char *filename, const char *log_entry);

// Funzione per registrare un'operazione (upload/download)
void log_operation(const char *operation, const char *package_name, const char *package_version, struct sockaddr_in *client_addr, pid_t client_pid);

// Funzione per registrare un pacchetto esistente
void log_existing_package(const char *package_name, const char *package_version, struct sockaddr_in *client_addr, pid_t client_pid);

#endif // LOGGER_H
