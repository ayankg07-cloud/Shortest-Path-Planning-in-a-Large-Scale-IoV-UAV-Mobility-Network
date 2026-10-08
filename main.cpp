#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <queue>
#include <chrono>
#include<cmath>
#include<random>

using namespace std;

struct Edge
{
    int to;
    int weight;
};

struct Nodedist
{
    int node;
    long long dist;
    bool operator>(const Nodedist& other) const {
        return dist > other.dist;
    }
};
struct Coordinate
{
    long long longitude;
    long long latitude;
};
struct Metrics
{
    long long cost;
    int ver_explored;
    long long edges_examined;
    long long time;
    bool found;
};
long long heuristic(int u, int dest, const vector<Coordinate>& coords) {

    const double R = 6371000.0;
    const double PI = 3.14159265358979323846;

    double lat1 = coords[u].latitude / 1e6;
    double lon1 = coords[u].longitude / 1e6;

    double lat2 = coords[dest].latitude / 1e6;
    double lon2 = coords[dest].longitude / 1e6;

    double ref_lat = (lat1 + lat2) / 2.0;
    ref_lat *= PI / 180.0; 
    double x1 = R * (lon1 * PI / 180.0) * cos(ref_lat);
    double y1 = R * (lat1 * PI / 180.0);

    double x2 = R * (lon2 * PI / 180.0) * cos(ref_lat);
    double y2 = R * (lat2 * PI / 180.0);
    double res = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    return (long long)(res * 10 * 0.8);
}

