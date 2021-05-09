#!/bin/bash

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
	CLASS_PATH="$2"
	shift # past argument
	;;
	-bm|--benchmark)
	BENCHMARK="$2"
	shift # past argument
	;;
	-m|--main_entry)
	MAIN_PROG="$2"
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

# if [[ $# -ne 0 ]]; then
# 	Usage
# 	exit 1
# fi


./compile.sh
if [ $? -ne 0 ]; then 
	exit 1
fi

echo CLASS_PATH  = "${CLASS_PATH}"
echo BENCHMARK     = "${BENCHMARK}"
echo MAIN_PROG    = "${MAIN_PROG}"

if [[ "$CLASS_PATH" != "" && "$MAIN_PROG" != "" ]]; then 
	cd spa/
	ant -Dmain_entry=$MAIN_PROG -Dprogram_cp=$CLASS_PATH run.myprog
	if [ $? -ne 0 ]; then
		echo "problem generating spg graph."
		exit 1
	fi
	cd ../
	echo ""
	./build/main.out "./spg/${MAIN_PROG}.spg"
elif [[ "$BENCHMARK" != "" ]]; then
	cd spa/
	ant -Dbname=$BENCHMARK run.benchmark
	if [ $? -ne 0 ]; then 
		echo "problem generating spg graph."
		exit 1
	fi
	cd ../
	echo ""
	./build/main.out "./spg/${BENCHMARK}.spg"
else
	Usage
	exit 3
fi