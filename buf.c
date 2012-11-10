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
#include <stdarg.h>
#include <string.h>

#include "json_int.h"
#include "buf.h"

#define BUF_EXPAND_SIZE 4096

json_err json_bufExpand(struct json_buf *buf) {
	return json_bufnExpand(buf, BUF_EXPAND_SIZE);
}
json_err json_bufnExpand(struct json_buf *buf, size_t extra) {
	unsigned char *nBuf;
	unsigned int nBufLen;

	if (!buf) return JSON_EMISSINGPARAM;

	nBufLen = buf->len + extra;

	if ((nBuf = realloc(buf->data, nBufLen)) == NULL) return JSON_ENOMEM;

	buf->data = nBuf;
	buf->len = nBufLen;

	return JSON_ENONE;
}

json_err json_bufSpace(struct json_buf *buf, size_t extra) {
	unsigned int nBytes;

	if (!buf) return JSON_EMISSINGPARAM;

	nBytes  = buf->pos + extra;
	if (nBytes < buf->len) return JSON_ENONE;

	return json_bufnExpand(buf, nBytes - buf->len);
}

json_err json_bufPrintf(struct json_buf *buf, const char *format, ...) {
	json_err ret;
	va_list ap;

	va_start(ap, format);
	ret = json_bufvPrintf(buf, format, ap);
	va_end(ap);

	return ret;
}

json_err json_bufvPrintf(struct json_buf *buf, const char *format, va_list ap) {
	return _json_bufvPrintf(buf, format, ap, 1);
}

json_err _json_bufvPrintf(struct json_buf *buf, const char *format, va_list ap, int recurse) {
	json_err ret;
	va_list ap2;
	unsigned int space, printed;
	int needed;

	space = buf->len - buf->pos;
	va_copy(ap2, ap);
	printed = vsnprintf((char*)&(buf->data[buf->pos]), space, format, ap2);
	va_end(ap2);
	needed = printed - space;

	if (needed < 0) {
		buf->pos += printed;
		return JSON_ENONE;
	}
	if (!recurse) return JSON_ENOMEM;

	/* over allocate if we are just after a small amount of data... */
	if (needed < BUF_EXPAND_SIZE) {
		ret = json_bufExpand(buf);
	} else {
		ret = json_bufnExpand(buf, needed);
	}
	if (ret != JSON_ENONE) return ret;

	return _json_bufvPrintf(buf, format, ap, 0);
}

json_err json_bufTrim(struct json_buf *buf) {
	unsigned char *nBuf;

	if (!buf) return JSON_EMISSINGPARAM;

	if ((nBuf = realloc(buf->data, buf->pos + 1)) == NULL) return JSON_ENOMEM;

	buf->data = nBuf;
	buf->len = buf->pos + 1;

	return JSON_ENONE;
}
