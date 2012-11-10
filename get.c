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

EXPORT json_err json_getType(struct json *json, unsigned char *identifier, enum json_dataTypes *type) {
	json_err ret;
	struct json_object *target;

	if (!json || !json->head || !identifier) return JSON_EMISSINGPARAM;
	if ((ret = json_locateObject(json->head, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;

	*type = target->type;

	return JSON_ENONE;
}

EXPORT json_err json_getInteger(struct json *json, unsigned char *identifier, int *data) {
	json_err ret;
	struct json_object *target;

	if (!json || !json->head || !identifier || !data) return JSON_EMISSINGPARAM;
	if ((ret = json_locateObject(json->head, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;
	if (target->type != JSON_INTEGER) return JSON_ETYPEMISMATCH;

	*data = target->data.asInt;

	return JSON_ENONE;
}

EXPORT json_err json_getFloat(struct json *json, unsigned char *identifier, float *data) {
	json_err ret;
	struct json_object *target;

	if (!json || !json->head || !identifier || !data) return JSON_EMISSINGPARAM;
	if ((ret = json_locateObject(json->head, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;
	if (target->type != JSON_FLOAT) return JSON_ETYPEMISMATCH;

	*data = target->data.asFloat;

	return JSON_ENONE;
}

EXPORT json_err json_getString(struct json *json, unsigned char *identifier, unsigned char **data, unsigned int *dataLen) {
	json_err ret;
	struct json_object *target;

	if (!json || !json->head || !identifier || !data || !dataLen) return JSON_EMISSINGPARAM;
	if ((ret = json_locateObject(json->head, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;
	if (target->type != JSON_STRING) return JSON_ETYPEMISMATCH;

	*data = target->data.asRaw;
	*dataLen = target->data_len;

	return JSON_ENONE;
}

EXPORT json_err json_getFunction(struct json *json, unsigned char *identifier, unsigned char **data, unsigned int *dataLen) {
	return JSON_ENOTIMPLEMENTED;
}

EXPORT json_err json_getArrayLen(struct json *json, unsigned char *identifier, unsigned int *length) {
	json_err ret;
	struct json_object *target;
	struct json_object *child;

	if (!json || !json->head || !identifier || !length) return JSON_EMISSINGPARAM;
	if ((ret = json_locateObject(json->head, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;
	if (target->type != JSON_ARRAY) return JSON_ETYPEMISMATCH;

	for (child = target->child_head; child && child->sibling_prev; child = child->sibling_prev);
	for ((*length) = 0; child; (*length)++, child = child->sibling_next);

	return JSON_ENONE;
}

EXPORT json_err json_getObject(struct json *json, unsigned char *identifier, struct json_object **targetRet) {
	json_err ret;
	struct json_object *target;

	if (!json || !json->head || !identifier) return JSON_EMISSINGPARAM;
	if ((ret = json_locateObject(json->head, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;

	if (targetRet) *targetRet = target;

	return JSON_ENONE;
}

