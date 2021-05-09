#!/bin/sh
echo "compiling the source code"
mkdir -p bin
javac -classpath ./lib/sootclasses-2.5.0.jar:./lib/jasminclasses-2.5.0.jar:./lib/polyglotclasses-1.3.5.jar -d ./bin `find ./src -name "*java"`
