# Shortest-Path Project — README

## What is this project?

This project compares two shortest-path algorithms — **Dijkstra** and **A\*** — on a real road network of New York City. The goal is to see which algorithm finds the shortest route faster and more efficiently.

---

## Dataset

I used the **New York road network** from the 9th DIMACS Implementation Challenge.

Download the dataset from here:
> https://www.diag.uniroma1.it/challenge9/download.shtml

After downloading, place these two files inside a folder called `raw data` in the same folder as `main.cpp`:
- `USA-road-d.NY.gr` — the road network (nodes and edges)
- `USA-road-d.NY.co` — the GPS coordinates of every node

---

## How to Run the Project

### Step 1 — Compile the code
Open a terminal in the project folder and run:
```
g++ main.cpp -o main.exe
```

### Step 2 — Run the program
```
.\main.exe
```

The program will load the dataset, run both algorithms on different graph sizes, and save the results to `result.txt`.

### Step 3 — Generate the plots
Make sure Python is installed, then run:
```
python plots.py
```
This will save 3 graph images (`plot_vertices.png`, `plot_edges.png`, `plot_time.png`) in the same folder.

---

## What does the program actually do?

1. It reads the New York road network file and builds a graph using an **adjacency list**.
2. It also reads the GPS coordinates of every node (needed for the A* heuristic).
3. It runs **200 random source-destination queries** for each of these graph sizes: 1000, 5000, 10000, 25000, and 50000 nodes.
4. It averages the results and saves them to `result.txt`.

---

## Key Results Summary

Both algorithms always produce the **same optimal path cost**. A* is faster because it uses an **equirectangular-projection heuristic** (straight-line distance in metres, scaled by `0.8`) to guide the search toward the destination.

| Graph Size | Algorithm | Avg Cost | Avg Vertices | Avg Edges | Avg Time (ms) |
|------------|-----------|----------|--------------|-----------|---------------|
| 1,000 | Dijkstra | 53,548 | 443 | 1,024 | 0 |
| 1,000 | A* | 53,548 | 207 | 493 | 0 |
| 50,000 | Dijkstra | 155,444 | 16,124 | 43,893 | 10 |
| 50,000 | A* | 155,444 | 6,681 | 18,854 | 6 |

---

## Files in this project

| File | What it is |
|------|------------|
| `main.cpp` | The main C++ code (graph construction + Dijkstra + A* + experiments) |
| `result.txt` | The experimental results table |
| `Report.md` | The written report |
| `plots.py` | Python script to generate comparison graphs |
| `README.md` | This file |
| `raw data/` | Folder containing the dataset files |

---

## Note on reproducibility

The random source-destination pairs are always generated with the same seed (`srand(42)`), so it will produce the same results every time the program is executed.
