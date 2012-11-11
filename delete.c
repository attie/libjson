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

EXPORT json_err json_deleteElement(struct json_object *root, unsigned char *identifier) {
	json_err ret;
	struct json_object *target;

	if (!root || !identifier) return JSON_EMISSINGPARAM;
	if ((ret = json_getObject(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;

	if (target->parent && target->parent->child_head == target) {
		/* in this case, we must relink the parent to a sibling */
		if (target->sibling_prev) {
			target->parent->child_head = target->sibling_prev;
		} else {
			target->parent->child_head = target->sibling_next;
		}
		target->parent = NULL;
	}

	if (target->sibling_prev) {
		target->sibling_prev->sibling_next = target->sibling_next;
		target->sibling_prev = NULL;
	}

	if (target->sibling_next) {
		target->sibling_next->sibling_prev = target->sibling_prev;
		target->sibling_next = NULL;
	}

	/* we are now completely un-linked... destroy us and all our children */
	return json_objectDestroy(target);
}
