#include <cstring>
#include <utility>

#include "lemon/concepts/digraph.h"
#include "lemon/list_graph.h"
#include "lemon/maps.h"
#include "lemon/smart_graph.h"
#include "lemon/static_graph.h"

#include "lemon/capacity_scaling.h"
#include "lemon/cost_scaling.h"
#include "lemon/network_simplex.h"

#include "types.h"

template <typename T> inline void *copyToHeap(T &obj) {
  void *holder = malloc(sizeof obj);
  std::memcpy(holder, &obj, sizeof obj);
  return holder;
}

template <typename T> inline T &deref(void *ptr) { return *((T *)ptr); }

#define CLASS(C, name)                                                         \
  void *name##_construct() { return new C(); }                                 \
  void name##_destruct(void *ptr) { delete (C *)ptr; }

#define TEMPLATED_CLASS(C, T, name)                                            \
  void *name##_construct() { return new C<T>(); }                              \
  void name##_destruct(void *ptr) { delete (C<T> *)ptr; }

#define NODE_MAP(G, T, name)                                                   \
  void *name##_construct(void *graphPtr) {                                     \
    return new G::NodeMap<T>(deref<G>(graphPtr));                              \
  }                                                                            \
  void name##_destruct(void *ptr) { delete (G::NodeMap<T> *)ptr; }             \
  T name##_get(void *mapPtr, void *nodePtr) {                                  \
    return deref<G::NodeMap<T>>(mapPtr)[deref<G::Node>(nodePtr)];              \
  }                                                                            \
  void name##_set(void *mapPtr, void *nodePtr, T value) {                      \
    deref<G::NodeMap<T>>(mapPtr)[deref<G::Node>(nodePtr)] = value;             \
  }

#define ARC_MAP(G, T, name)                                                    \
  void *name##_construct(void *graphPtr) {                                     \
    return new G::ArcMap<T>(deref<G>(graphPtr));                               \
  }                                                                            \
  void name##_destruct(void *ptr) { delete (G::ArcMap<T> *)ptr; }              \
  T name##_get(void *mapPtr, void *arcPtr) {                                   \
    return deref<G::ArcMap<T>>(mapPtr)[deref<G::Arc>(arcPtr)];                 \
  }                                                                            \
  void name##_set(void *mapPtr, void *arcPtr, T value) {                       \
    deref<G::ArcMap<T>>(mapPtr)[deref<G::Arc>(arcPtr)] = value;                \
  }

#define MIN_COST_FLOW(ALG, G, V, C, name)                                      \
  void *name##_construct(void *graphPtr) {                                     \
    return new ALG<G, V, C>(deref<G>(graphPtr));                               \
  }                                                                            \
  void name##_destruct(void *ptr) { delete (ALG<G, V, C> *)ptr; }              \
  void name##_setCostMap(void *algoPtr, void *mapPtr) {                        \
    deref<ALG<G, V, C>>(algoPtr).costMap(deref<G::ArcMap<C>>(mapPtr));         \
  }                                                                            \
  void name##_setLowerMap(void *algoPtr, void *mapPtr) {                       \
    deref<ALG<G, V, C>>(algoPtr).lowerMap(deref<G::ArcMap<V>>(mapPtr));        \
  }                                                                            \
  void name##_setUpperMap(void *algoPtr, void *mapPtr) {                       \
    deref<ALG<G, V, C>>(algoPtr).upperMap(deref<G::ArcMap<V>>(mapPtr));        \
  }                                                                            \
  void name##_setSupplyMap(void *algoPtr, void *mapPtr) {                      \
    deref<ALG<G, V, C>>(algoPtr).supplyMap(deref<G::NodeMap<V>>(mapPtr));      \
  }                                                                            \
  int name##_run(void *algoPtr) {                                              \
    auto type = deref<ALG<G, V, C>>(algoPtr).run();                            \
    switch (type) {                                                            \
    case ALG<G, V, C>::ProblemType::INFEASIBLE:                                \
      return 0;                                                                \
    case ALG<G, V, C>::ProblemType::OPTIMAL:                                   \
      return 1;                                                                \
    case ALG<G, V, C>::ProblemType::UNBOUNDED:                                 \
      return 2;                                                                \
    }                                                                          \
  }                                                                            \
  V name##_flow(void *algoPtr, void *arcPtr) {                                 \
    return deref<ALG<G, V, C>>(algoPtr).flow(deref<G::Arc>(arcPtr));           \
  }

