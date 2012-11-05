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

json_err json_objectNew(struct json_object **objectRet) {
	struct json_object *object;

	if (!objectRet) return JSON_EMISSINGPARAM;

	if ((object = malloc(sizeof(*object))) == NULL) return JSON_ENOMEM;
	memset(object, 0, sizeof(*object));

	*objectRet = object;

	return JSON_ENONE;
}

json_err json_objectDestroy(struct json_object *object) {
	struct json_object *sibling, *sibling2;

	if (!object) return JSON_EMISSINGPARAM;

	/* locate the left-most sibling */
	for (sibling = object->sibling_prev; sibling->sibling_prev; sibling = sibling->sibling_prev);
	/* destroy from left to right, stepping over us! */
	sibling2 = (sibling ? sibling->sibling_next : NULL);
	for (; sibling; sibling = sibling2, sibling2 = sibling->sibling_next) {
		if (sibling != object) json_objectDestroy(sibling);
	}

	if (object->child_head) json_objectDestroy(object->child_head);

	if (object->name) free(object->name);
	switch (object->type) {
		case JSON_STRING:
		case JSON_FUNCTION:
			if (object->data.asRaw) free(object->data.asRaw);
		default:;
	}
	
	return JSON_ENONE;
}

enum identifierType {
	ID_INVALID,
	ID_NUMBER,
	ID_IDENTIFIER,
};

enum characterType {
	CHAR_NONE,
	CHAR_SPACE,
	CHAR_NUMERIC,
	CHAR_NONNUMERIC,
	CHAR_DOT,
};

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
	enum {
		CHAR_NONE,
		CHAR_SPACE,
		CHAR_NUMERIC,
		CHAR_NONNUMERIC,
		CHAR_DOT,
	} charType;
	
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

EXPORT json_err json_locateObject(struct json_object *root, unsigned char *identifier, struct json_object **targetRet) {
	json_err ret;
	unsigned char *identifierStart, *identifierEnd;
	enum identifierType idType;
	unsigned char *t;
	struct json_object *target;

	if (!root || !identifier) return JSON_EMISSINGPARAM;
	
	/* skip over the white space */
	for (t = identifier; *t == ' '; t++);
	
	if (*t == '[') { /* handle the array identifier */
		/* for simplicity, this will not resolve strings or floating point numbers, it will ONLY handle positive integer indexes
		   if you want to dig into an object, then use a freaking dot */
		unsigned int index;
		
		t++;
		
		if ((ret = json_identifyAsArray(t, &identifierStart, &identifierEnd, &idType)) != JSON_ENONE) return ret;
		
		switch (idType) {
			case ID_INVALID: return JSON_EINVAL;
			
			case ID_NUMBER: { /* simple offset, counting from zero being the left-most sibling */
				unsigned int ret;
				unsigned char *pIndex;
				unsigned char *q;

				/* make space */
				if ((pIndex = malloc(sizeof(*pIndex) * (identifierEnd - identifierStart + 2))) == NULL) return JSON_ENOMEM;
				
				/* copy the string in, and terminate */
				for (t = identifierStart, q = pIndex; t <= identifierEnd; *q = *t, t++, q++);
				*q = '\0';
				
				/* try to pull out the integer */
				ret = sscanf((char *)pIndex, "%u", &index);
				free(pIndex);
				if (ret != 1) return JSON_EINVAL;
				
				break;
			}
			case ID_IDENTIFIER: /* needs to be evaluated... */
#warning TODO - for simplicity im pretending that this path doesnt exist... maybe later
				return JSON_ENOTIMPLEMENTED;
				break;
		}

		/* find the left-most sibling */
		for (target = root->child_head; target && target->sibling_prev; target = target->sibling_prev);
		/* iterate to the indexed child */
		for (; index > 0 && target; index--, target = target->sibling_next);
		
		/* check that we actually found something */
		if (index != 0) return JSON_EMISSING;
		if (*identifierEnd != '\0') identifierEnd++;
		
	} else {
		if ((ret = json_identifyAsElement(t, &identifierStart, &identifierEnd, &idType)) != JSON_ENONE) return ret;
		
		/* find the left-most child */
		for (target = root->child_head; target && target->sibling_prev; target = target->sibling_prev);
		
		/* find the named sibling */
		for (; target; target = target->sibling_next) {
			unsigned char *q, *p;
			
			/* skip items with no name... */
			if (!target->name) continue;
			
			/* compare the names */
			for (q = identifierStart, p = target->name; q <= identifierEnd && *q == *p; q++, p++);

			/* check that the match was valid (full against full) */
			if (q == identifierEnd + 1 && *p == '\0') break;
		}
	}
	
	/* check that we actually found something */
	if (!target) return JSON_EMISSING;
	
	/* now that we are done with the identifier, move it along before passing it on */
	if (*identifierEnd != '\0') identifierEnd++;
	if (*identifierEnd != '\0' && *identifierEnd == '.') identifierEnd++;

	/* if we haven't run through all of the identifiers, then continue! */
	if (*identifierEnd != '\0') {
		return json_locateObject(target, identifierEnd, targetRet);
	}

	/* return the target if they wanted it */
	if (targetRet) *targetRet = target;

	return JSON_ENONE;
}
