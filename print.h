#ifndef __PRINT_H
#define __PRINT_H

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

struct json_print_ctx {
	int tab_depth;
	struct json_object *root;
	struct json_buf *buf;
};

json_err _json_printElement(struct json_print_ctx *ctx);
json_err _json_printNull(struct json_print_ctx *ctx);
json_err _json_printBoolean(struct json_print_ctx *ctx);
json_err _json_printInteger(struct json_print_ctx *ctx);
json_err _json_printFloat(struct json_print_ctx *ctx);
json_err _json_printString(struct json_print_ctx *ctx);
json_err _json_printFunction(struct json_print_ctx *ctx);
json_err _json_printObject(struct json_print_ctx *ctx);
json_err _json_printArray(struct json_print_ctx *ctx);

#endif /* __PRINT_H */