void a_star (const vector<vector<Edge>>& adj_list, const vector<Coordinate>& coords, int source, int dest) {
    int n = adj_list.size();
    vector<long long> dist(n, -1);
    vector<int> parent(n, -1);
    priority_queue<Nodedist, vector<Nodedist>, greater<Nodedist>> pq;
    int ver_explored = 0;
    long long edges_examined = 0;
    auto start_time = chrono::high_resolution_clock::now();
    pq.push({source, 0 + heuristic(source, dest, coords)});
    dist[source] = 0;
    while (!pq.empty()) {
        Nodedist curr = pq.top();
        pq.pop();
        int u = curr.node;
        long long d = curr.dist;
        ver_explored++;
        if (u == dest) {
            break;
        }
        for (const Edge& edge : adj_list[u]) {
            edges_examined++;
            int v = edge.to;
            long long weight = edge.weight;
            if (dist[v] == -1 || dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push({v, dist[v] + heuristic(v, dest, coords)});
            }
        }
    }
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "A* ALGORITHM" << endl;
    cout << "Source: " << source << " | Destination: " << dest << endl;
    
    if (dist[dest] == -1) {
        cout << "No path found between " << source << " and " << dest << "." << endl;
    } 
    else{
        cout << "shortest-path cost: " << dist[dest] << endl;
        vector<int> path;
        for (int at = dest; at != -1; at = parent[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        cout << "Complete sequence of vertices in the shortest path:\n";
        if (path.size() <= 20) {
            for (int i = 0; i < path.size(); i++) {
                cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
            }
            cout << endl;
        } 
        else {
            for (int i = 0; i < 10; i++) cout << path[i] << " -> ";
            cout << "\n    ... (" << path.size() - 20 << " more nodes) ... \n    -> ";
            for (int i = path.size() - 10; i < path.size(); i++) {
                cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
            }
            cout << "\n    (Total path length: " << path.size() << " nodes)" << endl;
        }
    }
    cout << "Number of vertices explored: " << ver_explored << endl;
    cout << "Number of edges examined:    " << edges_examined << endl;
    cout << "Execution time:              " << duration.count() << " ms" << endl;
}
Metrics a_star_exp (const vector<vector<Edge>>& adj_list, const vector<Coordinate>& coords, int source, int dest,int max_nodes) {
    int n = adj_list.size();
    vector<long long> dist(n, -1);
    priority_queue<Nodedist, vector<Nodedist>, greater<Nodedist>> pq;
    int ver_explored = 0;
    long long edges_examined = 0;
    auto start_time = chrono::high_resolution_clock::now();
    pq.push({source, 0 + heuristic(source, dest, coords)});
    dist[source] = 0;
    while (!pq.empty()) {
        Nodedist curr = pq.top();
        pq.pop();
        int u = curr.node;
        long long d = curr.dist;
        ver_explored++;
        if (u == dest) {
            break;
        }
        for (const Edge& edge : adj_list[u]) {
            int v = edge.to;
            if (v > max_nodes) continue;
            edges_examined++;
            long long weight = edge.weight;
            if (dist[v] == -1 || dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({v, dist[v] + heuristic(v, dest, coords)});
            }
        }
    }
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    return {dist[dest], ver_explored, edges_examined, (long long)duration.count(), dist[dest] != -1};
 
}
void dijkstra (const vector<vector<Edge>>& adj_list, int source, int dest) {
    int n = adj_list.size();
    vector<long long> dist(n, -1);
    vector<int> parent(n, -1);
    priority_queue<Nodedist, vector<Nodedist>, greater<Nodedist>> pq;
    int ver_explored = 0;
    long long edges_examined = 0;
    auto start_time = chrono::high_resolution_clock::now();
    pq.push({source, 0});
    dist[source] = 0;
    while (!pq.empty()) {
        Nodedist curr = pq.top();
        pq.pop();
        int u = curr.node;
        long long d = curr.dist;
        if (dist[u] != -1 && d > dist[u]) continue;
        ver_explored++;
        if (u == dest) {
            break;
        }
        for (const Edge& edge : adj_list[u]) {
            edges_examined++;
            int v = edge.to;
            long long weight = edge.weight;
            if (dist[v] == -1 || dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push({v, dist[v]});
            }
        }
    }
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "DIJKSTRA'S ALGORITHM" << endl;
    cout << "Source: " << source << " | Destination: " << dest << endl;
    
    if (dist[dest] == -1) {
        cout << "No path found between " << source << " and " << dest << "." << endl;
    } 
    else{
        cout << "shortest-path cost: " << dist[dest] << endl;
        vector<int> path;
        for (int at = dest; at != -1; at = parent[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        cout << "Complete sequence of vertices in the shortest path:\n";
        if (path.size() <= 20) {
            for (int i = 0; i < path.size(); i++) {
                cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
            }
            cout << endl;
        } 
        else {
            for (int i = 0; i < 10; i++) cout << path[i] << " -> ";
            cout << "\n    ... (" << path.size() - 20 << " more nodes) ... \n    -> ";
            for (int i = path.size() - 10; i < path.size(); i++) {
                cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
            }
            cout << "\n    (Total path length: " << path.size() << " nodes)" << endl;
        }
    }
    cout << "Number of vertices explored: " << ver_explored << endl;
    cout << "Number of edges examined:    " << edges_examined << endl;
    cout << "Execution time:              " << duration.count() << " ms" << endl;
}
Metrics dijkstra_exp (const vector<vector<Edge>>& adj_list, int source, int dest, int max_nodes) {
    int n = adj_list.size();
    vector<long long> dist(n, -1);
    priority_queue<Nodedist, vector<Nodedist>, greater<Nodedist>> pq;
    int ver_explored = 0;
    long long edges_examined = 0;
    auto start_time = chrono::high_resolution_clock::now();
    pq.push({source, 0});
    dist[source] = 0;
    while (!pq.empty()) {
        Nodedist curr = pq.top();
        pq.pop();
        int u = curr.node;
        long long d = curr.dist;
        if (dist[u] != -1 && d > dist[u]) continue;
        ver_explored++;
        if (u == dest) {
            break;
        }
        for (const Edge& edge : adj_list[u]) {
            int v = edge.to;
            if (v > max_nodes) continue;
            edges_examined++;
            long long weight = edge.weight;
            if (dist[v] == -1 || dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({v, dist[v]});
            }
        }
    }
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    return {dist[dest], ver_explored, edges_examined, (long long)duration.count(), dist[dest] !=-1};
}
int main() {
    string filename = "C:/Users/ayank/OneDrive/Desktop/DAA/shortest path project/raw data/USA-road-d.NY.gr";
    ifstream file(filename);
    if(!file.is_open()) {
        cout << "Error in openning file" << filename << endl;
        return 0;
    }
    int num_vertices = 0;
    int num_edges = 0;
    vector<vector<Edge>> adj_list;
    string line;
    while (getline(file,line)) {
        if (line.empty()) continue;
        if (line[0] == 'c') {
            continue;
        }
        else if (line[0] == 'p') {
            stringstream ss(line);
            string p, sp;
            ss >> p >> sp >> num_vertices >> num_edges;
            adj_list.resize(num_vertices + 1);

        }
        else if (line[0] == 'a') {
            stringstream ss(line);
            string a;
            int u, v, w;
            ss >> a >> u >> v >> w;
            adj_list[u].push_back({v, w});
        }
    }
    file.close();  
    string file2 = "C:/Users/ayank/OneDrive/Desktop/DAA/shortest path project/raw data/USA-road-d.NY.co";
    ifstream file_co(file2);
    if(!file_co.is_open()) {
        cout << "Error in openning file" << file2 << endl;
        return 0;
    }
    vector<Coordinate> coords(num_vertices + 1);
    while (getline(file_co,line)) {
        if (line.empty()) continue;
        if (line[0] == 'c' || line[0] == 'p') {
            continue;
        }
        else if (line[0] == 'v') {
            stringstream ss(line);
            string v;
            int u, lon, lat;
            ss >> v >> u >> lon >> lat;
            coords[u].latitude=lat;
            coords[u].longitude=lon;
        }
    }
    file_co.close();
    ofstream out("result.txt");
    out << "| Graph Size | Algorithm | Avg Shortest-Path Cost | Avg Vertices Explored | Avg Edges Examined | Avg Exec Time (ms) |\n";
    out << "|------------|-----------|------------------------|-----------------------|--------------------|--------------------|\n"; 
    int sizes[] = {1000, 5000, 10000, 25000, 50000};
    int pairs_per_size = 200;
    srand(42);
    for (int size : sizes) {
        long long d_tot_cost = 0, d_tot_vert = 0, d_tot_edges = 0, d_tot_time = 0;
        long long a_tot_cost = 0, a_tot_vert = 0, a_tot_edges = 0, a_tot_time = 0;
        int pairs = 0;
        while (pairs < pairs_per_size) {
            int src = (rand() % size) + 1;
            int dest = (rand() % size) + 1;
            if (src == dest) continue;
            Metrics  d_res = dijkstra_exp(adj_list, src, dest, size);
            Metrics a_res = a_star_exp(adj_list, coords, src, dest, size);
            if (d_res.found) {
                d_tot_cost += d_res.cost;
                d_tot_vert += d_res.ver_explored;
                d_tot_edges += d_res.edges_examined;
                d_tot_time += d_res.time;
                a_tot_cost += a_res.cost;
                a_tot_vert += a_res.ver_explored;
                a_tot_edges += a_res.edges_examined;
                a_tot_time += a_res.time;
                pairs++;
            }
        }
        out << "| " << size << " | Dijkstra | " 
            << d_tot_cost / pairs << " | "
            << d_tot_vert / pairs << " | "
            << d_tot_edges / pairs << " | "
            << d_tot_time / pairs << " |\n";
        out << "| " << size << " | A*       | " 
            << a_tot_cost / pairs << " | "
            << a_tot_vert / pairs << " | "
            << a_tot_edges / pairs << " | "
            << a_tot_time / pairs << " |\n";
    }
    out.close();
    return 0;
}


