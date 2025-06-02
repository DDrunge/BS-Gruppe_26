#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int socket_fd, connection_fd;
    int result, ret;
    int backlog = 5; //verbindungsanfragen in der warteschlange
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    ssize_t number_bytes;

    printf("Ok, dann wollen wir mal...\n");

    // Socket erstellen
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket konnte nicht erstellt werden!"); //perror print text + Fehlermeldung
        return 1;
    } else {
        printf("Socket erstellt\n");
    }

    // Adresse konfigurieren
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5678);  // lieber Port 8080 statt 22

    // Bind
    result = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (result < 0) {
        printf("Bind fehlgeschlagen");
        return 1;
    } else {
        printf("Bind erfolgreich\n");
    }

    // Listen
    ret = listen(socket_fd, backlog);
    if (ret < 0) {
        printf("Listen fehlgeschlagen");
        return 1;
    } else {
        printf("Listen für eingehende Verbindung\n");
    }

    // Accept
    connection_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
    if (connection_fd < 0) {
        printf("Accept fehlgeschlagen");
        return 1;
    } else {
        printf("Verbindung akzeptiert\n");
    }

    // Read
    number_bytes = read(connection_fd, buffer, sizeof(buffer));
    if (number_bytes < 0) {
        printf("Lesefehler");
        return 1;
    }
    buffer[number_bytes] = '\0'; // null-terminieren
    printf("Empfangen: %s\n", buffer);

    // Write
    number_bytes = write(connection_fd, buffer, number_bytes);
    if (number_bytes < 0) {
        perror("Fehler beim Schreiben");
        return 1;
    }
    printf("Antwort gesendet.\n");

    close(connection_fd);
    close(socket_fd);
    return 0;
}
