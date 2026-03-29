#ifndef MAXFLOW_H
#define MAXFLOW_H

#include <queue>
#include <limits>
#include <unordered_map>
#include "Graph.h"

/**
 * @brief Max-Flow algorithms.
 */
struct MaxFlow {

    /**
     * @brief Edmonds-Karp algorithm (BFS-based Max-Flow).
     *        Time Complexity: O(V * E^2)
     * @param g          Flow network graph
     * @param sourceInfo Source node identifier
     * @param sinkInfo   Sink node identifier
     * @return Maximum flow value
     */
    template <class T>
    static int edmondsKarp(Graph<T> &g, T sourceInfo, T sinkInfo) {

        Vertex<T>* source = g.findVertex(sourceInfo);
        Vertex<T>* sink   = g.findVertex(sinkInfo);

        if (!source || !sink)
            return 0;

        int maxFlow = 0;

        while (true) {

            // BFS para encontrar caminho aumentante
            std::queue<Vertex<T>*> q;
            std::unordered_map<Vertex<T>*, Edge<T>*> parent;

            for (auto v : g.getVertexSet())
                v->setVisited(false);

            source->setVisited(true);
            q.push(source);

            bool foundPath = false;

            while (!q.empty() && !foundPath) {
                Vertex<T>* u = q.front();
                q.pop();

                for (Edge<T>* e : u->getAdj()) {
                    double residual = e->getWeight() - e->getFlow();

                    if (residual > 0 && !e->getDest()->isVisited()) {
                        e->getDest()->setVisited(true);
                        parent[e->getDest()] = e;

                        if (e->getDest() == sink) {
                            foundPath = true;
                            break;
                        }

                        q.push(e->getDest());
                    }
                }
            }

            if (!foundPath)
                break;

            // Encontrar bottleneck
            double augFlow = std::numeric_limits<double>::max();
            Vertex<T>* v = sink;

            while (v != source) {
                Edge<T>* e = parent[v];
                augFlow = std::min(augFlow, e->getWeight() - e->getFlow());
                v = e->getOrig();
            }

            // Atualizar fluxos (forward e residual)
            v = sink;
            while (v != source) {
                Edge<T>* e = parent[v];
                e->setFlow(e->getFlow() + augFlow);
                if (e->getReverse())
                    e->getReverse()->setFlow(e->getReverse()->getFlow() - augFlow);
                v = e->getOrig();
            }

            maxFlow += (int)augFlow;
        }

        return maxFlow;
    }
};

#endif // MAXFLOW_H