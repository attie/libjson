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
#include <ctype.h>

#include "json_int.h"
#include "parse.h"

json_err json_parseHandleElement(struct json *json, enum json_dataTypes type, unsigned char *name, unsigned int nameLen) {
	json_err ret;
	struct json_parse *p;
	unsigned char c;
	
	if (!json) return JSON_EMISSINGPARAM;
	if (type != JSON_OBJECT && type != JSON_ARRAY) return JSON_ETYPEMISMATCH;
	p = &json->parse;
	
	if (name) {
		c = name[nameLen];
		name[nameLen] = '\0';
	}
	
	if (type == JSON_OBJECT) {
		ret = json_addObject(p->element, "", name, &p->element);
	} else {
		ret = json_addArray(p->element, "", name, &p->element);
	}
	
	if (name) {
		name[nameLen] = c;
	}
	
	if (ret != JSON_ENONE) return ret;
	
	memset(&p->state, 0, sizeof(p->state));
	p->pos++;
	return JSON_ENONE;
}
				
json_err json_parseHandleItem(struct json *json) {
	json_err ret;
	struct json_parse *p;
	unsigned char c;
	char *name, *value;
	int nameLen, valueLen;
	unsigned char c_name, c_value;
	
	if (!json) return JSON_EMISSINGPARAM;
	p = &json->parse;
	
	name = &(p->buf.data[p->state.s_name]);
	nameLen = p->state.e_name - p->state.s_name;
	c_name = name[nameLen];
	name[nameLen] = '\0';
	
	value = &(p->buf.data[p->state.s_value]);
	valueLen = p->state.e_value - p->state.s_value;
	c_value = value[valueLen];
	value[valueLen] = '\0';
	
	/* is it a string? */
	if (p->state.q_value == 2) {
		if ((ret = json_addString(p->element, "", name, value, valueLen)) != JSON_ENONE) return ret;
		goto taken;
	}
	/* is it an integer/float/hex? */
	{	int i, d, h;
		for (i = 0, d = 0, h = 0; i < valueLen; i++) {
			if (i == 0) {
				/* allow a sign for the first character */
				if (value[i] == '-' || value [i] == '+') continue;
				/* remember if the first character is an '0' - looking for a hex value */
				if (value[i] == '0') h++;
			} else if (i == 1) {
				/* allow the second character to be an 'x', only if the first was an '0' - looking for a hex value */
				if (value[i] == 'x' || value[i] == 'X') {
					if (h != 1) goto not_number;;
					h++;
					continue;
				}
			}
			if (value[i] == '.') {
				if (h == 2) goto not_number;
				d++;
				continue;
			}
			if (!isdigit(value[i])) break;
		}
		if (i == valueLen) {
			if (d == 0) {
				if (h == 2) {
					/* hex! -> integer */
					int v;
					value += 2;
					valueLen -= 2;
					if (sscanf(value, "%x", &v) != 1) return JSON_EINVAL;
					if ((ret = json_addInteger(p->element, "", name, v)) != JSON_ENONE) return ret;
					goto taken;
				} else if (h == 0) {
					/* integer! */
					int v;
					if (sscanf(value, "%d", &v) != 1) return JSON_EINVAL;
					if ((ret = json_addInteger(p->element, "", name, v)) != JSON_ENONE) return ret;
					goto taken;
				}
				goto not_number;
			} else if (d == 1 && h < 2) {
				/* float! */
				double v;
				if (sscanf(value, "%lf", &v) != 1) return JSON_EINVAL;
				if ((ret = json_addFloat(p->element, "", name, v)) != JSON_ENONE) return ret;
				goto taken;
			}
		}
	}
not_number:
	
	/* is it a null? */
	if (!strncasecmp("null", value, 4)) {
		if ((ret = json_addNull(p->element, "", name)) != JSON_ENONE) return ret;
		goto taken;
	}
	
	/* is it a boolean? */
	if (!strncasecmp("true", value, 4)) {
		if ((ret = json_addBoolean(p->element, "", name, 1)) != JSON_ENONE) return ret;
		goto taken;
	} else if (!strncasecmp("false", value, 5)) {
		if ((ret = json_addBoolean(p->element, "", name, 0)) != JSON_ENONE) return ret;
		goto taken;
	}

	return JSON_EINVAL;
	
taken:
	name[nameLen] = c_name;
	value[valueLen] = c_value;
	
	memset(&p->state, 0, sizeof(p->state));
	for (; p->pos < p->buf.pos; p->pos++) {
		c = p->buf.data[p->pos];
		if (c != ',' && !isspace(c)) break;
	}

	return JSON_ENONE;
}

