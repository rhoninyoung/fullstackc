#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <json-c/json.h>
#include "database.h"

#define PORT 8888
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread <= 0) {
        close(client_socket);
        return;
    }

    printf("Received request:\n%s\n", buffer);

    if (strstr(buffer, "POST /direction")) {
        // Extract the direction from the POST body
        char *body_start = strstr(buffer, "\r\n\r\n");
        if (body_start != NULL) {
            body_start += 4; // Move past \r\n\r\n
            json_object *parsed_json = json_tokener_parse(body_start);
            json_object *dir_obj;
            if (json_object_object_get_ex(parsed_json, "direction", &dir_obj)) {
                char new_direction = json_object_get_string(dir_obj)[0];
                set_current_direction(new_direction);
                printf("Received new direction: %c\n", new_direction);
            }
            json_object_put(parsed_json);
        }
    } else if (strstr(buffer, "GET /direction")) {
        // Send the current direction as response in JSON format
        char current_direction = get_current_direction();
        json_object *response_json = json_object_new_object();
        json_object_object_add(response_json, "direction", json_object_new_string(&current_direction));

        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %zu\r\n"
                 "\r\n"
                 "%s",
                 strlen(json_object_to_json_string(response_json)),
                 json_object_to_json_string(response_json));
        write(client_socket, response, strlen(response));
        printf("Returning current direction: %c\n", current_direction);
        json_object_put(response_json);
    } else if (strstr(buffer, "GET /history")) {
        // Send the history of directions as response in JSON format
        json_object *response_json = json_object_new_object();
        get_history_directions(response_json);

        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %zu\r\n"
                 "\r\n"
                 "%s",
                 strlen(json_object_to_json_string(response_json)),
                 json_object_to_json_string(response_json));
        write(client_socket, response, strlen(response));
        printf("Returning history of directions\n");
        json_object_put(response_json);
    }

    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Initialize database
    init_db();

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Binding to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        handle_client(new_socket);
    }

    close(server_fd);
    sqlite3_close(db);
    return 0;
}