# I wanted to make sure the graph is acyclic before writing my
# solution (it seems like it from the description, but I wanted to
# make sure)

from pathlib import Path
import networkx as nx

path = Path(__file__).parent
graph_text = open(path/"input.txt").read().strip()

G = nx.DiGraph()
for line in graph_text.splitlines():
    vertex, neighbors = line.split(": ")
    for neighbor in neighbors.split():
        G.add_edge(vertex.strip(), neighbor.strip())

print(f"Is acyclic: {'Yes' if nx.is_directed_acyclic_graph(G) else 'No'}")
print(f"Is undirected version acyclic: "
      f"{'Yes' if nx.is_directed_acyclic_graph(G.to_undirected()) else 'No'}")
