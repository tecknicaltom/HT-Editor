/*
 *	HT Editor
 *	str.cc
 *
 *	Copyright (C) 2002 Stefan Weyergraf (stefan@weyergraf.de)
 *	Copyright (C) 2002, 2003 Sebastian Biallas (sb@biallas.net)
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

#include <new>
#include <cctype>
#include <cstdlib>
#include <cstring>

#include "except.h"
#include "htdebug.h"
#include "snprintf.h"
#include "str.h"
#include "stream.h"
#include "strtools.h"
#include "tools.h"

#ifdef HAVE_HT_OBJECTS
#include "atom.h"
#endif

extern "C" {
#include "regex.h"
}

/*
 *	CLASS String
 */

/**
 *	creates empty string
 */
String::String()
{
	mContent = NULL;
	realloc(0);
}

/**
 *	create string from char *
 */
String::String(const char *s)
{
	mContent = NULL;
	assign(s);
}

/**
 *	copy constructor
 */
String::String(const String *s)
{
	mContent = NULL;
	assign(s);
}

/**
 *	copy constructor
 */
String::String(const String &s)
{
	assert(&s != this);
	mContent = NULL;
	assign(s);
}

/**
 *   creates string from array |s| size |aLength|
 */
String::String(const byte *s, int aLength)
{
	mContent = NULL;
	assign(s, aLength);
}

/**
 *   creates string with |count| times |c|
 */
String::String(char c, int count)
{
	mContent = NULL;
	assign(c, count);
}

String::~String()
{
	free(mContent);
}

/**
 *	(re-)assigns string to |s|
 */
void String::assign(const String *s)
{
	realloc(s->mLength);
	memcpy(mContent, s->mContent, mLength);
}

/**
 *	(re-)assigns string to |s|
 */
void String::assign(const String &s)
{
	realloc(s.mLength);
	memcpy(mContent, s.mContent, mLength);
}

/**
 *	(re-)assigns string to char * |s|
 */
void String::assign(const char *s)
{
	int slen = s ? strlen(s) : 0;
	realloc(slen);
	memcpy(mContent, s, mLength);
}

/**
 *	(re-)assigns string to array |s| length |aLength|
 */
void String::assign(const byte *s, int aLength)
{
	realloc(aLength);
	memcpy(mContent, s, mLength);
}

/**
 *	(re-)assigns string to |count| times |c|
 */
void String::assign(char c, int count)
{
	realloc(count);
	memset(mContent, c, count);
}

/**
 *	(re-)assigns string via ht_snprintf
 */
void String::assignFormat(const char *s, ...)
{
	char buf[1024];
	va_list vargs;
	va_start(vargs, s);
	ht_vsnprintf(buf, sizeof buf, s, vargs);
	va_end(vargs);
	assign(buf);
}

/**
 *   appends |s| to the end
 */
void String::append(const String &s)
{
	if (s.mLength) {
		int oldLength = mLength;
		realloc(mLength + s.mLength);
		memcpy(&mContent[oldLength], s.mContent, s.mLength);
	}
}

void String::append(const char *s)
{
	if (s && *s) {
		int oldLength = mLength;
		int slen = strlen(s);
		realloc(mLength + slen);
		memcpy(&mContent[oldLength], s, slen);
	}
}

void String::appendChar(char c)
{
	realloc(mLength+1);
	mContent[mLength-1] = c;
}

void String::append(const byte *s, int aLength)
{
	if (aLength <= 0) return;
	int oldLength = mLength;
	realloc(mLength + aLength);
	memcpy(&mContent[oldLength], s, aLength);
}

/**
 *	(re-)append to string via ht_snprintf
 */
void String::appendFormat(const char *s, ...)
{
	char buf[1024];
	va_list vargs;
	va_start(vargs, s);
	ht_vsnprintf(buf, sizeof buf, s, vargs);
	va_end(vargs);
	append(buf);
}

/**
 *   prepends |s| to the front
 */
void String::prepend(const String &s)
{
	if (s.mLength) {
		int oldLength = mLength;
		realloc(mLength + s.mLength);
		memmove(&mContent[s.mLength], &mContent[0], oldLength);
		memcpy(&mContent[0], s.mContent, s.mLength);
	}
}

/**
 *	Empties string.
 */
void String::clear()
{
	realloc(0);
}

String *String::clone() const
{
	return new String(mContent, mLength);
}

/**
 *   compares to characters.
 *	used in compareTo() and findXXX() (and therefore replace())
 *	@returns 0 for equality, negative number if |c1<c2| and positive number if |c1>c2|
 */
int String::compareChar(char c1, char c2) const
{
	if (c1 < c2) return -1;
	if (c1 > c2) return 1;
	return 0;
}

