


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
2. Java 1.8 or higher
3. apache ant

## Directory Structure

```
AE/
|--- README.md			This README 
|--- Alias_Analysis/	Source code of the tool.
|--- benchmarks/		The benchmark files in .dot format.
```

## Steps

The overall workflow is pretty simple. Follow the steps below.


```
./Alias_Analysis/compile
Alias_Analysis/build/main.out 
```

This runs the D1 odot D1 algorithm followed by the D1 odot Dk algorithm, printing tables with the results in the terminal.
