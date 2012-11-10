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

#include "json_int.h"

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

	{
		char *a;
		int b;
		json_print(json, &a, &b);
		printf("b = %d\n", b);
		printf(">>\n%s\n<<", a);
	}

	return 0;
}
