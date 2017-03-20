#!/bin/sh

lcov --capture --directory . --output-file test.info
genhtml -f --demangle-cpp --legend --num-spaces 4 -s test.info --output-directory cover
