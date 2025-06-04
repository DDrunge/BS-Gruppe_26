#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int socket_fd, connection_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    ssize_t number_bytes;

    // Standardausgabe sofort anzeigen (kein stdout-Puffer)
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("Server startet...\n");

    // Socket erstellen
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket konnte nicht erstellt werden");
        return 1;
    }

    // Server-Adresse konfigurieren
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5678);

    // Socket an Adresse binden
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind fehlgeschlagen");
        return 1;
    }

    // Auf eingehende Verbindungen warten
    if (listen(socket_fd, 5) < 0) {
        perror("Listen fehlgeschlagen");
        return 1;
    }

    printf("Warte auf Verbindungen auf Port 5678...\n");

    // Verbindung akzeptieren
    connection_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
    if (connection_fd < 0) {
        perror("Accept fehlgeschlagen");
        return 1;
    }

    printf("Client verbunden.\n");

    // Echo-Schleife
    while (1) {
        number_bytes = read(connection_fd, buffer, sizeof(buffer) - 1);
        if (number_bytes <= 0) {
            printf("Client getrennt.\n");
            break;
        }

        buffer[number_bytes] = '\0'; // Null-terminieren

        printf("Vom Client empfangen: %s\n", buffer);

        // Prüfe auf "exit" oder "close"
        if (strncmp(buffer, "exit", 4) == 0 || strncmp(buffer, "close", 5) == 0) {
            printf("Beende Verbindung aufgrund von Befehl.\n");
            break;
        }

        // Echo zurücksenden
        if (write(connection_fd, buffer, strlen(buffer)) < 0) {
            perror("Fehler beim Senden");
            break;
        }
    }


    close(connection_fd);
    close(socket_fd);
    return 0;
}
