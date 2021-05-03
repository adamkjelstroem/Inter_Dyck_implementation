
import sys

#Note: maybe not the best idea for a large file
with open(sys.argv[1], 'r') as f:
    contents = f.read()


lines = contents.split("\n")
lines = [l for l in lines if "->" in l] #trims data

#load edges into data structure
#structure is (start, end, label)
#this is stored as a list
#TODO might need to rework this into different data structure
edges = list()
for line in lines:
    [a, k] = line.split("->")
    [b, l] = k.split("[label=\"")
    l = l[:2]
    #parse to make data look nicer
    if l == "op":
        l = "("
    elif l == "cp":
        l = ")"
    elif l == "ob":
        l = "["
    else:
        l = "]"
    edges.append((a,b,l))


#actual algorithm

#n is the number of nodes in the graph (i'm pretty sure at least)
