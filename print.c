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
#include "print.h"
#include "buf.h"

//#define PRINT_WHITESPACE

#ifdef PRINT_WHITESPACE
#define NEW_LINE "\n"
#define TAB "\t"
#endif

json_err _json_printElement(struct json_print_ctx *ctx) {
	json_err ret;
	int c;

	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	if (ctx->root->parent && ctx->root->parent->type == JSON_ARRAY) {
		for (c = 0; c < ctx->tab_depth; c++) {
#ifdef TAB
			json_bufPrintf(ctx->buf, TAB);
#endif
		}
	}
	switch (ctx->root->type) {
		//case JSON_ELEMENT:	ret = _json_printElement(ctx);  break;
		case JSON_NULL:			ret = _json_printNull(ctx);     break;
		case JSON_BOOLEAN:	ret = _json_printBoolean(ctx);  break;
		case JSON_INTEGER:	ret = _json_printInteger(ctx);  break;
		case JSON_FLOAT:		ret = _json_printFloat(ctx);    break;
		case JSON_STRING:		ret = _json_printString(ctx);   break;
		case JSON_FUNCTION:	ret = _json_printFunction(ctx); break;
		case JSON_OBJECT:		ret = _json_printObject(ctx);   break;
		case JSON_ARRAY:
			json_bufPrintf(ctx->buf, "[");
#ifdef NEW_LINE
			json_bufPrintf(ctx->buf, NEW_LINE);
#endif
			ctx->tab_depth++;
			ret = _json_printArray(ctx);
			ctx->tab_depth--;
			{
				int c;
				for (c = 0; c < ctx->tab_depth; c++) {
#ifdef TAB
					json_bufPrintf(ctx->buf, TAB);
#endif
				}
			}
			json_bufPrintf(ctx->buf, "]");
			break;
		default:						return JSON_EUNKNOWN;
	}
	return ret;
}

json_err _json_printNull(struct json_print_ctx *ctx) {
	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	json_bufPrintf(ctx->buf, "null");
	return JSON_ENONE;
}

json_err _json_printBoolean(struct json_print_ctx *ctx) {
	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	json_bufPrintf(ctx->buf, "%s", (ctx->root->data.asInt?"true":"false"));
	return JSON_ENONE;
}

json_err _json_printInteger(struct json_print_ctx *ctx) {
	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	json_bufPrintf(ctx->buf, "%d", ctx->root->data.asInt);
	return JSON_ENONE;
}

json_err _json_printFloat(struct json_print_ctx *ctx) {
	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	json_bufPrintf(ctx->buf, "%f", ctx->root->data.asFloat);
	return JSON_ENONE;
}

json_err _json_printString(struct json_print_ctx *ctx) {
	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	if (ctx->root->data.asRaw) {
		json_bufPrintf(ctx->buf, "\"%s\"", ctx->root->data.asRaw);
	} else {
		json_bufPrintf(ctx->buf, "\"\"");
	}
	return JSON_ENONE;
}

json_err _json_printFunction(struct json_print_ctx *ctx) {
	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	return JSON_ENONE;
}

json_err _json_printObject(struct json_print_ctx *ctx) {
	json_err ret;
	int c;
	struct json_object *o, *i;

	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	o = ctx->root;
	json_bufPrintf(ctx->buf, "{");
#ifdef NEW_LINE
	json_bufPrintf(ctx->buf, NEW_LINE);
#endif
	ctx->tab_depth++;

	for (i = o->child_head; i && i->sibling_prev; i = i->sibling_prev);
	for (c = 0; i; i = i->sibling_next, c++) {
		if (c) {
			json_bufPrintf(ctx->buf, ",");
#ifdef NEW_LINE
			json_bufPrintf(ctx->buf, NEW_LINE);
#endif
		}
		{
			int c;
			for (c = 0; c < ctx->tab_depth; c++) {
#ifdef TAB
				json_bufPrintf(ctx->buf, TAB);
#endif
			}
		}
		ctx->root = i;
		if (i->name) json_bufPrintf(ctx->buf, "\"%s\":", i->name);
		if ((ret = _json_printElement(ctx)) != JSON_ENONE) return ret;
	}

	ctx->tab_depth--;
#ifdef NEW_LINE
	json_bufPrintf(ctx->buf, NEW_LINE);
#endif
	for (c = 0; c < ctx->tab_depth; c++) {
#ifdef TAB
		json_bufPrintf(ctx->buf, TAB);
#endif
	}
	json_bufPrintf(ctx->buf, "}");
	ctx->root = o;

	return JSON_ENONE;
}

json_err _json_printArray(struct json_print_ctx *ctx) {
	json_err ret;
	int c;
	struct json_object *o, *i;

	if (!ctx || !ctx->root || !ctx->buf) return JSON_EMISSINGPARAM;
	o = ctx->root;

	for (i = o->child_head; i && i->sibling_prev; i = i->sibling_prev);
	for (c = 0; i; i = i->sibling_next, c++) {
		if (c) {
			json_bufPrintf(ctx->buf, ",");
#ifdef NEW_LINE
			json_bufPrintf(ctx->buf, NEW_LINE);
#endif
		}
		ctx->root = i;
		if ((ret = _json_printElement(ctx)) != JSON_ENONE) return ret;
	}
#ifdef NEW_LINE
	json_bufPrintf(ctx->buf, NEW_LINE);
#endif

	ctx->root = o;

	return JSON_ENONE;
}

EXPORT json_err json_print(struct json *json, unsigned char **output, unsigned int *outputLen) {
	return json_printObject(json->root, output, outputLen);
}

EXPORT json_err json_printObject(struct json_object *root, unsigned char **output, unsigned int *outputLen) {
	json_err ret;
	struct json_buf buf;
	struct json_print_ctx ctx;

	if (!root || !output || !outputLen) return JSON_EMISSINGPARAM;

	memset(&ctx, 0, sizeof(ctx));
	memset(&buf, 0, sizeof(buf));
	ctx.root = root;
	ctx.buf = &buf;

	if ((ret = _json_printElement(&ctx)) != JSON_ENONE) {
		if (buf.data) free(buf.data);
		return ret;
	}
#ifdef NEW_LINE
	json_bufPrintf(ctx.buf, NEW_LINE);
#endif

	json_bufTrim(&buf);

	*output = buf.data;
	*outputLen = buf.len;

	return JSON_ENONE;
}
