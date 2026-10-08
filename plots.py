import matplotlib.pyplot as plt
sizes = ['1K', '5K', '10K', '25K', '50K']

dijkstra_vertices = [443,   2246,  2739,  7191,  16124]
astar_vertices    = [207,   1096,  1214,  3379,  6681]

dijkstra_edges    = [1024,  5675,  6826,  19330, 43893]
astar_edges       = [493,   2866,  3136,  9395,  18854]

dijkstra_time     = [0,     1,     1,     4,     10]
astar_time        = [0,     0,     0,     3,     6]

plt.figure(1)
plt.plot(sizes, dijkstra_vertices, marker='o', label='Dijkstra')
plt.plot(sizes, astar_vertices,    marker='o', label='A*')
plt.title('Avg Vertices Explored vs Graph Size')
plt.xlabel('Graph Size')
plt.ylabel('Vertices Explored')
plt.legend()
plt.grid(True)
plt.savefig('plot_vertices.png')

plt.figure(2)
plt.plot(sizes, dijkstra_edges, marker='o', label='Dijkstra')
plt.plot(sizes, astar_edges,    marker='o', label='A*')
plt.title('Avg Edges Examined vs Graph Size')
plt.xlabel('Graph Size')
plt.ylabel('Edges Examined')
plt.legend()
plt.grid(True)
plt.savefig('plot_edges.png')

plt.figure(3)
plt.plot(sizes, dijkstra_time, marker='o', label='Dijkstra')
plt.plot(sizes, astar_time,    marker='o', label='A*')
plt.title('Avg Execution Time vs Graph Size')
plt.xlabel('Graph Size')
plt.ylabel('Time (ms)')
plt.legend()
plt.grid(True)
plt.savefig('plot_time.png')
plt.show()
