import sys, queue

# based on algorithm 1 as defined in
# https://helloqirun.github.io/papers/pldi2013_qirun.pdf


def run_experiment(file):
    graph = set()
    nodes = set()
    q = queue.Queue()
    g2 = set()

    def add(v):
        if v not in graph and v not in g2:
            g2.add(v)
            q.put(v)

    with open(file, 'r') as f:
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
                id = t[1]
                
                nodes.add(a)
                nodes.add(b)

                if label == "ob":
                    graph.add((a, b, "["))  
                    graph.add((b, a, "]"))
                elif label == "cb":
                    graph.add((b, a, "["))
                    graph.add((a, b, "]"))
                elif label == "op":
                    graph.add((a, b, "("))
                    graph.add((b, a, ")"))
                elif label == "cp":
                    graph.add((b, a, "("))
                    graph.add((a, b, ")"))


    for n in nodes:
        add((n, n, "S"))

    for e in g2:
        graph.add(e)
    g2 = set()


    #grammar:
    #S -> eps | ( A | [ B | SS
    #A -> S )
    #B -> S ]

    while not q.empty():
        (s, e, l) = q.get() #start, end, label
        
        
        if l == "A":
            for (s1, e1, l1) in graph:
                if e1 == s and l1 == "(":
                    #S -> ( A
                    add((s1, e, "S"))
        elif l == "B":
            for (s1, e1, l1) in graph:
                if e1 == s and l1 == "[":
                    #S -> [ B
                    add((s1, e, "S"))
        elif l == "S":
            for (s1, e1, l1) in graph:
                if l1 == ")" and e == s1:
                    #A -> S )
                    add((s, e1, "A"))
                elif l1 == "]" and e == s1:
                    #B -> S ]
                    add((s, e1, "B"))
                elif l1 == "S" and e == s1:
                    #S -> SS
                    add((s, e1, "S"))
                elif l1 == "S" and s == e1:
                    #S -> SS (but we found the other S)
                    add((s1,e, "S"))
        for e in g2:
            graph.add(e)
        g2 = set()

    res = 0
    for (s, e, l) in graph:
        if l == "S" and s < e:
            res+=1

    print("D' reachability for " + file + ": " + str(res))


tests = [
    "antlr",
    "bloat",
    "chart",
    "eclipse",
    "fop",
    "hsqldb",
    "jython",
    "luindex",
    "lusearch",
    "pmd",
    "xalan"
    ]

for test in tests:
    run_experiment( "./Alias_Analysis/spg/reduced_bench/" + test + "_reduced.dot")