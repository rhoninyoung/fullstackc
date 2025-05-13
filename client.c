#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <json-c/json.h>
#include <ctype.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void send_post_request(char direction) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    json_object *request_json = json_object_new_object();
    json_object_object_add(request_json, "direction", json_object_new_string(&direction));

    char post_request[BUFFER_SIZE];
    snprintf(post_request, sizeof(post_request),
             "POST /direction HTTP/1.1\r\n"
             "Host: localhost:%d\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s", PORT, strlen(json_object_to_json_string(request_json)), json_object_to_json_string(request_json));

    send(sock, post_request, strlen(post_request), 0);
    printf("Sending direction: %c\n", direction);

    close(sock);
    json_object_put(request_json);
}

char get_current_direction() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return '\0';
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return '\0';
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return '\0';
    }

    char get_request[] = "GET /direction HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
    send(sock, get_request, strlen(get_request), 0);

    int valread = read(sock, buffer, BUFFER_SIZE);
    if (valread <= 0) {
        close(sock);
        return '\0';
    }

    // Find the start of the response body
    char *body_start = strstr(buffer, "\r\n\r\n");
    if (body_start != NULL) {
        body_start += 4; // Move past \r\n\r\n
        json_object *response_json = json_tokener_parse(body_start);
        json_object *dir_obj;
        if (json_object_object_get_ex(response_json, "direction", &dir_obj)) {
            char direction = json_object_get_string(dir_obj)[0];
            close(sock);
            json_object_put(response_json);
            return direction;
        }
        json_object_put(response_json);
    }

    close(sock);
    return '\0';
}

void get_history_directions() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    char get_request[] = "GET /history HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
    send(sock, get_request, strlen(get_request), 0);

    int valread = read(sock, buffer, BUFFER_SIZE);
    if (valread <= 0) {
        close(sock);
        return;
    }

    // Find the start of the response body
    char *body_start = strstr(buffer, "\r\n\r\n");
    if (body_start != NULL) {
        body_start += 4; // Move past \r\n\r\n
        json_object *response_json = json_tokener_parse(body_start);
        json_object *history_array;
        if (json_object_object_get_ex(response_json, "history", &history_array)) {
            int array_len = json_object_array_length(history_array);
            printf("History of directions:\n");
            for (int i = 0; i < array_len; i++) {
                json_object *entry = json_object_array_get_idx(history_array, i);
                json_object *dir_obj, *timestamp_obj;
                if (json_object_object_get_ex(entry, "direction", &dir_obj) &&
                    json_object_object_get_ex(entry, "timestamp", &timestamp_obj)) {
                    printf("Direction: %s, Timestamp: %s\n", json_object_get_string(dir_obj), json_object_get_string(timestamp_obj));
                }
            }
        }
        json_object_put(response_json);
    }

    close(sock);
}

void draw_arrow(char direction) {
    switch(direction) {
        case 'u':
            printf("   ^\n");
            printf("   |\n");
            break;
        case 'd':
            printf("   |\n");
            printf("   v\n");
            break;
        case 'l':
            printf("<-\n");
            break;
        case 'r':
            printf("->\n");
            break;
        default:
            printf("Unknown direction\n");
            break;
    }
}

void frontend_terminal() {
    char input;
    while(1) {
        printf("Enter direction (u/d/l/r), or type 'h' for history: ");
        fflush(stdout); // Ensure prompt is displayed immediately
        scanf(" %c", &input);
        input = tolower(input);
        printf("debug: %c\n", input);
        if (input == 'h') {
            get_history_directions();
        } else if (input == 'u' || input == 'd' || input == 'l' || input == 'r') {
            send_post_request(input);
            char current_direction = get_current_direction();
            printf("Current direction: %c\n", current_direction);
            draw_arrow(current_direction);
            printf("\n"); // Add a newline for better readability
        } else {
            printf("Invalid input. Please enter u, d, l, r, or h.\n");
        }
    }
}

int main() {
    frontend_terminal();
    return 0;
}