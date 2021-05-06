import sys
import time


#timing logic
t = time.time()
def timer(msg):
    global t
    print(str(time.time()-t) + " s: " + msg)
    t = time.time()

#inverse of "[" is "]" etc
def inv(p):
    if p == "[":
        return "]"
    if p == "]":
        return "["
    if p == "(":
        return ")"
    if p == ")":
        return "("
    return ""

#parse data
def parse(filename):

    #Note: maybe not the best idea for a large file
    with open(filename, 'r') as f:
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

        a = int(a)
        b = int(b)

        edges.append((a,b,l))
        edges.append((b,a,inv(l))) # handles bidirectedness
        nodes.add(a)
        nodes.add(b)
    return nodes, edges

nodes, edges = parse(sys.argv[1])

timer("Done with parsing")


#actual algorithm

#for now, nothing fancy; just implement straightforwardly what the paper says.
# we can always optimize later.

n = len(nodes)

c = 12 * n**2 + 6 * n

print("Nodes of original: " + str(n))
print("Expansion factor: " + str(c))

# construct new graph:
nodes2 = set()
for node in nodes:
    for j in range(c+1):
        #add nodes
        nodes2.add((node, j))


edges2 = set()
for (u, v, l) in edges:
    for j in range(c+1):
        #add edges
        if l == "[":
            if j < c:
                edges2.add(((u, j),(v, j+1),""))
        elif l == "]":
            if j > 0:
                edges2.add(((u,j),(v,j-1),""))
        else:
            #l is "(", ")", or epsilon
            edges2.add(((u,j),(v,j),l))

timer("Done building new graph!")
print("|E|, |V| of new graph: " + str(len(edges2)) + ", " + str(len(nodes2)))


# do D1 reachability
#TODO use better implementation of reachability.

#for now, just add a summary edge

queue = set()
[queue.add(x) for x in edges2]
summary = set()
[summary.add(x) for x in edges2]
while queue:
    if len(queue) % 100 == 0:
        print("queue size: " + str(len(queue)))
    el = queue.pop()
    (u, v, l) = el;
    sum2 = set()
    for edge in summary:
        def a(u, v, l):
            new_edge = (u, v, l)
            if not new_edge in sum2 and not new_edge in summary:
                queue.add(new_edge)
                sum2.add(new_edge)

        (u1, v1, l1) = edge
        if u == v1:
            #"edge" is in-edge of el
            if l == ")":
                if l1 == "":
                    a(u1, v, ")")
                elif l1 == "(":
                    a(u1, v, "")
            elif l == "":
                a(u1, v, l1)
            elif l == "(":
                if l1 == "":
                    a(u1, v, l)
        elif v == u1:
            #"edge" is out-edge of el
            if l == "(":
                if l1 == ")":
                    a(u, v1, "")
                elif l1 == "":
                    a(u, v1, "(")
            elif l == "":
                a(u, v1, l1)
    summary = summary.union(sum2)


timer("Done computing reachability!")

for s in summary:
    (u1, v1, l) = s
    if l == "":
        (u, i) = u1
        (v, j) = v1
        if i == 0 and j == 0 and u != v:
            print("path from " + str(u) + " to " + str(v))

timer("Done printing solutions!")
