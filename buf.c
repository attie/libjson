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
#include "buf.h"

#define BUF_EXPAND_SIZE 4096

json_err json_bufExpand(struct json_buf *buf) {
	char *nBuf;
	int nBufLen;

	if (!buf) return JSON_EMISSINGPARAM;

	nBufLen = buf->len + BUF_EXPAND_SIZE;

	if ((nBuf = realloc(buf->data, nBufLen)) == NULL) return JSON_ENOMEM;

	buf->data = nBuf;
	buf->len = nBufLen;

	return JSON_ENONE;
}

json_err json_bufTrim(struct json_buf *buf) {
	char *nBuf;

	if (!buf) return JSON_EMISSINGPARAM;

	if ((nBuf = realloc(buf->data, buf->pos)) == NULL) return JSON_ENOMEM;

	buf->data = nBuf;
	buf->len = buf->pos;

	return JSON_ENONE;
}
