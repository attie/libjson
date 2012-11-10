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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_int.h"
#include "object.h"
#include "add.h"

json_err json_addElement(struct json_object *root, unsigned char *parent, struct json_object **element, unsigned char *name) {
	json_err ret;
	struct json_object *target;
	struct json_object *object;
	unsigned char *name2;

	if (!root || !parent || !element) return JSON_EMISSINGPARAM;
	if ((ret = json_getObject(root, parent, &target)) != JSON_ENONE) return ret;
	switch (target->type) {
		case JSON_OBJECT:
			if (!name) return JSON_EMISSINGPARAM;
			if ((ret = json_getObject(target, name, NULL)) != JSON_EMISSING) return JSON_EEXISTS;
			if ((name2 = malloc(strlen((char*)name) + 1)) == NULL) return JSON_ENOMEM;
			strcpy((char*)name2, (char*)name);
			break;
		case JSON_ARRAY:
			if (name) return JSON_EPARENTISARRAY;
			name2 = NULL;
			break;
		default:
			return JSON_ETYPEMISMATCH;
	}

	if ((ret = json_objectNew(&object)) != JSON_ENONE) {
		if (name2) free(name2);
		return ret;
	}
	object->json = root->json;
	object->parent = target;
	object->name = name2;

	/* link it up */
	if (!target->child_head) {
		/* there are no children yet */
		target->child_head = object;
	} else {
		/* there are children... link it to the end */
		struct json_object *sibling;
		for (sibling = target->child_head; sibling && sibling->sibling_next; sibling = sibling->sibling_next);
		sibling->sibling_next = object;
		object->sibling_prev = sibling;
	}

	*element = object;

	return JSON_ENONE;
}

EXPORT json_err json_addInteger(struct json_object *root, unsigned char *parent, unsigned char *name, int data) {
	json_err ret;
	struct json_object *object;

	if (!root || !parent) return JSON_EMISSINGPARAM;
	if ((ret = json_addElement(root, parent, &object, name)) != JSON_ENONE) return ret;

	object->type = JSON_INTEGER;
	object->data.asInt = data;

	return JSON_ENONE;
}

EXPORT json_err json_addFloat(struct json_object *root, unsigned char *parent, unsigned char *name, float data) {
	json_err ret;
	struct json_object *object;

	if (!root || !parent) return JSON_EMISSINGPARAM;
	if ((ret = json_addElement(root, parent, &object, name)) != JSON_ENONE) return ret;

	object->type = JSON_FLOAT;
	object->data.asFloat = data;

	return JSON_ENONE;
}

EXPORT json_err json_addString(struct json_object *root, unsigned char *parent, unsigned char *name, unsigned char *data, unsigned int dataLen) {
	json_err ret;
	struct json_object *object;
	unsigned char *data2;

	if (!root || !parent || !data) return JSON_EMISSINGPARAM;
	if ((data2 = malloc(dataLen + 1)) == NULL) return JSON_ENOMEM;
	memcpy(data2, data, dataLen);
	data2[dataLen] = '\0';

	if ((ret = json_addElement(root, parent, &object, name)) != JSON_ENONE) {
		free(data2);
		return ret;
	}

	object->type = JSON_STRING;
	object->data_len = dataLen;
	object->data.asRaw = data2;

	return JSON_ENONE;
}

EXPORT json_err json_addFunction(struct json_object *root, unsigned char *parent, unsigned char *name, unsigned char *data, unsigned int dataLen) {
	return JSON_ENOTIMPLEMENTED;
}

EXPORT json_err json_addObject(struct json_object *root, unsigned char *parent, unsigned char *name, struct json_object **objectRet) {
	json_err ret;
	struct json_object *object;

	if (!root || !parent) return JSON_EMISSINGPARAM;
	if ((ret = json_addElement(root, parent, &object, name)) != JSON_ENONE) return ret;

	object->type = JSON_OBJECT;

	if (objectRet) *objectRet = object;

	return JSON_ENONE;
}

EXPORT json_err json_addArray(struct json_object *root, unsigned char *parent, unsigned char *name, struct json_object **objectRet) {
	json_err ret;
	struct json_object *object;

	if (!root || !parent) return JSON_EMISSINGPARAM;
	if ((ret = json_addElement(root, parent, &object, name)) != JSON_ENONE) return ret;

	object->type = JSON_ARRAY;

	if (objectRet) *objectRet = object;

	return JSON_ENONE;
}

