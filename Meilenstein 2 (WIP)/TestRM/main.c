#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_ENTRIES 64
#define MAX_KEY 64
#define MAX_VALUE 256

typedef struct {
    char key[MAX_KEY];
    char value[MAX_VALUE];
    int in_use;
} KeyValue;

KeyValue store[MAX_ENTRIES];

// Hilfsfunktion: Suche Schlüssel im Speicher
int find_key_index(const char *key) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (store[i].in_use && strcmp(store[i].key, key) == 0)
            return i;
    }
    return -1;
}

// Hilfsfunktion: Füge Schlüssel hinzu oder aktualisiere ihn
int put_key_value(const char *key, const char *value) {
    int index = find_key_index(key);
    if (index >= 0) {
        strncpy(store[index].value, value, MAX_VALUE);
        return 1; // aktualisiert
    }

    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (!store[i].in_use) {
            strncpy(store[i].key, key, MAX_KEY);
            strncpy(store[i].value, value, MAX_VALUE);
            store[i].in_use = 1;
            return 1; // hinzugefügt
        }
    }

    return 0; // Speicher voll
}

// DELETE
int delete_key(const char *key) {
    int index = find_key_index(key);
    if (index >= 0) {
        store[index].in_use = 0;
        return 1;
    }
    return 0;
}

int main() {
    int socket_fd, connection_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    ssize_t number_bytes;

    setvbuf(stdout, NULL, _IONBF, 0);

    printf("Server startet...\n");

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket konnte nicht erstellt werden");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5678);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind Fehler");
        return 1;
    }

    if (listen(socket_fd, 5) < 0) {
        perror("Listen Fehler");
        return 1;
    }

    printf("Warte auf Verbindungen auf Port 5678...\n");

    connection_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
    if (connection_fd < 0) {
        perror("Accept Fehler");
        return 1;
    }

    printf("Client verbunden.\n");

    while (1) {
        number_bytes = read(connection_fd, buffer, sizeof(buffer) - 1);
        if (number_bytes <= 0) {
            printf("Client getrennt.\n");
            break;
        }

        buffer[number_bytes] = '\0';

        // Entferne \r und \n am Ende
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[--len] = '\0';
        }

        //nötig um doppeltes enter zu vermeiden...
        if (strlen(buffer) == 0) {
            continue; // Eingabezeile war leer (nur Enter) -> überspringen
        }

        printf("Vom Client empfangen: %s\n", buffer);


        char command[10], key[MAX_KEY], value[MAX_VALUE];
        memset(command, 0, sizeof(command));
        memset(key, 0, sizeof(key));
        memset(value, 0, sizeof(value));

        sscanf(buffer, "%s %s %[^\n]", command, key, value);

        if (strcasecmp(command, "PUT") == 0) {
            if (strlen(key) == 0 || strlen(value) == 0) {
                write(connection_fd, "FEHLER: Befehl unvollständig, syntax ist: PUT <key> <value>\n", 27);
            } else if (put_key_value(key, value)) {
                write(connection_fd, "Gespeichert\n", 17);
            } else {
                write(connection_fd, "FEHLER: Kein Speicherplatz mehr!\n", 23);
            }
        } else if (strcasecmp(command, "GET") == 0) {
            int idx = find_key_index(key);
            if (idx >= 0) {
                char response[512];
                snprintf(response, sizeof(response), "WERT: %s\n", store[idx].value);
                write(connection_fd, response, strlen(response));
            } else {
                write(connection_fd, "FEHLER: Key nicht gefunden\n", 34);
            }
        } else if (strcasecmp(command, "DELETE") == 0) {
            if (delete_key(key)) {
                write(connection_fd, "Gelöscht\n", 13);
            } else {
                write(connection_fd, "FEHLER: Key nicht gefunden\n", 34);
            }
        } else if (strcasecmp(command, "QUIT") == 0) {
            write(connection_fd, "Verbindung wird beendet...\n", 28);
            break;
        } else {
            write(connection_fd, "Befehl nicht bekannt\n", 20);
        }
    }

    close(connection_fd);
    close(socket_fd);
    return 0;
}
