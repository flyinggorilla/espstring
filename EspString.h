/*
  WString.h - String library for Wiring & Arduino
 ...mostly rewritten by Paul Stoffregen...
 Copyright (c) 2009-10 Hernando Barragan.  All right reserved.
 Copyright 2011, Paul Stoffregen, paul@pjrc.com

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 ---------
 
 Modifications/extensions by flyinggorilla
 
 */

#ifndef String_class_h
#define String_class_h
#ifdef __cplusplus

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "pgmspace.h"

// An inherited class for holding the result of a concatenation.  These
// result objects are assumed to be writable by subsequent concatenations.
class StringSumHelper;

// an abstract class used as a means to proide a unique pointer type
// but really has no body
class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))

// The string class
class String
{
    // use a function pointer to allow for "if (s)" without the
    // complications of an operator bool(). for more information, see:
    // http://www.artima.com/cppsource/safebool.html
    typedef void (String::*StringIfHelperType)() const;
    void StringIfHelper() const
    {
    }

public:
    /* @brief  just  frees allocated memory and initializes String object.
    */
    //void clean() to empty the string but keep the allocated memory as is
    //void wipe() like clean, but also initializes the buffer with zeroes 
    //bool setLength(unsigned int length) used when data is set via .c_str() to then correct the length; set the terminating zero too
    //unsigned int capacity() returns allocated memory-1 (to keep space for trailing 0)
    //

    //ADDED!!!
    /* @brief   frees allocated memory and initializes String object.
     * TODO: SHOULD BE RENEAMED TO free() or reset(), as reset() would better fit the truncate to 0 length
    */
    void reset() { invalidate(); };

    //ADDED!!!
    /* @brief stream-like printf method that appends the "printf" formatted output to the String data
     * use reset() if you dont want to append but simply reuse an existing String object 
     *             and make sure we dont allocate more memory then necessary
     * @param   format     normal printf style format strings
     * @return  the number of characters that have been written, not counting the terminating null character.
     *          If an encoding error occurs or memory allocation failed, a negative number is returned.
     */
    int printf(const char* format, ...);

    //TEMP ADDED!! DEBUGGING
    //void dump();


    /*
     * @brief   Pre-allocates memory and initializes it with zeroes. Deletes any existing String data.
     *          This method is particularly useful to setup a buffer that receives string or binary data.
                That buffer can be accessed with c_str() and length() methods.
     * @param   size  allocates minimum size+1 bytes memory, writes the memory with zeroes, and sets length() to size
     * @return  true (1) if memory allocation was successful, false (0) otherwise
     */
    unsigned char resize(unsigned int size);

    // memory management
    // return true on success, false on failure (in which case, the string
    // is left unchanged).  reserve(0), if successful, will validate an
    // invalid string (i.e., "if (s)" will be true afterwards)
    unsigned char reserve(unsigned int size);

    // prepares the buffer for receiving data via memcopy or the like. 
    // first, the internal buffer is guaranteed to have size+1; 
    // buffer is resized if too small, but will never shrink!
    // capacity will be set to size, so that the buffer will always have an additional terminating zero
    // finally, the buffer is always zeroed. 
    // call setlength(actuallength) after copying data to a c_str() buffer access.
    unsigned char prepare(unsigned int size);

    // useful when copying data into buffer via c_str() access, to then update the string/data length 
    // if length is larger than the capacity, then length is set to capacity
    // a terminating 0 is written at buffer[length]
    unsigned char setlength(unsigned int length);


    // writes a terminating 0 at the beginning and resets the length to 0; 
    // does NOT free the memory, so to avoid another malloc(); 
    // use reset() if you intend to free the memory
    void clear() { setlength(0); };

    // returns the allocated capacity writable to the c_str() accessible internal buffer
    // capacity + 1 == internal buffer size. , so that there is always room for the terminating 0
    unsigned int capacity(void);


