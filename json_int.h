#ifndef __JSON_INT_H
#define __JSON_INT_H

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

#define EXPORT __attribute__((visibility("default")))
#include "json.h"

struct json;
struct json_object;

struct json {
	unsigned char *data;
	unsigned int length;

	unsigned int parse_pos;
	struct json_object *parse_object;

	struct json_object *head;
};

struct json_object {
	struct json_object *parent;
	struct json_object *sibling_prev;
	struct json_object *sibling_next;
	struct json_object *child_head;

	unsigned char *name;

	enum json_dataTypes type;
	unsigned int data_len;
	union {
		unsigned char *asRaw;
		int asInt;
		float asFloat;
	} data;
};

json_err json_locateObject(struct json_object *root, char *identifier, struct json_object **target);

#endif /* __JSON_INT_H */
