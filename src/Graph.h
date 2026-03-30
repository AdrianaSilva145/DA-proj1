#ifndef DA_TP_CLASSES_GRAPH
#define DA_TP_CLASSES_GRAPH

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include "MutablePriorityQueue.h"

template <class T>
class Edge;

#define INF std::numeric_limits<double>::max()

/************************* Vertex **************************/

/**
 * @brief Representa um vertice num grafo generico.
 *
 * Contem a informacao do no, a lista de arestas de saida (adj) e de entrada
 * (incoming), bem como campos auxiliares usados por varios algoritmos de grafos
 * (BFS, DFS, Dijkstra, SCC, Max-Flow, etc.).
 *
 * @tparam T Tipo do identificador do vertice.
 */
template <class T>
class Vertex {
public:
    Vertex(T in);
    bool operator<(Vertex<T> & vertex) const;

    T getInfo() const;
    std::vector<Edge<T> *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge<T> *getPath() const;
    std::vector<Edge<T> *> getIncoming() const;

    void setInfo(T info);
    void setVisited(bool visited);
    void setProcessing(bool processing);

    int getLow() const;
    void setLow(int value);
    int getNum() const;
    void setNum(int value);

    void setIndegree(unsigned int indegree);
    void setDist(double dist);
    void setPath(Edge<T> *path);
    Edge<T> * addEdge(Vertex<T> *dest, double w);
    bool removeEdge(T in);
    void removeOutgoingEdges();

    friend class MutablePriorityQueue<Vertex>;
protected:
    T info;                          ///< Valor/identificador do vertice.
    std::vector<Edge<T> *> adj;      ///< Lista de arestas de saida.

    bool visited = false;            ///< Auxiliar para DFS/BFS.
    bool processing = false;         ///< Auxiliar para detecao de ciclos.
    int low = -1, num = -1;          ///< Auxiliar para SCC (Tarjan).
    unsigned int indegree;           ///< Grau de entrada (para ordenacao topologica).
    double dist = 0;                 ///< Distancia acumulada (para Dijkstra/Prim).
    Edge<T> *path = nullptr;         ///< Aresta do caminho (para reconstrucao).

    std::vector<Edge<T> *> incoming; ///< Lista de arestas de entrada.
    int queueIndex = 0;              ///< Indice na MutablePriorityQueue.

    void deleteEdge(Edge<T> *edge);
};

/********************** Edge ****************************/

/**
 * @brief Representa uma aresta num grafo generico.
 *
 * Contem os vertices de origem e destino, o peso/capacidade da aresta, e campos
 * auxiliares para algoritmos de fluxo (flow, reverse) e selecao (selected).
 *
 * Para redes de fluxo (Max-Flow), cada aresta forward tem uma aresta residual
 * associada via o ponteiro reverse, criada automaticamente por Graph::addEdge().
 *
 * @tparam T Tipo do identificador dos vertices.
 */
template <class T>
class Edge {
public:
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w);

    Vertex<T> * getDest() const;
    double getWeight() const;
    bool isSelected() const;
    Vertex<T> * getOrig() const;
    Edge<T> *getReverse() const;
    double getFlow() const;

    void setSelected(bool selected);
    void setReverse(Edge<T> *reverse);
    void setFlow(double flow);
    void setWeight(double w);
protected:
    Vertex<T> * dest;        ///< Vertice de destino.
    double weight;           ///< Peso ou capacidade da aresta.

    bool selected = false;   ///< Auxiliar para algoritmos de selecao.

    Vertex<T> *orig;         ///< Vertice de origem.
    Edge<T> *reverse = nullptr; ///< Aresta residual (para Max-Flow).

    double flow;             ///< Fluxo atual na aresta (para Max-Flow).
};

/********************** Graph ****************************/

/**
 * @brief Representa um grafo generico com lista de adjacencia.
 *
 * Suporta grafos dirigidos e nao-dirigidos. Inclui metodos para adicionar/remover
 * vertices e arestas, e campos auxiliares para algoritmos como Floyd-Warshall.
 *
 * Para redes de fluxo, o metodo addEdge() cria automaticamente a aresta forward
 * e a correspondente aresta residual (reverse com capacidade 0), ligando-as
 * pelo ponteiro reverse.
 *
 * @tparam T Tipo do identificador dos vertices.
 */
template <class T>
class Graph {
public:
    ~Graph();

    Vertex<T> *findVertex(const T &in) const;
    bool addVertex(const T &in);
    bool removeVertex(const T &in);
    bool addEdge(const T &sourc, const T &dest, double w);
    bool removeEdge(const T &source, const T &dest);
    bool addBidirectionalEdge(const T &sourc, const T &dest, double w);

    void clear();

    int getNumVertex() const;

    /**
     * @brief Retorna o vertice com o identificador dado, ou nullptr se nao existir.
     * @param id Identificador do vertice a procurar.
     * @return Ponteiro para o vertice, ou nullptr.
     */
    Vertex<T>* getVertex(const T &id) {
        for (auto v : getVertexSet()) {
            if (v->getInfo() == id)
                return v;
        }
        return nullptr;
    }

    std::vector<Vertex<T> *> getVertexSet() const;

protected:
    std::vector<Vertex<T> *> vertexSet; ///< Conjunto de vertices do grafo.

    double ** distMatrix = nullptr; ///< Matriz de distancias (Floyd-Warshall).
    int **pathMatrix = nullptr;     ///< Matriz de caminhos (Floyd-Warshall).

    int findVertexIdx(const T &in) const;
};

