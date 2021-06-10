import sys, queue


graph = dict()
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

            if not a in graph:
                graph[a] = set()
            if not b in graph:
                graph[b] = set()

            if label == "ob":
                graph[b].add((a, "["))
                q.put((b, a, "["))
            elif label == "cb":
                graph[a].add((b, "["))
                q.put((a, b, "["))
            elif label == "op":
                graph[b].add((a, "("))
                q.put((b, a, "("))
            elif label == "cp":
                graph[a].add((b, "("))
                q.put((a, b, "("))


            id = t[1]


def push(a, b, label):
    if not (b, label) in graph[a]:
        graph[a].add((b,label))
        q.put((a, b, label))

# Do parsing
while not q.empty():
    (a, b, label) = q.get()

    vertices = graph[a].copy()

    if label=="":
        for (c, label2) in vertices:
            push(a, c, label2)
    else:
        for (c, label2) in vertices:
            if label == label2:
                push(a, c, "")
                push(c, a, "")

    if q.qsize() % 1000 == 0:
        print(q.qsize())


res = 0

for a in graph.keys():
    for (b, label) in graph[a]:
        if a < b and label == "":
            res += 1
print(res)
#2923 is res for "antlr_reduced"