    // constructors
    // creates a copy of the initial value.
    // if the initial value is null or invalid, or if memory allocation
    // fails, the string will be marked as invalid (i.e. "if (s)" will
    // be false).
    String(const char *cstr = "");
    String(const String &str);
    String(const __FlashStringHelper *str) : String(reinterpret_cast<const char *>(str)) {};
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    String(String &&rval);
    String(StringSumHelper &&rval);
#endif
    explicit String(char c);
    explicit String(unsigned char, unsigned char base = 10);
    explicit String(int, unsigned char base = 10);
    explicit String(unsigned int, unsigned char base = 10);
    explicit String(long, unsigned char base = 10);
    explicit String(unsigned long, unsigned char base = 10);
    explicit String(float, unsigned char decimalPlaces = 2);
    explicit String(double, unsigned char decimalPlaces = 2);
    ~String(void);


    inline unsigned int length(void) const
    {
        if(buffer) {
            return len;
        } else {
            return 0;
        }
    }

    // creates a copy of the assigned value.  if the value is null or
    // invalid, or if the memory allocation fails, the string will be
    // marked as invalid ("if (s)" will be false).
    String & operator =(const String &rhs);
    String & operator =(const char *cstr);
    String & operator = (const __FlashStringHelper *str);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    String & operator =(String &&rval);
    String & operator =(StringSumHelper &&rval);
#endif

    // concatenate (works w/ built-in types)

    // returns true on success, false on failure (in which case, the string
    // is left unchanged).  if the argument is null or invalid, the
    // concatenation is considered unsucessful.
    unsigned char concat(const String &str);
    unsigned char concat(const char *cstr);
    unsigned char concat(char c);
    unsigned char concat(unsigned char c);
    unsigned char concat(int num);
    unsigned char concat(unsigned int num);
    unsigned char concat(long num);
    unsigned char concat(long long num);
    unsigned char concat(unsigned long num);
    unsigned char concat(float num);
    unsigned char concat(double num);
    unsigned char concat(const __FlashStringHelper * str);

    // if there's not enough memory for the concatenated value, the string
    // will be left unchanged (but this isn't signalled in any way)
    String & operator +=(const String &rhs)
    {
        concat(rhs);
        return (*this);
    }
    String & operator +=(const char *cstr)
    {
        concat(cstr);
        return (*this);
    }
    String & operator +=(char c)
    {
        concat(c);
        return (*this);
    }
    String & operator +=(unsigned char num)
    {
        concat(num);
        return (*this);
    }
    String & operator +=(int num)
    {
        concat(num);
        return (*this);
    }
    String & operator +=(unsigned int num)
    {
        concat(num);
        return (*this);
    }
    String & operator +=(long num)
    {
        concat(num);
        return (*this);
    }
    String & operator +=(long long num)
    {
        concat(num);
        return (*this);
    }
    String & operator +=(unsigned long num)
    {
        concat(num);
        return (*this);
    }
    String & operator +=(float num)
    {
        concat(num);
        return (*this);
    }
    String & operator +=(double num)
    {
        concat(num);
        return (*this);
    }
    String & operator += (const __FlashStringHelper *str)
    {
        concat(str);
        return (*this);
    }

