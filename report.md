# Shortest-Path Planning in a Large-Scale IoV/UAV Mobility Network

## Part A: Dataset Selection

**1. Dataset name:**
9th DIMACS Implementation Challenge: Shortest Paths, TIGER/Line graph (New York road network)

**2. Source or URL:**
https://www.diag.uniroma1.it/challenge9/download.shtml

**3. Brief description of the dataset:**
This dataset is the road network of New York, taken from the US Census Bureau's TIGER/Line data. Intersections are nodes and road segments are arcs. There are separate files for physical distances (`USA-road-d`) and travel times (`USA-road-t`), plus a coordinates file (`.co`).

**4. Number of records, nodes, and edges used:**
- Nodes: 264,346
- Edges (arcs): 733,846
- Total records: 733,853

**5. How the dataset is converted into a graph:**
The `.gr` file is a simple edge list:
- Lines starting with `c` are comments.
- The line `p sp <nodes> <edges>` tells the size of the graph.
- Lines starting with `a <u> <v> <w>` are directed edges from `u` to `v` with weight `w`.

In the C++ program the file is read line by line to build an adjacency list as a `vector<vector<Edge>>` of size `num_vertices + 1`, since node IDs start from 1. For every arc line `Edge{v, w}` is pushed into `adj_list[u]`.And the same is done for the `.co` file.

**6. What each vertex represents:**
Each vertex is a real intersection in the New York road network. Its longitude and latitude are given in the `.co` file.

**7. What each edge represents:**
Each edge is a road segment connecting two intersections, so together the edges define which routes a vehicle can actually take.

**8. What edge weight is used:**
The physical distance from `USA-road-d.NY.gr` is used here. The weight is the length of the road segment between two intersections, in the DIMACS standard unit.

**9. Why the selected dataset is suitable for this problem:**
It is a real, large, publicly available mobility network with 264,346 nodes, making it easy to sample subgraphs of different sizes for scalable benchmarking. The companion coordinate file provides real GPS locations for every vertex, enabling the computation of an admissible distance-based heuristic for A*. This directly models ground-level IoV navigation and 2D waypoint routing.

---

## Preprocessing Details


1. **Opened the `.gr` file** — The program opens `USA-road-d.NY.gr` and reads it line by line.

2. **Skipped comment lines** — Any line starting with `c` is a comment. These are completely ignored.

3. **Read the graph size** — The line starting with `p` tells us how many nodes and edges the graph has. This is used to set up the adjacency list in memory.

4. **Built the adjacency list** — Every line starting with `a` represents a road segment. For example, `a 1 2 803` means there is a road from node 1 to node 2 with a distance of 803. The program reads these and stores them in a `vector<vector<Edge>>` adjacency list.

5. **Loaded GPS coordinates** — The program also opens `USA-road-d.NY.co` and reads the longitude and latitude of every node. These coordinates are stored separately and used by the A* algorithm to estimate the straight-line distance to the destination.

---

## Part F: Analysis

**1. Why is the dataset you selected suitable for shortest-path analysis?**

The New York road network is a strong fit for several reasons. It is a real-world graph with 264,346 nodes and 733,846 edges, providing sufficient scale for algorithmic performance differences to clearly emerge. The structure is natural: nodes represent intersections and edges have physical distance weights, which directly reflects Internet of Vehicles (IoV) ground routing and provides a representative benchmark for fixed-altitude waypoint navigation in UAV flight corridors. Furthermore, every node has geographic coordinates that allow building an admissible heuristic for A*, and the standardized DIMACS format makes parsing consistent and reproducible.

---

**2. How did you transform the raw dataset into a weighted graph?**

I parsed the `.gr` file line by line in C++. Comment lines (starting with `c`) were ignored. The `p` line gave me the number of vertices and edges, which I used to size the adjacency list as a `vector<vector<Edge>>` of `num_vertices + 1` entries (1-indexed). For each `a` line I read `u`, `v` and `w` and added `Edge{v, w}` to `adj_list[u]`. I parsed the `.co` file in the same way to get each node's longitude and latitude for the A* heuristic.

---

**3. Do Dijkstra's algorithm and A* produce the same optimal path cost in your experiments? Explain.**

Yes. Both algorithms gave identical shortest-path costs at every graph size (e.g., 53,548 at size 1,000 and 155,444 at size 50,000). This is because the heuristic used is admissible — it never overestimates the true road distance. If the heuristic is admissible, A* cannot settle early on a sub-optimal path, so it is guaranteed to return the same optimal cost as Dijkstra. The results confirm this at every graph size.

---

**4. Which algorithm explores fewer vertices, and why?**

A* explored noticeably fewer vertices than Dijkstra at every size. On the 50,000-node subgraph, Dijkstra explored about **16,124 vertices** on average while A* explored only **6,681**, which is roughly 2.4x fewer. Similarly for edges: Dijkstra examined **43,893** edges versus A*'s **18,854**.

The reason is that Dijkstra spreads out from the source in all directions like a ripple, because it has no idea where the destination is. A* picks nodes using `f(v) = g(v) + h(v)`, where `h(v)` estimates the remaining distance to the destination. So it prefers nodes that are geometrically closer to the target and mostly ignores the ones heading the wrong way.

