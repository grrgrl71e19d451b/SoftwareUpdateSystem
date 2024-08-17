#ifndef COMMON_H
#define COMMON_H

// Dimensioni massime per nome e versione del pacchetto
#define MAX_PACKAGE_NAME 256
#define MAX_PACKAGE_VERSION 64

// Struttura che rappresenta un pacchetto
typedef struct {
    char name[MAX_PACKAGE_NAME];       // Nome del pacchetto
    char version[MAX_PACKAGE_VERSION]; // Versione del pacchetto
    int content_size;                  // Dimensione del contenuto del pacchetto
    char *content;                     // Contenuto del pacchetto
} Package;

// Confronta due versioni di pacchetti
int compare_versions(const char *v1, const char *v2);

// Estrae il nome e la versione dal percorso del file
void extract_name_and_version(const char *filepath, char *name, char *version);

#endif // COMMON_H