#define GRAPH(C, name)                                                         \
  CLASS(C, name)                                                               \
  void *name##_addNode(void *graphPtr) {                                       \
    C::Node node = deref<C>(graphPtr).addNode();                               \
    return copyToHeap(node);                                                   \
  }                                                                            \
  void *name##_addArc(void *graphPtr, void *node1, void *node2) {              \
    C::Arc arc = deref<C>(graphPtr).addArc(deref<C::Node>(node1),              \
                                           deref<C::Node>(node2));             \
    return copyToHeap(arc);                                                    \
  }                                                                            \
  NODE_MAP(C, LONG, name##_NodeMap_LONG)                                       \
  ARC_MAP(C, LONG, name##_ArcMap_LONG)                                         \
  ARC_MAP(C, DOUBLE, name##_ArcMap_DOUBLE)                                     \
                                                                               \
  MIN_COST_FLOW(NetworkSimplex, C, LONG, LONG,                                 \
                name##_NetworkSimplex_LONG_LONG)                               \
  MIN_COST_FLOW(NetworkSimplex, C, LONG, DOUBLE,                               \
                name##_NetworkSimplex_LONG_DOUBLE)                             \
  MIN_COST_FLOW(CostScaling, C, LONG, LONG, name##_CostScaling_LONG_LONG)      \
  MIN_COST_FLOW(CostScaling, C, LONG, DOUBLE, name##_CostScaling_LONG_DOUBLE)  \
  MIN_COST_FLOW(CapacityScaling, C, LONG, LONG,                                \
                name##_CapacityScaling_LONG_LONG)                              \
  MIN_COST_FLOW(CapacityScaling, C, LONG, DOUBLE,                              \
                name##_CapacityScaling_LONG_DOUBLE)

#define SG StaticDigraph
#define PV std::vector<std::pair<int, int>>

#define SG_NODE_MAP(T, name)                                                   \
  void *name##_construct(void *graphPtr) {                                     \
    return new StaticDigraph::NodeMap<T>(deref<SG>(graphPtr));                 \
  }                                                                            \
  void name##_destruct(void *ptr) { delete (SG::NodeMap<T> *)ptr; }            \
  T name##_get(void *mapPtr, int nodeIdx) {                                    \
    return deref<SG::NodeMap<T>>(mapPtr)[SG::node(nodeIdx)];                   \
  }                                                                            \
  void name##_set(void *mapPtr, int nodeIdx, T value) {                        \
    deref<SG::NodeMap<T>>(mapPtr)[SG::node(nodeIdx)] = value;                  \
  }

#define SG_ARC_MAP(T, name)                                                    \
  void *name##_construct(void *graphPtr) {                                     \
    return new SG::ArcMap<T>(deref<SG>(graphPtr));                             \
  }                                                                            \
  void name##_destruct(void *ptr) { delete (SG::ArcMap<T> *)ptr; }             \
  T name##_get(void *mapPtr, int arcIdx) {                                     \
    return deref<SG::ArcMap<T>>(mapPtr)[SG::arc(arcIdx)];                      \
  }                                                                            \
  void name##_set(void *mapPtr, int arcIdx, T value) {                         \
    deref<SG::ArcMap<T>>(mapPtr)[SG::arc(arcIdx)] = value;                     \
  }

#define SG_MIN_COST_FLOW(ALG, V, C, name)                                      \
  void *name##_construct(void *graphPtr) {                                     \
    return new ALG<SG, V, C>(deref<SG>(graphPtr));                             \
  }                                                                            \
  void name##_destruct(void *ptr) { delete (ALG<SG, V, C> *)ptr; }             \
  void name##_setCostMap(void *algoPtr, void *mapPtr) {                        \
    deref<ALG<SG, V, C>>(algoPtr).costMap(deref<SG::ArcMap<C>>(mapPtr));       \
  }                                                                            \
  void name##_setLowerMap(void *algoPtr, void *mapPtr) {                       \
    deref<ALG<SG, V, C>>(algoPtr).lowerMap(deref<SG::ArcMap<V>>(mapPtr));      \
  }                                                                            \
  void name##_setUpperMap(void *algoPtr, void *mapPtr) {                       \
    deref<ALG<SG, V, C>>(algoPtr).upperMap(deref<SG::ArcMap<V>>(mapPtr));      \
  }                                                                            \
  void name##_setSupplyMap(void *algoPtr, void *mapPtr) {                      \
    deref<ALG<SG, V, C>>(algoPtr).supplyMap(deref<SG::NodeMap<V>>(mapPtr));    \
  }                                                                            \
  int name##_run(void *algoPtr) {                                              \
    auto type = deref<ALG<SG, V, C>>(algoPtr).run();                           \
    switch (type) {                                                            \
    case ALG<SG, V, C>::ProblemType::INFEASIBLE:                               \
      return 0;                                                                \
    case ALG<SG, V, C>::ProblemType::OPTIMAL:                                  \
      return 1;                                                                \
    case ALG<SG, V, C>::ProblemType::UNBOUNDED:                                \
      return 2;                                                                \
    }                                                                          \
  }                                                                            \
  V name##_flow(void *algoPtr, int arcIdx) {                                   \
    return deref<ALG<SG, V, C>>(algoPtr).flow(SG::arc(arcIdx));                \
  }

using namespace lemon;

extern "C" {

void deleteObject(void *ptr) { free(ptr); }

GRAPH(SmartDigraph, SmartDigraph);
GRAPH(ListDigraph, ListDigraph);

void *PV_construct() { return new PV(); }

void PV_destruct(void *ptr) { delete (PV *)ptr; }

void PV_push_back(void *ptr, int first, int second) {
  deref<PV>(ptr).push_back(std::make_pair(first, second));
}

CLASS(SG, SG);

void SG_build(void *graphPtr, int nodeCount, void *arcsPtr) {
  PV &arcs = deref<PV>(arcsPtr);
  deref<SG>(graphPtr).build(nodeCount, arcs.begin(), arcs.end());
}

SG_NODE_MAP(LONG, SG_NodeMap_LONG);
SG_ARC_MAP(LONG, SG_ArcMap_LONG);
SG_ARC_MAP(DOUBLE, SG_ArcMap_DOUBLE);

SG_MIN_COST_FLOW(CostScaling, LONG, DOUBLE, SG_CostScaling_LONG_DOUBLE)
}
