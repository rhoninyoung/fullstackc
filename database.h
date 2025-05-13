#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <json-c/json.h>

extern sqlite3 *db; // Declare db as an external variable

void init_db();
char get_current_direction();
void set_current_direction(char direction);
void get_history_directions(json_object *response_json);

#endif // DATABASE_H