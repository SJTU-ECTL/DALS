/**
 * @file dinic.cpp
 * @brief
 * @author Nathan Zhou
 * @date 2019-01-30
 * @bug No known bugs.
 */

#include <dinic.h>

Dinic::Dinic(int N) : N(N), G(N, std::vector<int>()), level(N, 0), pt(N, 0), res_visited(N, false) {}

void Dinic::AddEdge(int u, int v, double cap) {
    if (u != v) {
        E.emplace_back(Edge(u, v, cap));
        G[u].emplace_back(E.size() - 1);
        E.emplace_back(Edge(v, u, 0));
        G[v].emplace_back(E.size() - 1);
    }
}

bool Dinic::BFS(int S, int T) {
    std::queue<int> q({S});
    fill(level.begin(), level.end(), N + 1);
    level[S] = 0;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        if (u == T) break;
        for (int k : G[u]) {
            Edge &e = E[k];
            if (e.flow < e.cap && level[e.v] > level[e.u] + 1) {
                level[e.v] = level[e.u] + 1;
                q.emplace(e.v);
            }
        }
    }
    return level[T] != N + 1;
}

double Dinic::DFS(int u, int T, double flow) {
    if (u == T || flow == 0) return flow;
    for (int &i = pt[u]; i < (int) G[u].size(); ++i) {
        Edge &e = E[G[u][i]];
        Edge &oe = E[G[u][i] ^ 1];
        if (level[e.v] == level[e.u] + 1) {
            double amt = e.cap - e.flow;
            if (flow != -1 && amt > flow) amt = flow;
            if (double pushed = DFS(e.v, T, amt)) {
                e.flow += pushed;
                oe.flow -= pushed;
                return pushed;
            }
        }
    }
    return 0;
}

double Dinic::MaxFlow(int S, int T) {
    double total = 0;
    while (BFS(S, T)) {
        fill(pt.begin(), pt.end(), 0);
        while (double flow = DFS(S, T))
            total += flow;
    }
    return total;
}

void Dinic::DFSResidualNetwork(int u) {
    res_visited[u] = true;
    for (int i : G[u]) {
        Edge &e = E[i];
        if (!res_visited[e.v]) {
            if (e.cap > 0 && e.cap > e.flow)
                DFSResidualNetwork(e.v);
            else if (e.cap == 0 && e.flow < 0)
                DFSResidualNetwork(e.v);
        }
    }
}

std::vector<Edge> Dinic::MinCut(int S, int T) {
    MaxFlow(S, T);
    std::vector<Edge> min_cut;
    DFSResidualNetwork(S);
    for (auto e : E)
        if (e.cap > 0 && e.flow > 0 && res_visited[e.u] && !res_visited[e.v])
            min_cut.push_back(e);
    return min_cut;
}
