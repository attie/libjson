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
struct json_object;

enum json_errors {
	JSON_ENONE = 0,	
	JSON_EUNKNOWN = -1,
	JSON_EMISSINGPARAM = -2,
	JSON_ENOMEM = -3,

	JSON_ETYPEMISMATCH = -4,

	JSON_EMISSING = -5,
	JSON_EINCOMPLETE = -6,
	JSON_ECOMPLETE = -7,
	JSON_EINVAL = -8,

	JSON_ENOTIMPLEMENTED = -9,

	/* this error occurs when you try to add a named item to an array - you should be using an object */
	JSON_EPARENTISARRAY = -10,

	JSON_EEXISTS = -11,
};
typedef enum json_errors json_err;

enum json_dataTypes {
	JSON_MISSING = 0,
	JSON_ELEMENT,
	JSON_NULL,
	JSON_INTEGER,
	JSON_FLOAT,
	JSON_STRING,
	JSON_FUNCTION,
	JSON_OBJECT,
	JSON_ARRAY,
};

EXPORT json_err json_new(struct json **json, struct json_object **root);
EXPORT json_err json_destroy(struct json *json);
EXPORT json_err json_isComplete (struct json *json);
EXPORT json_err json_dataAdd    (struct json *json, unsigned char *data, unsigned int len);

EXPORT json_err json_addInteger (struct json_object *root, unsigned char *parent, unsigned char *name, int data);
EXPORT json_err json_addFloat   (struct json_object *root, unsigned char *parent, unsigned char *name, float data);
EXPORT json_err json_addString  (struct json_object *root, unsigned char *parent, unsigned char *name, unsigned char *data, unsigned int dataLen);
EXPORT json_err json_addFunction(struct json_object *root, unsigned char *parent, unsigned char *name, unsigned char *data, unsigned int dataLen);
EXPORT json_err json_addObject  (struct json_object *root, unsigned char *parent, unsigned char *name, struct json_object **object);
EXPORT json_err json_addArray   (struct json_object *root, unsigned char *parent, unsigned char *name, struct json_object **object);

EXPORT json_err json_getType    (struct json_object *root, unsigned char *identifier, enum json_dataTypes *type);
EXPORT json_err json_getChildren(struct json_object *root, unsigned char *identifier, unsigned char **children);

EXPORT json_err json_getInteger (struct json_object *root, unsigned char *identifier, int *data);
EXPORT json_err json_getFloat   (struct json_object *root, unsigned char *identifier, float *data);
EXPORT json_err json_getString  (struct json_object *root, unsigned char *identifier, unsigned char **data, unsigned int *dataLen);
EXPORT json_err json_getFunction(struct json_object *root, unsigned char *identifier, unsigned char **data, unsigned int *dataLen);
EXPORT json_err json_getArrayLen(struct json_object *root, unsigned char *identifier, unsigned int *length);
EXPORT json_err json_getObject  (struct json_object *root, unsigned char *identifier, struct json_object **target);

EXPORT json_err json_print(struct json *json, unsigned char **output, unsigned int *outputLen);
EXPORT json_err json_printObject(struct json_object *root, unsigned char **output, unsigned int *outputLen);

#endif /* __JSON_H */
