import sys, queue


nodes = set()
vertices = set()
q = queue.Queue()

#TODO to make this work, just keep 'out-edges' for a node. If it has two out-edges labeled "(" (to node a and node b), then there's an epsilon between a and b.

with open(sys.argv[1], 'r') as f:
    contents = f.read()


    lines = contents.split("\n")


    for line in lines:
        #978->923[label="op--6338"]
        if "//" in line:
            continue
        if "->" in line:
            t = line.split("->")
            a = int(t[0])
            t = t[1].split("[label=\"")
            b = int(t[0])
            t = t[1].split("\"")[0].split("--")
            label = t[0]
            v = ""
            v2 = ""
            if label == "ob":
                v =  (a, b, "[")
                v2 = (b, a, "]")
            elif label == "cb":
                v =  (b, a, "[")
                v2 = (a, b, "]")
            elif label == "op":
                v =  (a, b, "(")
                v2 = (b, a, ")")
            elif label == "cp":
                v =  (b, a, "(")
                v2 = (a, b, ")")

            vertices.add(v)
            q.put(v)
            vertices.add(v2)
            q.put(v2)

            id = t[1]

            nodes.add(a)
            nodes.add(b)


v2 = set()

def push(v):
    if not v in vertices:
        v2.add(v)
        q.put(v)

# Do parsing
while not q.empty():
    v = q.get()
    for w in vertices:
        if v[1] == w[0]:
            # order is v w
            if (v[2] == "(" and w[2] == ")") or (v[2] == "[" and w[2] == "]"):
                push((v[0], w[1], ""))
            elif v[2] == "":
                push((v[0], w[1], w[2]))
        elif w[1] == v[0]:
            # order is w v
            if (w[2] == "(" and v[2] == ")") or (w[2] == "[" and v[2] == "]"):
                push((w[0], v[1], ""))
            elif v[2] == "":
                push((w[0], v[1], w[2]))

    for x in v2:
        vertices.add(x)

    if q.qsize() % 1000 == 0:
        print(q.qsize())


res = 0

for (a, b, label) in vertices:
    if a < b and label == "":
        res+=1
print(res)

#2923 is res for "antlr_reduced"
