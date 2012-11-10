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

EXPORT json_err json_new(struct json **jsonRet) {
	json_err ret;
	struct json *json;
	struct json_object *head;

	if (!jsonRet) return JSON_EMISSINGPARAM;

	if ((ret = json_objectNew(&head)) != JSON_ENONE) return ret;
	if ((json = malloc(sizeof(*json))) == NULL) return JSON_ENOMEM;
	memset(json, 0, sizeof(*json));
	json->head = head;
	head->root = json;
	head->type = JSON_OBJECT;

	*jsonRet = json;

	return JSON_ENONE;
}

EXPORT json_err json_destory(struct json *json) {
	if (!json) return JSON_EMISSINGPARAM;
	
	if (json->data) free(json->data);
	if (json->head) json_objectDestroy(json->head);
	
	free(json);

	return JSON_ENONE;
}

