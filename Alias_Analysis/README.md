# Field-Sensitive Alias Analysis

This is a tool for static analysis of java programs using Bidirectional dyck reachabiliy for interprocedural field-sensitive alias analysis using Symbolic Point-to Graphs. This tool has been tested on openJDK-7 Runtime Environment 7u131, Apache Ant version 1.9.3 and g++ version 4.8.4.

## Getting Started

### Prerequisites

1. Apache ant version 1.7.0 or above. To install Apache ant you can run the following command

```bash
sudo apt-get install ant
```

2. g++ a \*nix-based C++ compiler. You can install it using

```
sudo apt-get install g++
``` 

3. Java Development Kit (JDK). You can install it using 

``` 
sudo apt-get install openjdk-7-jdk
```

### Configuring

1. Set the path to the directory containing java runtime (JRE) library in the file spa/build.xml in the property called "LIB". Instructions on setting path is given in the file itself.

2. Give executable permission to the script file present in spa and base directory.

## Running on Programs

for the general Usage of the tool or for knowing the names of the benchmark use 
```bash
./run.sh --help
```

### Running on benchmarks

To run on any of the DaCapo-2006-10-MR2 Benchmarks, run the command understated. Benchmark names are mentioned in the file Benchmark_list.txt. 
```bash
# ./run --benchmark <benchmark_name>
./run --benchmark antlr #example
```

### Running on general programs 

To run the tool on any general program you will need classpath of the program (contains .class files) to be analysed and the name of the Main Class (Entry point into your program).
```bash
# ./run --classpath <classpath> --main_entry <Main class>
./run --classpath some/path/bin --main_entry com.package.Main #example
```

### Running if SPG file present

We have provided the SPG files of the benchmark in spg directory. You can directly execute the alias analysis by 
```
./complile
build/main.out <path/to/SPG/file>
```

## Acknowledgments

This tool uses the tool provided by [Guoqing Xu et al.](https://www.ics.uci.edu/~guoqingx/tools/alias.htm) to build the Symbolic Point-to graphs, which inturn uses soot to provide them information necessary for SPG generation. 