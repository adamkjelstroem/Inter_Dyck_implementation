
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
nodes = set()
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

    a = int(a) #maybe not necessary
    b = int(b)
    
    edges.append((a,b,l))
    nodes.add(a)
    nodes.add(b)


#actual algorithm

#n is the number of nodes in the graph (I'm pretty sure at least)
n = len(nodes)

#compute CO summary for parentheses (pp. 59:16)


#compute CO summary for square brackets (pp. 59:16)

#implementation of "precise D1 dot D1 algorithm"
