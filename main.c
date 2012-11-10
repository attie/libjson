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
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "json_int.h"
#include "object.h"

/*
{
0       people: [
1               {
2                       id: 0,
3                       name: {
4                               first: "Attie",
5                               last: "Grande",
                        },
6                       dob: 599119200,
7               },{
8                       id: 1,
9                       name: {
10                              first: "Test",
11                              last: "Person",
                        },
12                      dob: 731138400,
                }
        ],
}
*/


int main(int argc, char *argv[]) {
	struct json *json;
	struct json_object *root;
	unsigned char *out;
	unsigned int outLen;
	unsigned char **children;
	unsigned int i;

	json_new(&json, NULL);
	
	{
		json_err ret;
		int fd;
		unsigned char inBuf[128];
		int nBytes;
		
		if ((fd = open("./testdata.dat", O_RDONLY)) < 0) {
			fprintf(stderr, "open(): %d - '%s'\n", fd, strerror(errno));
			return 1;
		}
		
		while ((nBytes = read(fd, inBuf, sizeof(inBuf))) > 0) {
			if ((ret = json_dataAdd(json, inBuf, nBytes)) != JSON_ENONE && ret != JSON_ECOMPLETE && ret != JSON_EINCOMPLETE) {
				fprintf(stderr, "json_dataAdd() failed: %d\n", ret);
			}
		}
		
		close(fd);
	}
	
	json_print(json, &out, &outLen);
	printf("out:>\n%s<\n", out);
	printf("outLen:%d\n", outLen);
	free(out);
	
	json_destroy(json);
	
#if 0
	json_new(&json, &root);

	json_addString(root, "", "request", "none", 4);
	json_addInteger(root, "", "time", time(NULL));
	json_addArray(root, "", "parameters", NULL);
	json_addArray(root, "parameters", NULL, NULL);
	json_addArray(root, "parameters[0]", NULL, NULL);
	json_addArray(root, "parameters[0][0]", NULL, NULL);
	json_addObject(root, "parameters", NULL, NULL);
	json_addString(root, "parameters[1]", "uri", "/index.htm", 10);

	json_print(json, &out, &outLen);

	printf("out:>\n%s<\n", out);
	printf("outLen:%d\n", outLen);
	free(out);
	
	json_getChildren(root, "", &children);
	for (i = 0; children[i]; i++) {
		printf("Child[%d]: '%s'\n", i, children[i]);
	}
	free(children);
	
	json_destroy(json);
#endif
	
#if 0
	json_err ret;

	struct json *json;
	struct json_object *root;
	struct json_object *o[13];
	struct json_object *target, *target2, *target3;

	json_new(&json);

	json_objectNew(&root);
	root->root = json;
	json->head = root;
	root->parent = NULL;
	root->name = NULL;
	root->type = JSON_OBJECT;

	json_objectNew(&o[0]);
	o[0]->root = json;
	root->child_head = o[0];
	o[0]->parent = root;
	o[0]->name = (unsigned char *)"people";
	o[0]->type = JSON_ARRAY;

	json_objectNew(&o[1]);
	o[0]->child_head = o[1];
	o[1]->root = json;
	o[1]->parent = o[0];
	o[1]->name = NULL;
	o[1]->type = JSON_OBJECT;	

	json_objectNew(&o[2]);
	o[1]->child_head = o[2];
	o[2]->root = json;
	o[2]->parent = o[1];
	o[2]->name = (unsigned char *)"id";
	o[2]->type = JSON_INTEGER;

	json_objectNew(&o[3]);
	o[2]->sibling_next = o[3];
	o[3]->sibling_prev = o[2];
	o[3]->root = json;
	o[3]->parent = o[1];
	o[3]->name = (unsigned char *)"name";
	o[3]->type = JSON_OBJECT;

	json_objectNew(&o[4]);
	o[3]->child_head = o[4];
	o[4]->root = json;
	o[4]->parent = o[3];
	o[4]->name = (unsigned char *)"first";
	o[4]->type = JSON_STRING;
	o[4]->data_len = 5;
	o[4]->data.asRaw = (unsigned char *)"Attie";

	json_objectNew(&o[5]);
	o[4]->sibling_next = o[5];
	o[5]->sibling_prev = o[4];
	o[5]->root = json;
	o[5]->parent = o[3];
	o[5]->name = (unsigned char *)"last";
	o[5]->type = JSON_STRING;
	o[5]->data_len = 6;
	o[5]->data.asRaw = (unsigned char *)"Grande";

	json_objectNew(&o[6]);
	o[3]->sibling_next = o[6];
	o[6]->sibling_prev = o[3];
	o[6]->root = json;
	o[6]->parent = o[1];
	o[6]->name = (unsigned char *)"dob";
	o[6]->type = JSON_INTEGER;
	o[6]->data.asInt = 599119200;

	{ struct json_object *x;
		json_objectNew(&x);
		o[6]->sibling_next = x;
		x->sibling_prev = o[6];
		x->root = json;
		x->parent = o[1];
		x->name = (unsigned char *)"favNum";
		x->type = JSON_FLOAT;
		x->data.asFloat = 6.34;
	}

	json_objectNew(&o[7]);
	o[1]->sibling_next = o[7];
	o[7]->sibling_prev = o[1];
	o[7]->root = json;
	o[7]->parent = o[0];
	o[7]->name = NULL;
	o[7]->type = JSON_OBJECT;

	json_objectNew(&o[8]);
	o[7]->child_head = o[8];
	o[8]->root = json;
	o[8]->parent = o[7];
	o[8]->name = (unsigned char *)"id";
	o[8]->type = JSON_INTEGER;
	o[8]->data.asInt = 1;

	json_objectNew(&o[9]);
	o[8]->sibling_next = o[9];
	o[9]->sibling_prev = o[8];
	o[9]->root = json;
	o[9]->parent = o[7];
	o[9]->name = (unsigned char *)"name";
	o[9]->type = JSON_OBJECT;

	json_objectNew(&o[10]);
	o[9]->child_head = o[10];
	o[10]->root = json;
	o[10]->parent = o[9];
	o[10]->name = (unsigned char *)"first";
	o[10]->type = JSON_STRING;
	o[10]->data_len = 3;
	o[10]->data.asRaw = (unsigned char *)"Test";

	json_objectNew(&o[11]);
	o[10]->sibling_next = o[11];
	o[11]->sibling_prev = o[10];
	o[11]->root = json;
	o[11]->parent = o[9];
	o[11]->name = (unsigned char *)"last";
	o[11]->type = JSON_STRING;
	o[11]->data_len = 6;
	o[11]->data.asRaw = (unsigned char *)"Person";

	json_objectNew(&o[12]);
	o[9]->sibling_next = o[12];
	o[12]->sibling_prev = o[9];
	o[12]->root = json;
	o[12]->parent = o[7];
	o[12]->name = (unsigned char *)"dob";
	o[12]->type = JSON_INTEGER;
	o[12]->data.asInt = 731138400;

#if 0
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[0]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[ 0]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[0 ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[ 0 ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[15]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[ 634]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[136345324 ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[ 6347543 ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[ 0.2 ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[0abc]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[0a0]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[0 0]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[a.b]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[a . b]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[a.b[6]]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[a.b [6] ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"[a.b[6].c]", &target));
#endif
	
#if 0
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"3eo", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"a33eo", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"peo", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"peo le", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"pe3", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"pe 3", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"peo.a", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"peo le.a", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"pe3.a", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"pe 3.a", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[0]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[ 0]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[0 ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[ 0 ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[ 0 0 ]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[\"attie\"]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[people[0].id]", &target));
	fprintf(stderr, "%d\n", json_locateObject(json->head, (unsigned char *)"people[ people[ 0 ] . id ]", &target));
#endif
	
#if 0
	if ((ret = json_locateObject(json->head, (unsigned char *)"people[0].id", &target)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	printf("Data: [%d]\n", target->data.asInt);
	
	if ((ret = json_locateObject(json->head, (unsigned char *)"people[0].name.first", &target)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	printf("Data: [%s]\n", target->data.asRaw);
	
	if ((ret = json_locateObject(json->head, (unsigned char *)"people[1].name.first", &target)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	printf("Data: [%s]\n", target->data.asRaw);
	
	if ((ret = json_locateObject(json->head, (unsigned char *)"people[0].name.last", &target)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	printf("Data: [%s]\n", target->data.asRaw);
	
	if ((ret = json_locateObject(json->head, (unsigned char *)"people[0].dob", &target)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	printf("Data: [%d]\n", target->data.asInt);
	
	
	
	
	if ((ret = json_locateObject(json->head, (unsigned char *)"people[0]", &target)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	if ((ret = json_locateObject(target, (unsigned char *)"dob", &target2)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	printf("Data: [%d]\n", target2->data.asInt);
	
	if ((ret = json_locateObject(target, (unsigned char *)"name", &target2)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	if ((ret = json_locateObject(target2, (unsigned char *)"first", &target3)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	printf("Data: [%s]\n", target3->data.asRaw);
	
	
	
	if ((ret = json_locateObject(json->head, (unsigned char *)"people", &target)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	if ((ret = json_locateObject(target, (unsigned char *)"[1].name.first", &target2)) != JSON_ENONE) {
		fprintf(stderr, "json_locateObject() failed: %d\n", ret);
		return 1;
	}
	printf("Data: [%s]\n", target2->data.asRaw);
#endif

#if 0
	{
		unsigned char *a;
		int b;
		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		json_printObject(o[7], &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		json_printObject(o[10], &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);
	}
#endif

#if 0
	{
		unsigned int l;
		int i;
		float f;
		unsigned char *str;

		if ((ret = json_getArrayLen(json, (unsigned char*)"people", &l)) != JSON_ENONE) {
			fprintf(stderr, "json_getArrayLen() failed: %d\n", ret);
			return 1;
		}
		printf("array len: %u\n", l);

		if ((ret = json_getString(json, (unsigned char*)"people[0].name.first", &str, &l)) != JSON_ENONE) {
			fprintf(stderr, "json_getString() failed: %d\n", ret);
			return 1;
		}
		printf("string[%u]: '%s'\n", l, str);

		if ((ret = json_getInteger(json, (unsigned char*)"people[0].dob", &i)) != JSON_ENONE) {
			fprintf(stderr, "json_getInteger() failed: %d\n", ret);
			return 1;
		}
		printf("int: %d\n", i);

		if ((ret = json_getFloat(json, (unsigned char*)"people[0].favNum", &f)) != JSON_ENONE) {
			fprintf(stderr, "json_getInteger() failed: %d\n", ret);
			return 1;
		}
		printf("float: %f\n", f);
	}
#endif

#if 1
	{
		unsigned char *a;
		struct json_object *o;
		int b;
		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		if ((ret = json_addInteger(root, "people[0]", "funny", 0)) != JSON_ENONE) {
			fprintf(stderr, "json_add*() failed: %d\n", ret);
			return 1;
		};

		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		if ((ret = json_addFloat(root, "people[0]", "test", 5.23)) != JSON_ENONE) {
			fprintf(stderr, "json_add*() failed: %d\n", ret);
			return 1;
		};

		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		if ((ret = json_addString(root, "people[0]", "test2", "hello", 5)) != JSON_ENONE) {
			fprintf(stderr, "json_add*() failed: %d\n", ret);
			return 1;
		};

		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		if ((ret = json_addArray(root, "people[0]", "test3", &o)) != JSON_ENONE) {
			fprintf(stderr, "json_add*() failed: %d\n", ret);
			return 1;
		};

		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		if ((ret = json_addString(root, "people[0].test3", NULL, "iscool", 6)) != JSON_ENONE) {
			fprintf(stderr, "json_add*() failed: %d\n", ret);
			return 1;
		};

		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		if ((ret = json_addObject(root, "people[0]", "test4", &o)) != JSON_ENONE) {
			fprintf(stderr, "json_add*() failed: %d\n", ret);
			return 1;
		};

		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

		if ((ret = json_addString(root, "people[0].test4", "attie", "iscool", 6)) != JSON_ENONE) {
			fprintf(stderr, "json_add*() failed: %d\n", ret);
			return 1;
		};

		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);

	}
#endif

#endif
	return 0;
}
