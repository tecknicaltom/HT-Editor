/* 
 *	HT Editor
 *	except.h
 *
 *	Copyright (C) 1999-2002 Stefan Weyergraf (stefan@weyergraf.de)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __EXCEPT_H__
#define __EXCEPT_H__

#include <errno.h>		// for IOException

#include "data.h"

/**
 *	A exception.
 */
class Exception: public Object {
public:
	/* new */
	virtual String &	reason(String &result) const;
	virtual	int		toString(char *buf, int buflen) const;
};

#define MSG_EXCEPTION_MAX_ERRSTR		256

/**
 *	A described exception.
 */
class MsgException: public Exception {
protected:
	char estr[MSG_EXCEPTION_MAX_ERRSTR];
public:
		MsgException(const char *errstr);
		MsgException() {};
	/* new */
	virtual	String &reason(String &result) const;
};

/**
 *	A formatted described exception.
 */
class MsgfException: public MsgException {
public:
		MsgfException(const char *errstr, ...);
};

/**
 *	A I/O exception.
 */
class IOException: public Exception {
protected:
	String errstr;
public:
	int mPosixErrno;

		IOException(int aPosixErrno);
	/* new */
	virtual	String &reason(String &result) const;
};

class EOFException: public IOException {
public:
		EOFException();
	/* new */
	virtual	String &reason(String &result) const;
};

typedef Exception InternalException;

/**
 *	A exception, indicating a not-implemented function.
 */
class NotImplementedException: public InternalException {
protected:
	String	location;
public:
		NotImplementedException(const String &filename, int line_number);
	/* new */
	virtual	String &reason(String &result) const;
};

/**
 *	A exception, indicating a illegal argument.
 */
class IllegalArgumentException: public InternalException {
protected:
	String	location;
public:
		IllegalArgumentException(const String &filename, int line_number);
	/* new */
	virtual	String &reason(String &result) const;
};

/**
 *	An index out-of-bounds exception
 */
class IndexOutOfBoundsException: public InternalException {
protected:
	String	location;
public:
		IndexOutOfBoundsException(const String &filename, int line_number);
	/* new */
	virtual	String &reason(String &result) const;
};

/**
 *	A exception, indicating a illegal type-cast.
 */
class TypeCastException: public InternalException {
protected:
	String	aresult;
public:
		TypeCastException(const String &cast_type, const String &obj_type);
	/* new */
	virtual	String &reason(String &result) const;
};

#endif /* __EXCEPT_H__ */