int String::compare(const char *s) const
{
	if (!mLength) {
		return (s) ? -1: 0;
	}
	if (!s) {
		return 1;
	}
	int l = mLength;
	for (int i=0; i < l; i++) {
		if (!*s) return 1;
		int r = compareChar(mContent[i], s[i]);
		if (r) return r;
	}
	if (s[l]) return -1;
	return 0;
}

int String::compare(const String &s) const
{
	if (!mContent) {
		return (s.mContent) ? -1: 0;
	}
	if (!s.mContent) {
		return 1;
	}
	int l = MIN(mLength, s.mLength);
	for (int i=0; i<l; i++) {
		int r = compareChar(mContent[i], s.mContent[i]);
		if (r) return r;
	}
	if (mLength < s.mLength) return -1;
	if (mLength == s.mLength) return 0;
	return 1;
}

/*
 *	like compare(s) but considers a maximum of |aMax| characters
 */
int String::compare(const String &s, int aMax) const
{
	if (aMax <= 0) return 0;
	if (!mContent) {
		return (s.mContent) ? -1: 0;
	}
	if (!s.mContent) {
		return 1;
	}
	int l = MIN(mLength, s.mLength);
	l = MIN(l, aMax);
	int i;
	for (i=0; i<l; i++) {
		int r = compareChar(mContent[i], s.mContent[i]);
		if (r) return r;
	}
	if (i == aMax) return 0;
	if (mLength < s.mLength) return -1;
	if (mLength == s.mLength) return 0;
	return 1;
}

int String::compareTo(const Object *o) const
{
	assert(getObjectID() == o->getObjectID());
	return compare(*((String *)o));
}

uint String::countChar(char c) const
{
	int i = 0;
	uint n = 0;
	while (i < mLength) {
		if (compareChar(mContent[i], c) == 0) n++;
		i++;
	}
	return n;
}

/**
 *	Crops the string to contain a maximum of |aNewLength| characters.
 */
void String::crop(int aNewLength)
{
	if ((aNewLength >= 0) && (aNewLength < mLength)) realloc(aNewLength);
}

/**
 *	Deletes |aLength| characters at |pos|
 */
void String::del(int pos, int aLength)
{
	if (pos < 0) {
		aLength += pos;
		pos = 0;
	}
	if (aLength <= 0 || pos >= mLength) return;
	if (pos+aLength >= mLength) aLength = mLength-pos;
	if (!aLength) return;
	if (pos + aLength < mLength) {
		memmove(&mContent[pos], &mContent[pos+aLength], mLength-aLength-pos);
	}
	realloc(mLength-aLength);
}

/**
 *	Escapes certains characters in a c-style manner (all characters < 0x20).
 *	@param aSpecialChars characters that need a \
 *	@param bit7 hex encode (\x..) characters >127
 */
void String::escape(const char *aSpecialChars, bool bit7)
{
	if (!mLength) return;
	String copy(this);
	realloc(mLength*4);
	realloc(escape_special((char*)mContent, mLength+1, copy.mContent,
					   copy.mLength, aSpecialChars, bit7));
}

/**
 *   Search forwards for |c| in string
 *	@param c character to search for
 *	@param start first character position to look for
 *	@returns position of character or number < 0 if not found
 */
int String::findCharFwd(char c, int start, int ith_match) const
{
	if (!mLength) return -1;
	if (start >= mLength) return -1;
	if (start < 0) start = 0;
	for (int i=start; i < mLength; i++) {
		if (compareChar(mContent[i], c) == 0) {
			if (ith_match <= 1) return i;
			ith_match--;
		}
	}
	return -1;
}

/**
 *   Search backwards for |c| in string
 *	@param c character to search for
 *	@param start first character position to look for
 *	@returns position of character or number < 0 if not found
 */
int String::findCharBwd(char c, int start, int ith_match) const
{
	if (!mLength) return -1;
	if (start >= mLength) return -1;
	if (start < 0) start = mLength-1;
	for (int i=start; i>=0; i--) {
		if (compareChar(mContent[i], c) == 0) {
			if (ith_match <= 1) return i;
			ith_match--;
		}
	}
	return -1;
}

/**
 *   Search forwards for |s| in string
 *	@param s string to search for
 *	@param start first character position to look for
 *	@returns position of character or number < 0 if not found
 */
int String::findStringFwd(const String &s, int start, int ith_match) const
{
	if (start < 0) start = 0;
	if (!mLength || !s.mLength || (start+s.mLength > mLength)) return -1;
	for (int i=start; (i+s.mLength <= mLength); i++) {
		for (int j=i; (j>=0) && (j+s.mLength <= mLength) && (j-i < s.mLength); j++) {
			if (compareChar(mContent[j], s.mContent[j-i])) goto notfound;
				if (ith_match <= 0) return i;
				ith_match--;
		}
		if (ith_match <= 1) return i;
		ith_match--;
notfound:;
	}
	return -1;
}

