


## Overview

**Paper** : The Decidability and Complexity of Interleaved Bidirected Dyck Reachability, POPL'22

**Artifact Outline** - In the above paper, we present an algorithm for solving interleaved, bidirected D1 odot D1 reachability in O(n^3 a(n)), where a is the inverse Ackermann function.
We have implemented this algorithm along with techniques for pre-processing and simplifying input graphs and run it on the Dacapo Benchmarks.

We also present an algorithm for solving interleaved, bidirected D1 odot Dk reachability in O(n^2 a(n)) with O(n) bounded counters.
We have also implemented this algorithm along with pre-processing and simplification and run it on the Dacapo Benchmarks.

We have implemented the algorithms in C++.

Here we describe how to run the implementations and obtain the results found in the paper.


## Requirements

1. Machine with Unix based operating system, such as Ubuntu or MacOS. Our experiments were performed on a machine with about 8GB RAM. 

## Directory Structure

```
root/
|--- README.md			This README 
|--- benchmarks/	        The benchmark files in .dot format.
|--- build/	                The output folder.
|--- include/		        Header files for the C++ project.
|--- src/		        Class files for the C++ project.

```

## Steps

The overall workflow is pretty simple. Run the below commands in a terminal from the root of this project.


```
./compile.sh
build/main.out 
```

This runs the D1 odot D1 algorithm followed by the D1 odot Dk algorithm, printing latex tables with the results in the terminal. 

## Custom Benchmarks

To run your own benchmarks, add your "\<benchmark\>.dot" files to this directory, then run 

```
build/main.out -b <space separated benchmark names>
```

### Benchmark file structure

Each line in a ".dot" file corresponds to an edge. Example:

a->b[label="ob--23"]

Here, an edge from 'a' to 'b' labeled 'ob--23' is declared. There are four types of labels:

 - "ob--i": opening bracket with id $i$, i.e. $[_i$
 - "cb--i": closing bracket with id $i$, i.e. $]_i$
 - "op--i": opening parenthesis with id $i$, i.e. $(_i$
 - "cp--i": closing parenthesis with id $i$, i.e. $)_i$

Brackets and parentheses correspond to each of the Dyck languages. 

Any line without the "->" substring will be skipped.