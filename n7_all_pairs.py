
import sys

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
    edges.append((b,a,inv(l))) # handles bidirectedness
    nodes.add(a)
    nodes.add(b)


#actual algorithm


#n is the number of nodes in the graph
n = len(nodes)

print("n is " + str(n))

#helper functions
def in_edges(node):
    return [x for x in edges if x[1] == node]

def out_edges(node):
    return [x for x in edges if x[0] == node]

def CO_summary(open, close, other_open, other_close, nodes, edges):
    #a summary is a tuple (start, end, a, b)
    # where start and end are nodes
    # and a and b are labels

    res = list() #TODO maybe guard against duplicates?
    queue = list()
    def add(start, end, i, j):
        v = (start, end, i, j)
        if not v in res:
            res.append(v)
            queue.append(v)

    #includes upper and lower bounds
    def inside(low, high, *l):
        for k in l:
            if k < low or k > high:
                return False
        return True

    #first rule: C_i O_i -> \eps
    #Note: the text says C_0 O_0, but this does not make sense?
    for i in nodes:
        add(i,i, 0, 0)

    while queue:
        (start, end, i, j) = queue.pop(0) #dequeue one element

        # C_i O_j -> "[" C_i O_j "]"
        if 0 <= i and i <= 6*n and 0 <= j and j <= 6*n:
            #this is worse than it needs to be, but fixing means
            #creating an equivalent CFL grammar
            inn = [x for x in in_edges(start) if x[2] == other_open]
            out = [x for x in out_edges(end) if x[2] == other_close]
            for x in inn:
                for y in out:
                    add(x[0], y[1], i, j)

        # C_{i+1} O_j -> ")" C_i O_j for 0<=i<=6n-1
        if 0 <= i and i <= 6*n-1:
            inn = [x for x in in_edges(start) if x[2] == close]
            for x in inn:
                add(x[0], end, i+1, j)

        # C_i O_{j+1} -> C_i O_j for 0<=j<=6n-1
        if 0 <= j and j <= 6*n-1:
            out = [x for x in out_edges(end) if x[2] == open]
            for x in out:
                add(start, x[1], i, j+1)

        # C_i O_j -> C_a O_b   C_c O_d
        # where i = a + max(c-b, 0), j = d + max(b-c, 0)
        # and 0<=i,j,a,b,c,d <= 6n
        # (this criterion should always hold)
        # THIS CRITERION DOES NOT ALWAYS HOLD
        lis = [x for x in res if x[0] == end]
        for x in lis:
            (_, e, c, d) = x
            i_new = a + max(c-b, 0)
            j_new = d + max(b-c, 0)
            if inside(0, 6*n, i, j, a, b, c, d):
                add(start, e, i_new, j_new)
    return res

#compute CO summary for parentheses (pp. 59:16)
S1 = CO_summary("(", ")", "[", "]", nodes, edges)

print("S1:")
print(S1)
print([x for x in S1 if x[2] == 0])

#compute CO summary for square brackets (pp. 59:16)
S2 = CO_summary("[", "]", "(", ")", nodes, edges)

print("S2:")
print(S2)
#implementation of "precise D1 dot D1 algorithm"

sol = set()
Gp = set()

def put(a,b):
    Gp.add((a,b))
    Gp.add((b,a))

print("computing actual algorithm")

for (u, v, label) in [x for x in edges if x[2] == "("]:
    # [1, 6*n-1]
    for j in range(1, 6*n):
        # [i, 6*n]
        for k in range(1, 6*n+1):
            put((u,j,k),(v,j+1,k))

print("Performed step 1")

for (u, v, label) in [x for x in edges if x[2] == "["]:
    # [1, 6*n-1]
    for j in range(1, 6*n):
        # [i, 6*n]
        for k in range(1, 6*n+1):
            put((u,j,k),(v,j,k+1))


print("Performed step 2")

for (u, v, a, b) in S1:
    for j in range(a, min(6*n-b+a, 6*n) + 1):
        for k in range(1, 6*n+1):
            put((u,j,k),(v,j-a+b,k))


print("Performed step 3")

for (u, v, a, b) in S2:
    for j in range(1, 6*n+1):
        for k in range(a, min(6*n-b+a, 6*n)+1):
            put((u,j,k),(v,j,k-a+b))


print("Performed step 4")

# do ordinary all pairs reachability
zeroes = [x for x in Gp if x[1] == 0 and x[2] == 0]

print(len(Gp))

print([x for x in Gp if x[1] == 0 and x[2] == 0])

for a in nodes:
    z = (a, 0, 0)
    reached = set()
    queue = [z]
    while queue:
        pos = queue.pop(0)
        for o in Gp:
            if o[0] == pos:
                if not o[1] in reached and not o[1] in queue:
                    reached.add(o[1])
                    queue.append(o[1])
                    print(o[1])
    print(str(z) + " reaches:")
    print(reached)

#print(zeroes)

#print(Gp)