/**
 *   Search backwards for |s| in string
 *	@param s string to search for
 *	@param start first character position to look for
 *	@returns position of character or number < 0 if not found
 */
int String::findStringBwd(const String &s, int start, int ith_match) const
{
	assert("not yet implemented" && 0);
	if (!mLength) return -1;
	return -1;
}

/**
 *	Assign s, destroy s
 */
void String::grab(String &s)
{
	free(mContent);
	mLength = s.mLength;
	mContent = s.mContent;
	s.mLength = 0;
	s.mContent = NULL;
	s.realloc(0);
}

/**
 *	Get the content of the String.
 *	The original String gets destroyed.
 *	It's up to you to free the content.
 */
byte *String::grabContent()
{
	byte *res = mContent;
	mContent = NULL;
	return res;
}

char *String::grabContentChar()
{
	char *res = (char*)mContent;
	mContent = NULL;
	return res;
}

/**
 *	inserts |s| at postion |pos| in string.
 */
void String::insert(const String &s, int pos)
{
	if (pos > mLength || pos < 0)
		throw MsgException("index out of bounds");
	if (!s.mLength) return;
	realloc(mLength+s.mLength);
	if (mLength-s.mLength-pos > 0)
		memmove(&mContent[pos+s.mLength], &mContent[pos], mLength-s.mLength-pos);
	memmove(&mContent[pos], s.mContent, s.mLength);
}

bool String::leftSplit(char chr, String &initial, String &rem) const
{
	int pivot = findCharFwd(chr);
	if (pivot < 0) {
		initial = *this;
		rem.clear();
		return false;
	}
	subString(0, pivot, initial);
	subString(pivot+1, length(), rem);
	return true;
}

void String::load(ObjectStream &s)
{
#if 0
	mContent = NULL;
	GET_INT32D(s, mLength);
	realloc(mLength);
	GET_BINARY(s, mContent, mLength);
#else
	GET_LSTRING(s, mContent, mLength);
	realloc(mLength);
#endif	
}

ObjectID String::getObjectID() const
{
	return OBJID_STRING;
}

void String::realloc(int aNewSize)
{
	mLength = aNewSize;
	mContent = (byte*)::realloc(mContent, mLength+1);
	if (!mContent) throw std::bad_alloc();
	mContent[mLength] = 0;
/*	if (mContent) {
		if (aNewSize) {
			mContent = (byte*)::realloc(mContent, aNewSize);
		} else {
			free(mContent);
			mContent = NULL;
		}
	} else {
		if (aNewSize) {
			mContent = (byte*)::malloc(aNewSize);
		}
	}
	mLength = aNewSize;*/
}

#define MAX_REGEX_MATCHES	32

bool String::regexMatch(const String &aRegEx, Container *resultStrings) const
{
	const char *re = aRegEx.toString();
	bool result = false;
	regex_t rx;

	int r = regcomp(&rx, re, REG_EXTENDED | ((compareChar('A','a')==0) ? REG_ICASE : 0));
	if (r) throw MsgException("EINVAL");

	regmatch_t pmatch[MAX_REGEX_MATCHES];
	if (regexec(&rx, (char*)mContent, MAX_REGEX_MATCHES, pmatch, 0) != 0) return false;

	if (resultStrings) {
		for (int i=1; i<MAX_REGEX_MATCHES; i++) {
			if (pmatch[i].rm_so == -1) break;
			String *s = new String();
			subString(pmatch[i].rm_so, pmatch[i].rm_eo-pmatch[i].rm_so, *s);
			resultStrings->insert(s);
		}
	}

	delete re;
	return result;
}

/**
 *	replaces all occurences of |what| in string with |with|
 *	@param what searchstring
 *	@param with replacement
 *	@returns number of replacements
 */
int String::replace(String &what, String &with)
{
	int p = findStringFwd(what);
	int whatlen = what.length();
	int withlen = with.length();
	int numRepl = 0;
	while (p >= 0) {
		if (whatlen == withlen) {
			// replace in situ
			memmove(&mContent[p], with.mContent, withlen);
		} else {
			del(p, whatlen);
			insert(with, p);
		}
		numRepl++;
		p = findStringFwd(what, p+withlen);
	}
	return numRepl;
}

