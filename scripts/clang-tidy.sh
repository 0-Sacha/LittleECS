#!/bin/bash

./conf/scripts/clang-tidy.py //custom:compile_commands_tests --enable-headers $* --config=./conf/checker/.clang-tidy --config=./conf/linter/RustLike/.clang-tidy
