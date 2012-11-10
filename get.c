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

EXPORT json_err json_getType(struct json_object *root, unsigned char *identifier, enum json_dataTypes *type) {
	json_err ret;
	struct json_object *target;

	if (!root || !identifier) return JSON_EMISSINGPARAM;
	if ((ret = json_getObject(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;

	if (type) *type = target->type;

	return JSON_ENONE;
}

EXPORT json_err json_getChildren(struct json_object *root, unsigned char *identifier, unsigned char ***childrenRet) {
	json_err ret;
	struct json_object *target;
	struct json_object *child, *cFirst;
	
	int i;
	size_t memSize;
	unsigned char **cList;
	unsigned char *cName;

	if (!root || !identifier) return JSON_EMISSINGPARAM;
	if ((ret = json_getObject(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;

	/* locate the left most child */
	for (child = target->child_head; child && child->sibling_prev; child = child->sibling_prev);
	if (!child) return JSON_EMISSING;
	cFirst = child;

	memSize = 0;
	for (i = 0; child; child = child->sibling_next) {
		if (!child->name) continue;
		memSize += sizeof(char *);
		memSize += sizeof(char) * (strlen(child->name) + 1);
		i++;
	}
	memSize += sizeof(char *);

	if ((cList = malloc(memSize)) == NULL) return JSON_ENOMEM;
	
	cName = (char *)&(cList[i+1]);
	for (i = 0, child = cFirst; child; child = child->sibling_next) {
		if (!child->name) continue;
		cList[i] = cName;
		strcpy(cName, child->name);
		cName += strlen(cName) + 1;
		i++;
	}
	cList[i] = NULL;
	
	*childrenRet = cList;
	
	return JSON_ENONE;
}

EXPORT json_err json_getInteger(struct json_object *root, unsigned char *identifier, int *data) {
	json_err ret;
	struct json_object *target;

	if (!root || !identifier || !data) return JSON_EMISSINGPARAM;
	if ((ret = json_getObject(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;
	if (target->type != JSON_INTEGER) return JSON_ETYPEMISMATCH;

	*data = target->data.asInt;

	return JSON_ENONE;
}

EXPORT json_err json_getFloat(struct json_object *root, unsigned char *identifier, float *data) {
	json_err ret;
	struct json_object *target;

	if (!root || !identifier || !data) return JSON_EMISSINGPARAM;
	if ((ret = json_getObject(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;
	if (target->type != JSON_FLOAT) return JSON_ETYPEMISMATCH;

	*data = target->data.asFloat;

	return JSON_ENONE;
}

EXPORT json_err json_getString(struct json_object *root, unsigned char *identifier, unsigned char **data, unsigned int *dataLen) {
	json_err ret;
	struct json_object *target;

	if (!root || !identifier || !data || !dataLen) return JSON_EMISSINGPARAM;
	if ((ret = json_getObject(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;
	if (target->type != JSON_STRING) return JSON_ETYPEMISMATCH;

	*data = target->data.asRaw;
	*dataLen = target->data_len;

	return JSON_ENONE;
}

EXPORT json_err json_getFunction(struct json_object *root, unsigned char *identifier, unsigned char **data, unsigned int *dataLen) {
	return JSON_ENOTIMPLEMENTED;
}

EXPORT json_err json_getArrayLen(struct json_object *root, unsigned char *identifier, unsigned int *length) {
	json_err ret;
	struct json_object *target;
	struct json_object *child;

	if (!root || !identifier || !length) return JSON_EMISSINGPARAM;
	if ((ret = json_getObject(root, identifier, &target)) != JSON_ENONE) return ret;
	if (!target) return JSON_EMISSING;
	if (target->type != JSON_ARRAY) return JSON_ETYPEMISMATCH;

	for (child = target->child_head; child && child->sibling_prev; child = child->sibling_prev);
	for ((*length) = 0; child; (*length)++, child = child->sibling_next);

	return JSON_ENONE;
}

EXPORT json_err json_getObject(struct json_object *root, unsigned char *identifier, struct json_object **targetRet) {
	json_err ret;
	unsigned char *identifierStart, *identifierEnd;
	enum identifierType idType;
	unsigned char *t;
	struct json_object *target;

	if (!root || !identifier) return JSON_EMISSINGPARAM;
	
	/* skip over the white space */
	for (t = identifier; *t == ' '; t++);
	
	if (*t == '\0') {
		if (targetRet) *targetRet = root;
		return JSON_ENONE;
	} else if (*t == '[') { /* handle the array identifier */
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
		return json_getObject(target, identifierEnd, targetRet);
	}

	/* return the target if they wanted it */
	if (targetRet) *targetRet = target;

	return JSON_ENONE;
}

