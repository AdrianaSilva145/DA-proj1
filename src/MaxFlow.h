#ifndef MAXFLOW_H
#define MAXFLOW_H

#include <queue>
#include <limits>
#include <unordered_map>
#include "Graph.h"

/**
 * @brief Estrutura que agrupa os algoritmos de fluxo máximo em redes.
 */
struct MaxFlow {

    /**
     * @brief Algoritmo de Edmonds-Karp para cálculo de fluxo máximo.
     *
     * Implementação do algoritmo de Ford-Fulkerson usando BFS para encontrar
     * caminhos aumentantes (variante Edmonds-Karp). Em cada iteração, o BFS
     * encontra o caminho mais curto (em número de arestas) da source ao sink
     * com capacidade residual positiva. O fluxo é então aumentado ao longo
     * desse caminho pelo valor do bottleneck.
     *
     * A rede de fluxo deve ter arestas forward com capacidade w e arestas
     * residuais (reverse) com capacidade 0, ligadas pelo ponteiro reverse.
     * O método Graph::addEdge() já cria automaticamente ambas as arestas.
     *
     * @tparam T     Tipo do identificador dos vértices do grafo.
     * @param g          Grafo de fluxo (modificado internamente com os fluxos).
     * @param sourceInfo Identificador do vértice source.
     * @param sinkInfo   Identificador do vértice sink.
     * @return Valor do fluxo máximo encontrado.
     *
     * @note Complexidade Temporal: O(V * E^2) onde V = vértices, E = arestas.
     *       Esta é a complexidade garantida de Edmonds-Karp independentemente
     *       das capacidades das arestas, ao contrário do Ford-Fulkerson genérico.
     */
    template <class T>
    static int edmondsKarp(Graph<T> &g, T sourceInfo, T sinkInfo) {

        Vertex<T>* source = g.findVertex(sourceInfo);
        Vertex<T>* sink   = g.findVertex(sinkInfo);

        if (!source || !sink)
            return 0;

        int maxFlow = 0;

        while (true) {

            // BFS para encontrar caminho aumentante mais curto
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

            // Calcular o bottleneck do caminho encontrado
            double augFlow = std::numeric_limits<double>::max();
            Vertex<T>* v = sink;

            while (v != source) {
                Edge<T>* e = parent[v];
                augFlow = std::min(augFlow, e->getWeight() - e->getFlow());
                v = e->getOrig();
            }

            // Atualizar fluxos nas arestas forward e residuais
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