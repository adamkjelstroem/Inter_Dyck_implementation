#!/bin/bash

### you have to set the value of "LIB" property to  directory containing java runtime environment (JRE) library. Usually this directory contains rt.jar (core.jar), jce.jar etc. among other libraries (classes.jar in case of mac). The path usually for  
##	Windows : C:/Program Files/Java/jre<version>/lib
##	linux : /usr/lib/jvm/java<version>/jre/lib
##	Mac OS : /System/Library/Frameworks/JavaVM.framework/Classes 
LIB=/usr/lib/jvm/java-7-openjdk-amd64/jre/lib   #this the path to the Java Runtime Environment (JRE) library
SOOT_CLASSES=./lib/sootclasses-2.5.0.jar:./lib/jasminclasses-2.5.0.jar:./lib/polyglotclasses-1.3.5.jar #
CLASS_PATHS=./bin:$SOOT_CLASSES   #  directory to .class files that the tool will use
JAVA_FILE=client.datarace.DataraceMain   # MAin entrance to the alias analysis tool
SOOT_PATHS=$LIB/rt.jar:$LIB/jce.jar:$LIB/jsse.jar:    # yeah soot uses different class path, so its path that soot will search for .class files
# ANA_PROG_CP=../java/bin  #argument 2: class files of the program to be analysed, sootpath=SOOT_PATHS:ANA_PROG_CP
# ANA_PROG_MAIN=Solution  #argument 3: main entrance of program to be analysed
DEPS=antlr-deps.jar:bloat.jar:eclipse-deps.jar:fop.jar:jython-deps.jar:luindex.jar:pmd-deps.jar:xalan.jar:antlr.jar:chart-deps.jar:eclipse.jar:hsqldb-deps.jar:jython.jar:lusearch-deps.jar:pmd.jar:bloat-deps.jar:chart.jar:fop-deps.jar:hsqldb.jar:luindex-deps.jar:lusearch.jar:xalan-deps.jar
ANA_PROG_CP=./lib/dacapo-2006-10-MR2.jar:$DEPS    # this is classpath for the program that you want to analyse  

function Usage
{
	echo "Usage: "
	echo "$0 (-bm | --benchmark  <benchmark name>)"
	echo "$0 (-cp | --classpath <class_path>) (-m | --main_entry <Main class>)"
	echo "$0 -h | --help)"
	echo -e "\nthe value of the argument (bm | benchmark) can be "
	echo "1.  antlr
2.  bloat
3.  chart
4.  eclipse
5.  fop
6.  hsqldb
7.  jython
8.  luindex
9.  lusearch
10. pmd
11. xalan"
}   # end of Usage


if [[ $# -eq 0 ]]; then
	Usage
	exit 1
fi

while [[ $# -gt 1 ]]
do
key="$1"

case $key in
	-cp|--classpath)
	ANA_PROG_CP="$2"
	shift # past argument
	;;
	-bm|--benchmark)
	ANA_PROG_MAIN="decapo.$2.Main"
	shift # past argument
	;;
	-m|--main_entry)
	ANA_PROG_MAIN="$2"
	shift # past argument
	;;
	-h|--help)
	H=1
	;;
	*)
	Usage		# unknown option
	exit 2
	;;
esac
shift # past argument or value
done

if [[ "$1" = "--help" || "$1" = "-h" || $H -eq 1  ]]; then
	Usage
	exit 0
fi


./compile.sh 

if [ $? -ne 0 ]; then
	exit 1
fi

echo CLASS_PATH  = "${ANA_PROG_CP}"
echo ENRTY_POINT = "${ANA_PROG_MAIN}"

# Benchmark_names=(antlr bloat chart eclipse fop hsqldb jython luindex lusearch pmd xalan batik eclipse jython sunflow tomcat)

java -Xmx8G -DMayAlias=spa -DTestSummary=1 -DDebug=1 -DMEASURE_SPARK=0 -DUseCache=0 -DBenchName=$i -classpath $CLASS_PATHS $JAVA_FILE $SOOT_PATHS $ANA_PROG_CP $ANA_PROG_MAIN


# To run on the general program set the value of variable ANA_PROG_CP and ANA_PROG_MAIN correctly.
# The entry point of Decapo Benchmarks are decapo.<Benchmark_name>.Main 