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
#include "element.h"

EXPORT json_err json_new(struct json **jsonRet, struct json_element **rootRet) {
	json_err ret;
	struct json *json;
	struct json_element *root;

	if (!jsonRet) return JSON_EMISSINGPARAM;

	if ((ret = json_elementNew(&root)) != JSON_ENONE) return ret;
	if ((json = malloc(sizeof(*json))) == NULL) return JSON_ENOMEM;
	memset(json, 0, sizeof(*json));
	json->parse.err = JSON_ENONE;
	json->root = root;
	root->json = json;
	root->type = JSON_OBJECT;

	*jsonRet = json;
	if (rootRet) *rootRet = root;

	return JSON_ENONE;
}

EXPORT json_err json_destroy(struct json *json) {
	if (!json) return JSON_EMISSINGPARAM;
	
	if (json->parse.buf.data) free(json->parse.buf.data);
	if (json->root) json_elementDestroy(json->root);
	
	free(json);

	return JSON_ENONE;
}

EXPORT json_err json_getRoot(struct json *json, struct json_element **root) {
	if (!json || !root) return JSON_EMISSINGPARAM;
	*root = json->root;
	return JSON_ENONE;
}

EXPORT json_err json_isComplete(struct json *json) {
	return JSON_ENOTIMPLEMENTED;
}