void deleteMatrix(int **m, int n);
void deleteMatrix(double **m, int n);

/************************* Vertex  **************************/

template <class T>
Vertex<T>::Vertex(T in): info(in) {}

template <class T>
Edge<T> * Vertex<T>::addEdge(Vertex<T> *d, double w) {
    auto newEdge = new Edge<T>(this, d, w);
    adj.push_back(newEdge);
    d->incoming.push_back(newEdge);
    return newEdge;
}

template <class T>
bool Vertex<T>::removeEdge(T in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        Vertex<T> *dest = edge->getDest();
        if (dest->getInfo() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true;
        }
        else {
            it++;
        }
    }
    return removedEdge;
}

template <class T>
void Vertex<T>::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

template <class T>
bool Vertex<T>::operator<(Vertex<T> & vertex) const {
    return this->dist < vertex.dist;
}

template <class T>
T Vertex<T>::getInfo() const { return this->info; }

template <class T>
int Vertex<T>::getLow() const { return this->low; }

template <class T>
void Vertex<T>::setLow(int value) { this->low = value; }

template <class T>
int Vertex<T>::getNum() const { return this->num; }

template <class T>
void Vertex<T>::setNum(int value) { this->num = value; }

template <class T>
std::vector<Edge<T>*> Vertex<T>::getAdj() const { return this->adj; }

template <class T>
bool Vertex<T>::isVisited() const { return this->visited; }

template <class T>
bool Vertex<T>::isProcessing() const { return this->processing; }

template <class T>
unsigned int Vertex<T>::getIndegree() const { return this->indegree; }

template <class T>
double Vertex<T>::getDist() const { return this->dist; }

template <class T>
Edge<T> *Vertex<T>::getPath() const { return this->path; }

template <class T>
std::vector<Edge<T> *> Vertex<T>::getIncoming() const { return this->incoming; }

template <class T>
void Vertex<T>::setInfo(T in) { this->info = in; }

template <class T>
void Vertex<T>::setVisited(bool visited) { this->visited = visited; }

template <class T>
void Vertex<T>::setProcessing(bool processing) { this->processing = processing; }

template <class T>
void Vertex<T>::setIndegree(unsigned int indegree) { this->indegree = indegree; }

template <class T>
void Vertex<T>::setDist(double dist) { this->dist = dist; }

template <class T>
void Vertex<T>::setPath(Edge<T> *path) { this->path = path; }

template <class T>
void Vertex<T>::deleteEdge(Edge<T> *edge) {
    Vertex<T> *dest = edge->getDest();
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getInfo() == info) {
            it = dest->incoming.erase(it);
        }
        else {
            it++;
        }
    }
    delete edge;
}

/********************** Edge  ****************************/

template <class T>
Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w): orig(orig), dest(dest), weight(w), flow(0) {}

template <class T>
Vertex<T> * Edge<T>::getDest() const { return this->dest; }

template <class T>
double Edge<T>::getWeight() const { return this->weight; }

template <class T>
Vertex<T> * Edge<T>::getOrig() const { return this->orig; }

template <class T>
Edge<T> *Edge<T>::getReverse() const { return this->reverse; }

template <class T>
bool Edge<T>::isSelected() const { return this->selected; }

template <class T>
double Edge<T>::getFlow() const { return flow; }

template <class T>
void Edge<T>::setSelected(bool selected) { this->selected = selected; }

template <class T>
void Edge<T>::setReverse(Edge<T> *reverse) { this->reverse = reverse; }

template <class T>
void Edge<T>::setFlow(double flow) { this->flow = flow; }

template <class T>
void Edge<T>::setWeight(double w) { this->weight = w; }

/********************** Graph  ****************************/

template <class T>
int Graph<T>::getNumVertex() const { return vertexSet.size(); }

template <class T>
std::vector<Vertex<T> *> Graph<T>::getVertexSet() const { return vertexSet; }

template <class T>
Vertex<T> * Graph<T>::findVertex(const T &in) const {
    for (auto v : vertexSet)
        if (v->getInfo() == in)
            return v;
    return nullptr;
}

template <class T>
int Graph<T>::findVertexIdx(const T &in) const {
    for (unsigned i = 0; i < vertexSet.size(); i++)
        if (vertexSet[i]->getInfo() == in)
            return i;
    return -1;
}

template <class T>
bool Graph<T>::addVertex(const T &in) {
    if (findVertex(in) != nullptr)
        return false;
    vertexSet.push_back(new Vertex<T>(in));
    return true;
}

template <class T>
bool Graph<T>::removeVertex(const T &in) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        if ((*it)->getInfo() == in) {
            auto v = *it;
            v->removeOutgoingEdges();
            for (auto u : vertexSet) {
                u->removeEdge(v->getInfo());
            }
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}

template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, 0);  // aresta residual com capacidade 0
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
    Vertex<T> * srcVertex = findVertex(sourc);
    if (srcVertex == nullptr) {
        return false;
    }
    return srcVertex->removeEdge(dest);
}

template <class T>
bool Graph<T>::addBidirectionalEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, w);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

inline void deleteMatrix(double **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

template <class T>
Graph<T>::~Graph() {
    deleteMatrix(distMatrix, vertexSet.size());
    deleteMatrix(pathMatrix, vertexSet.size());
}

template<class T>
void Graph<T>::clear() {
    for (auto v: vertexSet) {
        v->removeOutgoingEdges();
        delete v;
    }
    vertexSet.clear();
}

#endif /* DA_TP_CLASSES_GRAPH */