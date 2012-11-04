#ifndef __JSON_H
#define __JSON_H

/*
	libjson - a C library to parse and construct JSON data structures.

	Copyright (C) 2012 onwards  Attie Grande (attie@attie.co.uk)

	libjson is free software: you can redistribute it and/or modify it
	under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libjson is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EXPORT
#define EXPORT
#endif

struct json;

enum json_errors {
	JSON_ENONE = 0,	
	JSON_EUNKNOWN = -1,

	JSON_ENOMEM = -2,

	JSON_ETYPEMISMATCH = -3,

	JSON_EINCOMPLETE = -4,
	JSON_ECOMPLETE = -5,
};
typedef enum json_errors json_err;

enum json_dataTypes {
	JSON_MISSING = -1,
	JSON_NULL,
	JSON_INTEGER,
	JSON_FLOAT,
	JSON_STRING,
	JSON_FUNCTION,
	JSON_OBJECT,
	JSON_ARRAY,
};

struct json *json_new(void);
void json_destroy(struct json *json);

json_err json_isComplete (struct json *json);
json_err json_dataAdd    (struct json *json, unsigned char *data, unsigned int len);

json_err json_getType    (struct json *json, char *identifier, enum json_dataTypes *type);
json_err json_getChildren(struct json *json, char *identifier, char **children);

json_err json_getInteger (struct json *json, char *identifier, int *data);
json_err json_getFloat   (struct json *json, char *identifier, float *data);
json_err json_getString  (struct json *json, char *identifier, char **data);
json_err json_getFunction(struct json *json, char *identifier, char **data);
json_err json_getArrayLen(struct json *json, char *identifier, unsigned int *length);

#endif /* __JSON_H */
