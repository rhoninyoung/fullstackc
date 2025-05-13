#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

sqlite3 *db;

void init_db() {
    char *err_msg = 0;
    int rc = sqlite3_open("direction.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS directions("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "direction CHAR(1),"
                      "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    // Insert default direction if table is empty
    sql = "INSERT INTO directions(direction) SELECT 'u' WHERE NOT EXISTS(SELECT 1 FROM directions);";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
}

char get_current_direction() {
    sqlite3_stmt *res;
    char direction = 'u';
    const char *sql = "SELECT direction FROM directions ORDER BY id DESC LIMIT 1;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        while ((rc = sqlite3_step(res)) == SQLITE_ROW) {
            direction = sqlite3_column_text(res, 0)[0];
        }
    } else {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);
    return direction;
}

void set_current_direction(char direction) {
    sqlite3_stmt *res;
    const char *sql = "INSERT INTO directions(direction) VALUES (?);";

    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(res, 1, &direction, 1, SQLITE_STATIC);
        rc = sqlite3_step(res);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Failed to insert data: %s\n", sqlite3_errmsg(db));
        }
    } else {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);
}

void get_history_directions(json_object *response_json) {
    sqlite3_stmt *res;
    const char *sql = "SELECT direction, timestamp FROM directions ORDER BY id ASC;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        json_object *history_array = json_object_new_array();
        while ((rc = sqlite3_step(res)) == SQLITE_ROW) {
            json_object *entry = json_object_new_object();
            json_object_object_add(entry, "direction", json_object_new_string((const char *)sqlite3_column_text(res, 0)));
            json_object_object_add(entry, "timestamp", json_object_new_string((const char *)sqlite3_column_text(res, 1)));
            json_object_array_add(history_array, entry);
        }
        json_object_object_add(response_json, "history", history_array);
    } else {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);
}