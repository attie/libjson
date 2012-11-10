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

json_err json_objectNew(struct json_object **objectRet) {
	struct json_object *object;

	if (!objectRet) return JSON_EMISSINGPARAM;

	if ((object = malloc(sizeof(*object))) == NULL) return JSON_ENOMEM;
	memset(object, 0, sizeof(*object));

	*objectRet = object;

	return JSON_ENONE;
}

json_err json_objectDestroy(struct json_object *object) {
	if (!object) return JSON_EMISSINGPARAM;

	/* destroy all the children */
	if (object->child_head) json_objectDestroy(object->child_head);

	/* remove each sibling to either side */
	if (object->sibling_prev) {
		object->sibling_prev->sibling_next = NULL;
		json_objectDestroy(object->sibling_prev);
	}
	if (object->sibling_next) {
		object->sibling_next->sibling_prev = NULL;
		json_objectDestroy(object->sibling_next);
	}

	/* finally, destroy us */
	if (object->name) free(object->name);
	switch (object->type) {
		case JSON_STRING:
		case JSON_FUNCTION:
			if (object->data.asRaw) free(object->data.asRaw);
		default:;
	}
	free(object);
	
	return JSON_ENONE;
}

json_err json_identifyAsArray(unsigned char *identifier, unsigned char **identifierStart, unsigned char **identifierEnd, enum identifierType *idType) {
	unsigned char *t;
	unsigned char *startOfWord;
	unsigned char *lastNonSpace;
	unsigned int depth;
	unsigned int numbers;
	unsigned int words;
	unsigned int dots;
	unsigned int nonNumeric;
	unsigned int canBeIdentifier;
	enum characterType charType;
	
	*idType = ID_INVALID;
	
	t = identifier;
	
	/* skip over the white space */
	for (; *t == ' '; t++);
	
	*identifierStart = t;
	*identifierEnd = NULL;
	
	startOfWord = t;
	lastNonSpace = NULL;
	depth = 0;
	numbers = 0;
	words = 1;
	dots = 0;
	nonNumeric = 0;
	canBeIdentifier = 1;
	charType = CHAR_NONE;
	
	/* find the end */
	for (; *t != '\0'; t++) {
		/* track the braces (only break for the top level) */
		if (*t == '[') {
			depth++;
		} else if (*t == ']') {
			if (!depth) break;
			depth--;
			lastNonSpace = t;
			continue;
		}
		
		/* if we are inside braces, just keep going */
		if (depth) continue;
		
		/* keep hold of the start of word (determines if this can be an identifier) */
		if ((charType == CHAR_SPACE || charType == CHAR_DOT) && *t != ' ') startOfWord = t;
		if (*t != ' ') lastNonSpace = t;
		
		switch (*t) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				if (t == startOfWord) canBeIdentifier = 0;
				if (charType != CHAR_NUMERIC) {
					charType = CHAR_NUMERIC;
					numbers++;
				}
				break;
			case ' ':
				if (charType != CHAR_SPACE) charType = CHAR_SPACE;
				break;
			case '.':
				if (charType == CHAR_DOT) return JSON_EINVAL;
				charType = CHAR_DOT;
				dots++;
				break;
			default:
				if (charType == CHAR_SPACE || charType == CHAR_DOT) words++;
				charType = CHAR_NONNUMERIC;
				nonNumeric++;
				break;
		}
	}
	
	/* sanity check the identifier */
	if (depth || *t != ']') return JSON_EINVAL;
	*identifierEnd = lastNonSpace;
	if (*identifierEnd < *identifierStart) return JSON_EINVAL;

	if (numbers == 1 && words == 1 && nonNumeric == 0) {
		*idType = ID_NUMBER;
	} else if (canBeIdentifier && dots + 1 == words) {
		*idType = ID_IDENTIFIER;
	} else {
		return JSON_EINVAL;
	}
	
	return JSON_ENONE;
}
json_err json_identifyAsElement(unsigned char *identifier, unsigned char **identifierStart, unsigned char **identifierEnd, enum identifierType *idType) {
	unsigned int hadSpace;
	unsigned char *t;
	
	*idType = ID_INVALID;
	
	t = identifier;
	
	/* skip over the white space */
	for (; *t == ' '; t++);
	
	*identifierStart = t;
	*identifierEnd = NULL;
	
	hadSpace = 0;
	
	for (; *t != '\0'; t++) {
		if (*t == ' ') {
			hadSpace = 1;
		}
		if (hadSpace) return JSON_EINVAL;

		/* allow alpha and underscore */
		if ((*t >= 'a' && *t <= 'z') ||
		    (*t >= 'A' && *t <= 'Z') ||
		    (*t == '_')) continue;

		/* allow numeric, so long as it isnt the first character */
		if (*t >= '0' && *t <= '9') {
			if (t != *identifierStart) continue;
			return JSON_EINVAL;
		}

		if (*t == '[' || *t == '.') { 
			*identifierEnd = t - 1;
			*idType = ID_IDENTIFIER;
			break;
		}
	}
	if (*t == '\0') {
		*identifierEnd = t - 1;
		*idType = ID_IDENTIFIER;
	}
	
	if (*identifierEnd < *identifierStart) return JSON_EINVAL;
	
	return JSON_ENONE;
}