json_err json_parseGetName(struct json *json) {
	struct json_parse *p;
	unsigned char c;
	unsigned int s, e;
	
	if (!json) return JSON_EMISSINGPARAM;
	p = &json->parse;
	s = -1;
	e = -1;
	
	if (p->state.s_name == 0) {
		p->state.q_name = 0;
		for (; s == -1 && p->state.q_name == 0 && p->pos < p->buf.pos; p->pos++) {
			c = p->buf.data[p->pos];
			if (isspace(c) || c == ',') continue;
			if (c == '{') {
				return json_parseHandleElement(json, JSON_OBJECT, NULL, 0);
			} else if (c == '[') {
				return json_parseHandleElement(json, JSON_ARRAY, NULL, 0);
			} else if (c == '}' || c == ']') {
				if (!p->element->parent) return JSON_ECOMPLETE;
				p->element = p->element->parent;
				continue;
			} else if (c == '"') {
				p->state.q_name = 2;
				p->pos++;
				s = p->pos;
			} else {
				p->state.q_name = 1;
				s = p->pos;
			}
			break;
		}
		if (s == -1 || p->state.q_name == 0) return JSON_EINCOMPLETE;
		p->state.s_name = s;
	} else {
		s = p->state.s_name;
	}
	
	for (; e == -1 && p->pos < p->buf.pos; p->pos++) {
		c = p->buf.data[p->pos];
		if (p->state.q_name == 2) {
			if (p->buf.data[p->pos] != '"') continue;
			e = p->pos;
			p->pos++;
			break;
		} else {
			/* a ':' marks the end of the name */
			if (c == ':') {
				e = p->pos;
				break;
			}
			/* first character of an unquoted identifier can't be a number */
			if (p->pos == s && isdigit(c)) return JSON_EINVAL;
			/* can't contain punctuation */
			if (ispunct(c)) return JSON_EINVAL;
			/* whitespace delimits the end of the identifier */
			if (isspace(c)) {
				e = p->pos;
				break;
			}
		}
	}
	if (e == -1) return JSON_EINCOMPLETE;
	p->state.e_name = e;
	
	return JSON_ENONE;
}

json_err json_parseGetColon(struct json *json) {
	struct json_parse *p;
	unsigned char c;
	unsigned int i;
	
	if (!json) return JSON_EMISSINGPARAM;
	p = &json->parse;
	i = -1;

	for (; i == -1 && p->pos < p->buf.pos; p->pos++) {
		c = p->buf.data[p->pos];
		if (isspace(c)) continue;
		if (c == ':') {
			i = p->pos;
			p->pos++;
			break;
		}
	}
	if (i == -1) return JSON_EINCOMPLETE;
	p->state.i_colon = i;

	return JSON_ENONE;
}

json_err json_parseGetValue(struct json *json) {
	struct json_parse *p;
	unsigned char c;
	unsigned int s, e;
	
	if (!json) return JSON_EMISSINGPARAM;
	p = &json->parse;
	s = -1;
	e = -1;
	
	if (p->state.s_value == 0) {
		p->state.q_value = 0;
		for (; s == -1 && p->pos < p->buf.pos; p->pos++) {
			c = p->buf.data[p->pos];
			if (isspace(c)) continue;
			if (c == '{') {
				return json_parseHandleElement(json, JSON_OBJECT, &(p->buf.data[p->state.s_name]), p->state.e_name - p->state.s_name);
			} else if (c == '[') {
				return json_parseHandleElement(json, JSON_ARRAY, &(p->buf.data[p->state.s_name]), p->state.e_name - p->state.s_name);
			} else if (c == '"') {
				p->state.q_value = 2;
				p->pos++;
				s = p->pos;
				break;
			} else {
				p->state.q_value = 1;
				s = p->pos;
			}
		}
		if (s == -1) return JSON_EINCOMPLETE;
		p->state.s_value = s;
	} else {
		s = p->state.s_value;
	}

	for (; e == -1 && p->pos < p->buf.pos; p->pos++) {
		c = p->buf.data[p->pos];
		if (p->state.q_value == 2) {
			if (p->buf.data[p->pos] != '"') continue;
			e = p->pos;
			p->pos++;
			break;
		} else {
			if (isspace(c) || c == ',' || c == ']' || c == '}') {
				e = p->pos;
				break;
			}
		}
	}
	if (e == -1) return JSON_EINCOMPLETE;
	p->state.e_value = e;

	return JSON_ENONE;
}

json_err json_parseRun(struct json *json) {
	json_err ret;
	struct json_parse *p;
	unsigned char c;
	unsigned int prev_pos;
	
	if (!json) return JSON_EMISSINGPARAM;
	p = &json->parse;
	
	if (!p->buf.data || p->buf.pos == 0) return JSON_EINCOMPLETE;
	if (!p->element) p->element = json->root;
	
	if (p->pos == 0) {
		/* make sure we have JSON - must start with a '{' */
		for (;p->pos < p->buf.pos; p->pos++) {
			c = p->buf.data[p->pos];
			if (isspace(c)) continue;
			if (c == '{') break;
			p->pos = 0;
			p->buf.pos = 0;
		}
		if (c != '{') return JSON_EINCOMPLETE;
		p->pos++; /* step over the initial object */
	}
	
	for (prev_pos = 0; p->pos < p->buf.pos;) {
		if (p->state.e_name && p->state.e_value) {
			if ((ret = json_parseHandleItem(json)) != JSON_ENONE) return ret;
		}
		
		if (prev_pos == p->pos) return JSON_EINCOMPLETE;
		prev_pos = p->pos;
		
		if (!p->state.e_name) {
			if ((ret = json_parseGetName(json)) != JSON_ENONE) return ret;
		} else if (!p->state.i_colon) {
			if ((ret = json_parseGetColon(json)) != JSON_ENONE) return ret;
		} else if (!p->state.e_value) {
			if ((ret = json_parseGetValue(json)) != JSON_ENONE) return ret;
		}
	}
	
	return JSON_ENONE;
}

EXPORT json_err json_dataAdd(struct json *json, const unsigned char *data, unsigned int len) {
	json_err ret;
	
	if (!json || !data) return JSON_EMISSINGPARAM;
	if (len == 0) return JSON_ENONE;
	switch (json->parse.err) {
		case JSON_ENONE:
		case JSON_EINCOMPLETE:
			break;
		default:
			return json->parse.err;
	}
	
	if ((ret = json_bufImport(&json->parse.buf, data, len)) != JSON_ENONE) return ret;
	
	return (json->parse.err = json_parseRun(json));
}
