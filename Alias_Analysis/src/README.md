This directory contains the implementation of the reachability algorithms described in the paper. The entry point is main.cpp which takes the argument a path to SPG file (for example ../spg/antlr.spg) and finds the time taken for dyck reachability by ours and the previous existing algorithm on the graph constrcuted by the file.

### SPG file syntax

SPG file constist of list of edges. each edge in the file is represented by the following syntax. if the name of the node or field type itself contains "||" then it should be escaped. 

```
e || <node 1 name> || <node 2 name> || <field type>
```

to make a comment in the file you can do so by 
```
\\ || <comment>
```

