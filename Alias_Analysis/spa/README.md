# Symbolic Points-to Graph constructor

This is the code that is responsible to generate Symbolic points to graph of java programs given the Main entry of the program and the class path of JDK and the program. It saves the SPG formed in the form of list of edges in ../spg directory.

### Prerequisites

1. Apache ant version 1.7.0 or above. To install Apache ant you can run the following command

```bash
sudo apt-get install ant
```

2. Set the path to the directory containing java runtime (JRE) library in the file build.xml in the property called "LIB". Instructions on setting path is given in the file itself.

3. Give executable permission to script run.sh and compile.sh

### Running on benchmarks

Make sure that you have set up the LIB property in the build.xml correctly. To generate the SPG of any of the DaCapo-2006-10-MR2 benchmarks, run the listed command. 
```bash
# ant -Dbname=<benchmark_name> run.benchmark
ant -Dbname=antlr run.benchmark #example
``` 

You can also use the following command to generate the SPG for all the benchmarks but the method described above is preferred. You have to set the path to the directory containing JRE library in the LIB variable in run.sh file. 

```bash
./run.sh --benchmark <benchmark name> 
```

### Running on general programs 

To get the SPG on any general program you will need classpath of the program ( directory that contains .class files) to be analysed and the name of the Main Class (Entry point of your program). You can also use run.sh.

```bash
# ant -Dmain_entry=<entry_point> -Dprogram_cp=<class path> run.myprog
ant -Dmain_entry=com.package.Main -Dprogram_cp=some/path/bin run.myprog #example
./run.sh --classpath <class_path> --main_entry <Entry_point>
```

### Other Information

For more information you can visit this [link](https://www.ics.uci.edu/~guoqingx/tools/alias.htm).
