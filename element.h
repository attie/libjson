#ifndef __OBJECT_H
#define __OBJECT_H

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

json_err json_elementNew(struct json_element **element);
json_err json_elementDestroy(struct json_element *element);
json_err json_identifyAsArray(unsigned char *identifier, unsigned char **identifierStart, unsigned char **identifierEnd, enum identifierType *idType);
json_err json_identifyAsElement(unsigned char *identifier, unsigned char **identifierStart, unsigned char **identifierEnd, enum identifierType *idType);

#endif /* __OBJECT_H */
