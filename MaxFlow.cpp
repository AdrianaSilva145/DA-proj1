#include "MaxFlow.h"
#include <queue>
#include <limits>
#include <unordered_map>

template <class T>
int MaxFlow::edmondsKarp(Graph<T> &g, T sourceInfo, T sinkInfo) {

    Vertex<T>* source = g.findVertex(sourceInfo);
    Vertex<T>* sink   = g.findVertex(sinkInfo);

    if (!source || !sink)
        return 0;

    int maxFlow = 0;

    while (true) {

        // BFS
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

        // Encontrar fluxo mínimo
        double augFlow = std::numeric_limits<double>::max();
        Vertex<T>* v = sink;

        while (v != source) {
            Edge<T>* e = parent[v];
            augFlow = std::min(augFlow, e->getWeight() - e->getFlow());
            v = e->getOrig();
        }

        // Atualizar fluxos
        v = sink;
        while (v != source) {
            Edge<T>* e = parent[v];
            e->setFlow(e->getFlow() + augFlow);
            e->getReverse()->setFlow(e->getReverse()->getFlow() - augFlow);
            v = e->getOrig();
        }

        maxFlow += augFlow;
    }

    return maxFlow;
}
