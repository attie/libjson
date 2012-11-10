#ifndef __BUF_H
#define __BUF_H

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

struct json_buf {
	int len;
	int pos;
	unsigned char *data;
};

/* these will add BUF_EXPAND_SIZE / 'extra' bytes to the buffer */
json_err json_bufExpand(struct json_buf *buf);
json_err json_bufnExpand(struct json_buf *buf, size_t extra);

/* ensure that there are at least 'extra' bytes avaliable pas pos in the buffer */
json_err json_bufSpace(struct json_buf *buf, size_t extra);

/* print to the buffer! like printf() */
json_err json_bufPrintf(struct json_buf *buf, const char *format, ...);
json_err json_bufvPrintf(struct json_buf *buf, const char *format, va_list ap);
json_err _json_bufvPrintf(struct json_buf *buf, const char *format, va_list ap, int recurse);

/* trim the buffer to pos (which should point to the terminating NUL) */
json_err json_bufTrim(struct json_buf *buf);

#endif /* __BUF_H */
