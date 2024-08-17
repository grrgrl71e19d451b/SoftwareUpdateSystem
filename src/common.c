#include "../include/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Confronta due versioni in formato "major.minor.patch"
int compare_versions(const char *v1, const char *v2) {
    int major1, minor1, patch1;
    int major2, minor2, patch2;

    sscanf(v1, "%d.%d.%d", &major1, &minor1, &patch1);
    sscanf(v2, "%d.%d.%d", &major2, &minor2, &patch2);

    if (major1 != major2) return major1 - major2;
    if (minor1 != minor2) return minor1 - minor2;
    return patch1 - patch2;
}

// Estrae il nome e la versione dal percorso del file
void extract_name_and_version(const char *filepath, char *name, char *version) {
    char *base_name = strrchr(filepath, '/');
    base_name = base_name ? base_name + 1 : (char *)filepath;

    char *underscore = strrchr(base_name, '_');
    char *dot = strrchr(base_name, '.');

    if (!underscore || !dot || underscore > dot) {
        strcpy(name, "unknown");
        strcpy(version, "unknown");
    } else {
        strncpy(name, base_name, underscore - base_name);
        name[underscore - base_name] = '\0';
        strncpy(version, underscore + 1, dot - underscore - 1);
        version[dot - underscore - 1] = '\0';
    }
}
