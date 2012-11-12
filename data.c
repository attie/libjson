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
#include "get.h"

EXPORT json_err json_dataGet(struct json_element *root, unsigned char *identifier, void **user_data) {
	json_err ret;
	struct json_element *target;

	if (!root || !identifier || !user_data) return JSON_EMISSINGPARAM;
	if ((ret = json_getElement(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;

	*user_data = target->user_data;

	return JSON_ENONE;
}

EXPORT json_err json_dataSet(struct json_element *root, unsigned char *identifier, void *user_data) {
	json_err ret;
	struct json_element *target;

	if (!root || !identifier || !user_data) return JSON_EMISSINGPARAM;
	if ((ret = json_getElement(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;

	target->user_data = user_data;

	return JSON_ENONE;
}