---

**5. How does execution time vary as the graph size increases?**

Execution time grows with graph size. For the smallest subgraph (1,000 nodes) both algorithms completed in under 1 ms. At 50,000 nodes, Dijkstra averaged about **10 ms** while A* averaged about **6 ms**. Even at 25,000 nodes the gap is visible: Dijkstra took **4 ms** versus A*'s **3 ms**. This fits the `O((V + E) log V)` complexity: a bigger graph means more priority-queue operations and more time. A* grows more slowly in practice because it visits fewer vertices and stops earlier.

---

**6. Explain the effect of the graph's density on shortest-path computation.**

Density represents the proportion of edges relative to vertices in the graph. In a denser graph, each vertex has more outgoing neighbors, so every relaxation step checks more edges, increasing priority queue operations and overall runtime. In this New York road network, the average out-degree is about **2.78** ($E / V = 733,846 / 264,346$) and the average total degree (in-degree + out-degree) is **5.55**, which is very sparse. This sparsity is typical for physical road systems where an intersection rarely connects to more than 3 or 4 roads. Because the graph is sparse, both Dijkstra and A* execute efficiently. On a dense graph (such as a complete graph), the number of edge relaxations would grow rapidly ($O(V^2)$), substantially increasing the overhead of heap updates.

---

**7. What is the theoretical time complexity of your Dijkstra implementation?**

It is **O((V + E) log V)**, where `V` is the number of vertices and `E` is the number of edges.

- With an adjacency list, each edge is looked at at most once, which gives `O(E)` work.
- The priority queue is C++'s `std::priority_queue`, a binary heap, so each insert or extract costs `O(log V)`.
- In the worst case every vertex is extracted once and every edge causes at most one insertion, giving `O(V log V + E log V)`, which simplifies to **O((V + E) log V)**.

---

**8. What heuristic did you use for A*? Is it admissible and/or consistent? Justify your answer.**

**Heuristic used:** An equirectangular-projection distance between the current node and the destination, converted to metres and scaled by `0.8 × 10`:

```
ref_lat = radians((lat_v + lat_dest) / 2)
x1 = R * radians(lon_v)  * cos(ref_lat),  y1 = R * radians(lat_v)
x2 = R * radians(lon_dest) * cos(ref_lat), y2 = R * radians(lat_dest)
h(v) = 0.8 * 10 * sqrt((x2-x1)^2 + (y2-y1)^2)
```

Where coordinates from `USA-road-d.NY.co` are in millionths of a degree (divided by 1e6 before conversion), `R = 6,371,000 m` is Earth's radius, and the factor of 10 converts metres to decimetres (the DIMACS distance unit). Scaling by `0.8` ensures the heuristic remains strictly conservative and admissible.

**Admissibility:** Yes. The equirectangular formula gives the straight-line great-circle approximation, which is always ≤ the actual road distance. Multiplying by `0.8 < 1` makes it even more conservative, so `h(v) ≤ h*(v)` for every node `v` — the definition of admissibility.

**Consistency:** Yes. Consistency requires `h(u) ≤ w(u,v) + h(v)` for every edge `(u, v)`. The Euclidean distance satisfies the triangle inequality, and since the formula is scaled down by `0.8` it stays within the true edge weight on every step. So the heuristic is consistent.

---

**9. What difficulties did you face while converting the real-world dataset into a graph?**

- **Understanding the file format:** I had to separate comment lines (`c`), the problem configuration line (`p`), and the directed arc lines (`a`). Mistaking comment headers or metadata as arcs would corrupt graph construction.
- **1-based indexing:** DIMACS node IDs start from 1 rather than 0, so the adjacency list and coordinate arrays required `num_vertices + 1` entries to prevent out-of-bounds indexing.
- **Coordinate scaling and floating-point conversions:** Coordinates in the `.co` file are given as integers scaled by $10^6$ (e.g., `-73530767` represents $-73.530767^\circ$). To calculate geographic projections accurately without losing precision, they had to be parsed and scaled to `double` degrees before applying trigonometric projection equations.
- **Guaranteeing heuristic admissibility:** Due to planar projection approximations over spherical coordinates, coordinate rounding, and discrete integer distance units, an unscaled straight-line distance could occasionally exceed the recorded edge weight on certain segments. To strictly satisfy admissibility ($h(v) \le h^*(v)$) and consistency across the entire graph, I incorporated a conservative `0.8` scaling factor.

---

**10. Based on your observations, which algorithm would you recommend for large-scale IoV/UAV route planning? Justify your answer.**

I would recommend **A\***, whenever geographic coordinates for network nodes or waypoints are available.

In my experiments, A* explored roughly **2.4x fewer vertices** than Dijkstra (e.g., 6,681 vs 16,124 at 50,000 nodes) and examined about **2.3x fewer edges** (18,854 vs 43,893), completing substantially faster while still guaranteeing the exact optimal path.

In IoV (vehicular networks) and structured UAV mobility (fixed-corridor flight paths), nodes are fundamentally tied to geographic positions. Dijkstra expands blindly in all directions because it relies solely on cumulative edge costs. A* uses spatial coordinates to orient its search toward the destination, drastically reducing unnecessary expansions and making it the superior choice for large-scale route planning.