bool String::rightSplit(char chr, String &initial, String &rem) const
{
	int pivot = findCharBwd(chr);
	if (pivot < 0) {
		initial = *this;
		rem.clear();
		return false;
	}
	subString(0, pivot, initial);
	subString(pivot+1, length(), rem);
	return true;
}

/**
 *   assigns result to the substring with
 *	|aLength| characters starting at |aStart|.
 *	@param result will hold the result
 *	@param aStart position of first character in new string
 *	@param aLength number of characters to copy
 *	@returns number of characters copied
 */
int String::subString(int aStart, int aLength, String &result) const
{
	if (aLength <= 0 || aStart >= mLength) {
		result.clear();
		return 0;
	}
	if (aStart+aLength >= mLength) aLength = mLength-aStart;
	result.assign(&mContent[aStart], aLength);
	return aLength;
}

void String::store(ObjectStream &s) const
{
#if 0
	PUT_INT32D(s, mLength);
	s.putCommentf("%y", this);
	PUT_BINARY(s, mContent, mLength);
#else
	PUT_LSTRING(s, mContent, mLength);
#endif
}

/**
 *
 */
void String::transformCase(StringCase c)
{
	if (c==stringCaseCaps) {
	} else {
		for (int i=0; i<mLength; i++) {
			if (c==stringCaseLower) {
				mContent[i]=tolower(mContent[i]);
			} else {
				mContent[i]=toupper(mContent[i]);
			}
		}
	}
}

/**
 *	replace all characters of |inAlpha| in string with the corrensponding
 *	characters of |outAlpha|. The lengths of |inAlpha| and |outAlpha| must
 *	be identical. |inAlpha| should not contain the same characters multiple
 *	times.
 */
void String::translate(const String &inAlpha, const String &outAlpha)
{
	assert(inAlpha.mLength == outAlpha.mLength);
	if (inAlpha.isEmpty() || isEmpty()) return;
	byte tr[256];
	for (int i=0; i<256; i++) tr[i] = i;
	for (int i=0; i<inAlpha.mLength; i++) {
		tr[inAlpha.mContent[i]] = outAlpha.mContent[i];
	}
	for (int i=0; i<mLength; i++) {
		mContent[i] = tr[mContent[i]];
	}
}

/**
 *
 */
int String::toArray(byte *buf, int buflen) const
{
	if (buflen <= 0) return 0;
	int r = MIN(mLength, buflen-1);
	memcpy(buf, mContent, r);
	return r;
}

/**
 *
 */
bool String::toInt32(uint32 &u32, int defaultbase) const
{
	const char *b = (const char*)mContent;
	uint64 u64;
	if (!str2int(b, u64, defaultbase) && !parseIntStr(b, u64, defaultbase)) return false;
	u32 = u64;
	return true;
}

/**
 *
 */
bool String::toInt64(uint64 &u64, int defaultbase) const
{
	const char *b = (const char*)mContent;
	return str2int(b, u64, defaultbase) || parseIntStr(b, u64, defaultbase);
}

int String::toString(char *buf, int buflen) const
{
	if (buflen <= 0) return 0;
	int r = MIN(mLength, buflen-1);
	for (int i=0; i<r; i++) {
		if (mContent[i]) {
			buf[i] = mContent[i];
		} else {
			buf[i] = ' ';
		}
	}
	buf[r] = 0;
	return r;
}

/**
 *	all '\0' will be converted to ' ' (space)
 *	@returns new[] allocated char *
 */
char *String::toString() const
{
	char *s = new char[mLength+1];
	for (int i=0; i<mLength; i++) {
		if (mContent[i]) {
			s[i] = mContent[i];
		} else {
			s[i] = ' ';
		}
	}
	s[mLength] = 0;
	return s;
}

/**
 *	reverses action of <code>escape</code>.
 *	Note that output isnt defined if string wasnt escaped before.
 */
void String::unescape()
{
	String copy(this);
	realloc(unescape_special(mContent, mLength, (char*)copy.mContent));
}

String &String::operator +=(char c)
{
	realloc(mLength+1);
	mContent[mLength-1] = c;
	return *this;
}

/*
 *	global
 */

String operator +(const String &s1, const String &s2)
{
	String temp = String(s1);
	temp.append(s2);
	return temp;
}

String operator +(const char *s1, const String &s2)
{
	String temp = String(s1);
	temp.append(s2);
	return temp;
}

/*
 *	CLASS IString
 */

IString::IString()
{
}

IString *IString::clone() const
{
	IString *r = new IString();
	*r = *this;
	return r;
}

int IString::compareChar(char c1, char c2) const
{
	c1 = tolower(c1);
	c2 = tolower(c2);
	return String::compareChar(c1, c2);
}

ObjectID IString::getObjectID() const
{
	return OBJID_ISTRING;
}
