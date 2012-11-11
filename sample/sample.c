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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <json.h>

int main(int argc, char *argv[]) {
	json_err ret;
	
	struct json *json;
	struct json_element *root;
	struct json_element *target;
	double value;
	
	int fd;
	unsigned char inBuf[64];
	int nBytes;
	
	unsigned char *out;
	unsigned int outLen;
	
	
	/* create a new json instance, and get a handle to the root object */
	if ((ret = json_new(&json, &root)) != JSON_ENONE) {
		fprintf(stderr, "json_new() failed: %d\n", ret);
		return 1;
	}
	
	/* open the file with test data in it */
	if ((fd = open("./testdata.json", O_RDONLY)) < 0) {
		fprintf(stderr, "open(): %d - '%s'\n", fd, strerror(errno));
		return 1;
	}
	
	/* push the data into the json instance, chunk by chunk */
	while ((nBytes = read(fd, inBuf, sizeof(inBuf))) > 0) {
		/* json_dataAdd() can return;
		      JSON_ECOMPLETE on success - at this point there shouldn't be any more data in the file...
		      JSON_EINCOMPLETE          - at this point there should be more data in the file...
		      or any other error value */
		if ((ret = json_dataAdd(json, inBuf, nBytes)) != JSON_ENONE && ret != JSON_ECOMPLETE && ret != JSON_EINCOMPLETE) {
			fprintf(stderr, "json_dataAdd() failed: %d\n", ret);
			return 1;
		}
		
		/* on JSON_ECOMPLETE, you should probably stop feeding it... */
		if (ret == JSON_ECOMPLETE) break;
	}
	
	/* close the file */
	close(fd);
	
	/* check that we actually did comeplete reading in the json */
	if (ret != JSON_ECOMPLETE) {
		fprintf(stderr, "apparently there was some data missing... :-(\n");
		return 1;
	}
	
	/* print out the whole json object */
	if ((ret = json_print(json, &out, &outLen)) != JSON_ENONE) {
		fprintf(stderr, "json_print() failed: %d\n", ret);
		return 1;
	}
	printf("out:>\n%s<\n", out);
	printf("outLen:%d\n", outLen);
	free(out);
	
	/* you can even add more data */
	if ((ret = json_addString(root, "", "test", "Hello!", 6)) != JSON_ENONE) {
		fprintf(stderr, "json_addString() failed: %d\n", ret);
		return 1;
	}
	/* see... */
	if ((ret = json_print(json, &out, &outLen)) != JSON_ENONE) {
		fprintf(stderr, "json_print() failed: %d\n", ret);
		return 1;
	}
	printf("out:>\n%s<\n", out);
	printf("outLen:%d\n", outLen);
	free(out);
	
	/* and how about querying for a sub-section of the object? */
	if ((ret = json_getObject(root, "results[0].location", &target)) != JSON_ENONE) {
		fprintf(stderr, "json_getObject() failed: %d\n", ret);
		return 1;
	}
	/* see... */
	if ((ret = json_printElement(target, &out, &outLen)) != JSON_ENONE) {
		fprintf(stderr, "json_printElement() failed: %d\n", ret);
		return 1;
	}
	printf("out:>\n%s<\n", out);
	printf("outLen:%d\n", outLen);
	free(out);
	
	/* or even querying for a specific element */
	if ((ret = json_getFloat(root, "results[1].resolution", &value)) != JSON_ENONE) {
		fprintf(stderr, "json_getFloat() failed: %d\n", ret);
		return 1;
	}
	printf("results[1].resolution: %.12lf\n", value);
	
	/* or even querying for a specific element from the sub-section! */
	if ((ret = json_getFloat(target, "lat", &value)) != JSON_ENONE) {
		fprintf(stderr, "json_getFloat() failed: %d\n", ret);
		return 1;
	}
	printf("target->lat: %.12lf\n", value);
	
	/* how about deleting an element */
	if ((ret = json_deleteElement(root, "results[1].location")) != JSON_ENONE) {
		fprintf(stderr, "json_deleteElement() failed: %d\n", ret);
		return 1;
	}
	/* see... */
	if ((ret = json_print(json, &out, &outLen)) != JSON_ENONE) {
		fprintf(stderr, "json_print() failed: %d\n", ret);
		return 1;
	}
	printf("out:>\n%s<\n", out);
	printf("outLen:%d\n", outLen);
	free(out);

	/* tidy up the json instance */
	json_destroy(json);

	return 0;
}
