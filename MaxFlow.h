#ifndef MAXFLOW_H
#define MAXFLOW_H

#include "Graph.h"

struct MaxFlow {
     template <class T>
     static int edmondsKarp(Graph<T> &g, T source, T sink);
};

#include "MaxFlow.cpp"

#endif