    friend StringSumHelper & operator +(const StringSumHelper &lhs, const String &rhs);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, const char *cstr);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, char c);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, unsigned char num);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, int num);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, unsigned int num);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, long num);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, unsigned long num);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, float num);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, double num);
    friend StringSumHelper & operator +(const StringSumHelper &lhs, const __FlashStringHelper *rhs);

    // comparison (only works w/ Strings and "strings")
    operator StringIfHelperType() const
    {
        return buffer ? &String::StringIfHelper : 0;
    }
    int compareTo(const String &s) const;
    unsigned char equals(const String &s) const;
    unsigned char equals(const char *cstr) const;
    unsigned char operator ==(const String &rhs) const
    {
        return equals(rhs);
    }
    unsigned char operator ==(const char *cstr) const
    {
        return equals(cstr);
    }
    unsigned char operator !=(const String &rhs) const
    {
        return !equals(rhs);
    }
    unsigned char operator !=(const char *cstr) const
    {
        return !equals(cstr);
    }
    unsigned char operator <(const String &rhs) const;
    unsigned char operator >(const String &rhs) const;
    unsigned char operator <=(const String &rhs) const;
    unsigned char operator >=(const String &rhs) const;
    unsigned char equalsIgnoreCase(const String &s) const;
    unsigned char startsWith(const String &prefix) const;
    unsigned char startsWith(const String &prefix, unsigned int offset) const;
    unsigned char endsWith(const String &suffix) const;

    // character acccess
    char charAt(unsigned int index) const;
    void setCharAt(unsigned int index, char c);
    char operator [](unsigned int index) const;
    char& operator [](unsigned int index);
    void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index = 0) const;
    void toCharArray(char *buf, unsigned int bufsize, unsigned int index = 0) const
    {
        getBytes((unsigned char *) buf, bufsize, index);
    }
    
    const char * c_str() const
    {
        return buffer ? buffer : "";  //CHANGED!!!! DONT RETURN NULL
    }

    const char * c_str(unsigned int index) const
    {
        if (index >= len) {
            return "";
        }
        return buffer + index;  
    }

    // search
    int indexOf(char ch) const;
    int indexOf(char ch, unsigned int fromIndex) const;
    int indexOf(const String &str) const;
    int indexOf(const String &str, unsigned int fromIndex) const;

    bool contains(const char* match);
    bool contains(const String &match);

    int lastIndexOf(char ch) const;
    int lastIndexOf(char ch, unsigned int fromIndex) const;
    int lastIndexOf(const String &str) const;
    int lastIndexOf(const String &str, unsigned int fromIndex) const;
    String substring(unsigned int beginIndex) const
    {
        return substring(beginIndex, len);
    }
    ;
    String substring(unsigned int beginIndex, unsigned int endIndex) const;

    // modification
    void replace(char find, char replace);
    void replace(const String& find, const String& replace);
    /*void remove(unsigned int index);
    void remove(unsigned int index, unsigned int count); */
    void toLowerCase(void);
    void toUpperCase(void);

    // trims whitespaces as of isspace() 
    // For the "C" locale, white-space characters are any of:
    // ' '	(0x20)	space (SPC)
    // '\t'	(0x09)	horizontal tab (TAB)
    // '\n'	(0x0a)	newline (LF)
    // '\v'	(0x0b)	vertical tab (VT)
    // '\f'	(0x0c)	feed (FF)
    // '\r'	(0x0d)	carriage return (CR)
    void trim(void);

    // parsing/conversion
    long toInt(void) const;
    float toFloat(void) const;

    bool toFloat(float &value);
    bool toDouble(double &value);
    bool toInt(int &value);
    bool toLong(long &value);

    unsigned char concat(const char *cstr, unsigned int length);

protected:
    char *buffer;	        // the actual char array
    unsigned int mCapacity;  // the array length minus one (for the '\0')
    unsigned int len;       // the String length (not counting the '\0')
protected:
    void init(void);
    void invalidate(void);
    unsigned char changeBuffer(unsigned int maxStrLen);

    // copy and move
    String & copy(const char *cstr, unsigned int length);
    String & copy(const __FlashStringHelper *pstr, unsigned int length);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    void move(String &rhs);
#endif
};

class StringSumHelper: public String
{
public:
    StringSumHelper(const String &s) :
        String(s)
    {
    }
    StringSumHelper(const char *p) :
        String(p)
    {
    }
    StringSumHelper(char c) :
        String(c)
    {
    }
    StringSumHelper(unsigned char num) :
        String(num)
    {
    }
    StringSumHelper(int num) :
        String(num)
    {
    }
    StringSumHelper(unsigned int num) :
        String(num)
    {
    }
    StringSumHelper(long num) :
        String(num)
    {
    }
    StringSumHelper(unsigned long num) :
        String(num)
    {
    }
    StringSumHelper(float num) :
        String(num)
    {
    }
    StringSumHelper(double num) :
        String(num)
    {
    }
};

#endif  // __cplusplus
#endif  // String_class_h
