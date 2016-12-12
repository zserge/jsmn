
JSMN
====

jsmn (pronounced like 'jasmine') is a minimalistic JSON parser in C.  It can be
easily integrated into resource-limited or embedded projects.

You can find more information about JSON format at [json.org][1]

Library sources are available at https://github.com/zserge/jsmn

The web page with some information about jsmn can be found at
[http://zserge.com/jsmn.html][2]

Philosophy
----------

Most JSON parsers offer you a bunch of functions to load JSON data, parse it
and extract any value by its name. jsmn proves that checking the correctness of
every JSON packet or allocating temporary objects to store parsed JSON fields
often is an overkill. 

JSON format itself is extremely simple, so why should we complicate it?

jsmn is designed to be	**robust** (it should work fine even with erroneous
data), **fast** (it should parse data on the fly), **portable** (no superfluous
dependencies or non-standard C extensions). And of course, **simplicity** is a
key feature - simple code style, simple algorithm, simple integration into
other projects.

Features
--------

* compatible with C89
* no dependencies (even libc!)
* highly portable (tested on x86/amd64, ARM, AVR)
* about 200 lines of code
* extremely small code footprint
* API contains only 2 functions
* no dynamic memory allocation
* incremental single-pass parsing
* library code is covered with unit-tests

Design
------

The rudimentary jsmn object is a **token**. Let's consider a JSON string:

	'{ "name" : "Jack", "age" : 27 }'

It holds the following tokens:

* Object: `{ "name" : "Jack", "age" : 27}` (the whole object)
* Strings: `"name"`, `"Jack"`, `"age"` (keys and some values)
* Number: `27`

JSMN builds meta-tokens that point to token boundaries in the JSON
string and list their types and relationships.
In the example above jsmn will create tokens like:
Object [0..31] (skip 5), String [3..7] (skip 2),
String [12..16] (skip 1), String [20..23] (skip 2),
Number [27..29] (skip 1).

Every jsmn token has a type, which indicates the type of corresponding JSON
token. jsmn supports the following token types:

* Object - a container of key-value pairs, e.g.:
	`{ "foo":"bar", "x":0.3 }`
* Array - a sequence of values, e.g.:
	`[ 1, 2, 3 ]`
* String - a quoted sequence of chars, e.g.: `"foo"`
* Primitive - a number, a boolean (`true`, `false`) or `null`

Besides textual start/end positions, all jsmn tokens
also have a skip count, so you can easily jump to the object's
next sibling.  There's no trouble finding children of objects,
since the first child always follows the parent immediately.

```C
jsmntok_t *c, *p = tokens;
for (c = p+1; c < p+p->skip; c += c->skip) {
    printf("%.*s", FMT_STR(str_buffer, c));
}
```

WARNING: Before parsing is complete, the skip count
of all incomplete tokens is less than or equal to zero.
This will cause real trouble if you naively iterate on partial
parses.  After a successful parse, skip
counts are all >= 1, obviously.

This approach provides full access to the JSON hierarchy
and makes it possible to use zero-copy techniques.

Install
-------

To clone the repository you should have Git installed. Just run:

	$ git clone https://github.com/zserge/jsmn

Repository layout is simple: jsmn.c and jsmn.h are library files, tests are in
the jsmn\_test.c, you will also find README, LICENSE and Makefile files inside.

To build the library, run `make`. It is also recommended to run `make test`.
Let me know, if some tests fail.

If build was successful, you should get a `libjsmn.a` library.
The header file you should include is called `"jsmn.h"`.

API
---

Token types are described by `jsmntype_t`:

	typedef enum {
		JSMN_UNDEFINED = 0,
		JSMN_OBJECT = 1,
		JSMN_ARRAY = 2,
		JSMN_STRING = 3,
		JSMN_PRIMITIVE = 4
	} jsmntype_t;

**Note:** Unlike JSON data types, primitive tokens are not divided into
numbers, booleans and null, because one can easily tell the type using the
first character:

* <code>'t', 'f'</code> - boolean 
* <code>'n'</code> - null
* <code>'-', '0'..'9'</code> - number

Token is an object of `jsmntok_t` type:

	typedef struct {
		jsmntype_t type; // Token type
		int start;       // Token start position
		int end;         // Token end position
		int size;        // Number of child (nested) tokens
	} jsmntok_t;

**Note:** string tokens point to the first character after
the opening quote and the previous symbol before final quote. This was made 
to simplify string extraction from JSON data.

All job is done by `jsmn_parser` object. You can initialize a new parser using:

	jsmn_parser parser;
	jsmntok_t tokens[10];

	jsmn_init(&parser);

	// js - pointer to JSON string
	// tokens - an array of tokens available
	// 10 - number of tokens available
	jsmn_parse(&parser, js, strlen(js), tokens, 10);

This will create a parser, and then it tries to parse up to 10 JSON tokens from
the `js` string.

A non-negative return value of `jsmn_parse` is the number of tokens actually
used by the parser.
Passing NULL instead of the tokens array would not store parsing results, but
instead the function will return the value of tokens needed to parse the given
string. This can be useful if you don't know yet how many tokens to allocate.

If something goes wrong, you will get an error. Error will be one of these:

* `JSMN_ERROR_INVAL` - bad token, JSON string is corrupted
* `JSMN_ERROR_NOMEM` - not enough tokens, JSON string is too large
* `JSMN_ERROR_PART` - JSON string is too short, expecting more JSON data

Partial Parsing
----------

The complete parser state (including current position) is held inside
`jsmn_parser parser`, and only numerical offsets to your
tokens are ever stored.  Each time it's called, the parser consumes
all new characters and never back-tracks.

This means that if you get `JSON_ERROR_NOMEM`, you can re-allocate
more tokens (copying over your existing ones) and call
`jsmn_parse` once more so the parser can resume its work.

If you read json data from a stream, you can
periodically call `jsmn_parse` and try again with more data
if the return value is `JSON_ERROR_PART`.
You will most likely alternate between `JSON_ERROR_PART` and  `JSMN_ERROR_NOMEM`
until you reach the end of JSON data or encounter `JSMN_ERROR_INVAL`.
Note that since the parser outputs locations in the input buffer,
it always indexes from the very beginning of the stream.  If you re-use
your buffer you will need code like,
```C
        n1 = fill(buf, five_words);
	if(jsmn_parse(&parser, buf, n, tokens, 128) == JSMN_ERROR_PART) {
                n2 = fill(buf, five_more_words);
                jsmn_parse(&parser, buf-n1, n1+n2, tokens, 128);
        }
```


Other info
----------

This software is distributed under [MIT license](http://www.opensource.org/licenses/mit-license.php),
 so feel free to integrate it in your commercial products.

[1]: http://www.json.org/
[2]: http://zserge.com/jsmn.html
