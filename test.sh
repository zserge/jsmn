#!/bin/bash

#
# Test script is organized like this:
#   o two variables (PASSED and FAILED) hold the total 
#     number of passed/faled tests
#   o expect() function performs a single test. First 
#     argument of the function is the variable name and
#     the second is an expected value. PASSED/FAILED
#     values are updated automatically
#
# Most tests look like:
#     |
#     | expect "varName" "expectedValue" << JSON_END
#     | ..json data here...
#     | JSON_END
#     |
#

PASSED=0
FAILED=0

function expect() {	
	ret=$(./demo -t 10 -b 256 - | grep -A 1 "$1" | tail -n 1 | cut -c 20-)
	if [ "x$ret" = "x$2" ]; then
		PASSED=$(($PASSED+1))
	else
		echo "Failed: $1 != $2"
		FAILED=$(($FAILED+1))
	fi
}


#
# TEST SET: Basic types (simple values)
#
expect 'boolVar' 'true' << JSON_END
"boolVar" : true
JSON_END

expect 'boolVar' 'false'<< JSON_END
"boolVar" : false
JSON_END

expect 'intVar' '12345' << JSON_END
"intVar" : 12345
JSON_END

expect 'floatVar' '12.345' << JSON_END
"floatVar" : 12.345
JSON_END

expect 'nullVar' 'null' << JSON_END
"nullVar" : null
JSON_END

expect 'strVar' 'hello' << JSON_END
"strVar" : "hello"
JSON_END

#
# TEST SET: Simple types (boundary values)
#

expect 'intVar' '0' << JSON_END
"intVar" : 0
JSON_END

expect 'intVar' '-0' << JSON_END
"intVar" : -0
JSON_END

expect 'floarVar' '-0.0' << JSON_END
"floarVar" : -0.0
JSON_END

expect 'strVar' '\n\r\b\t \u1234' << JSON_END
"strVar" : "\n\r\b\t \u1234"
JSON_END

expect 'strVar' '' << JSON_END
"strVar" : ""
JSON_END

#
# TEST SET: Array types
#
expect 'arr' '[1,2,3,4,5]' << JSON_END
"arr" : [1,2,3,4,5]
JSON_END

expect 'arr' '[1, 2.3, "4", null, true]' << JSON_END
"arr" : [1, 2.3, "4", null, true]
JSON_END

expect 'arr' '[]' << JSON_END
"arr" : []
JSON_END

#
# TEST SET: Object types
#
expect 'obj' '{"a":"b"}' << JSON_END
"obj":{"a":"b"}
JSON_END

expect 'objField' 'value' << JSON_END
{
	"foo" : "bar",
	"objField" : "value"
}
JSON_END

expect 'foo' 'bar' << JSON_END
{
	"foo" : "bar"
	"a" : [
		{
			"x" : "y",
			"z" : "zz"
		},
		3,
		false,
		true,
		"end"
	],
}
JSON_END

echo
echo "Passed: $PASSED"
echo "Failed: $FAILED"
echo 

