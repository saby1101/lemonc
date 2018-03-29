#include <cstring>

#include "lemon/concepts/digraph.h"
#include "lemon/list_graph.h"
#include "lemon/maps.h"
#include "lemon/smart_graph.h"

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

#define MIN_COST_FLOW(ALG, G, I, name)                                         \
  void *name##_construct(void *graphPtr) {                                     \
    return new ALG<G, I>(deref<G>(graphPtr));                                  \
  }                                                                            \
  void name##_destruct(void *ptr) { delete (ALG<G, I> *)ptr; }                 \
  void name##_setCostMap(void *algoPtr, void *mapPtr) {                        \
    deref<ALG<G, I>>(algoPtr).costMap(deref<G::ArcMap<I>>(mapPtr));            \
  }                                                                            \
  void name##_setLowerMap(void *algoPtr, void *mapPtr) {                       \
    deref<ALG<G, I>>(algoPtr).lowerMap(deref<G::ArcMap<I>>(mapPtr));           \
  }                                                                            \
  void name##_setUpperMap(void *algoPtr, void *mapPtr) {                       \
    deref<ALG<G, I>>(algoPtr).upperMap(deref<G::ArcMap<I>>(mapPtr));           \
  }                                                                            \
  void name##_setSupplyMap(void *algoPtr, void *mapPtr) {                      \
    deref<ALG<G, I>>(algoPtr).supplyMap(deref<G::NodeMap<I>>(mapPtr));         \
  }                                                                            \
  INT name##_run(void *algoPtr) {                                              \
    auto type = deref<ALG<G, I>>(algoPtr).run();                               \
    switch (type) {                                                            \
    case ALG<G, I>::ProblemType::INFEASIBLE:                                   \
      return 0;                                                                \
    case ALG<G, I>::ProblemType::OPTIMAL:                                      \
      return 1;                                                                \
    case ALG<G, I>::ProblemType::UNBOUNDED:                                    \
      return 2;                                                                \
    }                                                                          \
  }                                                                            \
  I name##_flow(void *algoPtr, void *arcPtr) {                                 \
    return deref<ALG<G, I>>(algoPtr).flow(deref<G::Arc>(arcPtr));              \
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
  NODE_MAP(C, INT, name##_NodeMap_INT)                                         \
  NODE_MAP(C, LONG, name##_NodeMap_LONG)                                       \
  ARC_MAP(C, INT, name##_ArcMap_INT)                                           \
  ARC_MAP(C, LONG, name##_ArcMap_LONG)                                         \
  MIN_COST_FLOW(NetworkSimplex, C, INT, name##_NetworkSimplex_INT)             \
  MIN_COST_FLOW(NetworkSimplex, C, LONG, name##_NetworkSimplex_LONG)           \
  MIN_COST_FLOW(CostScaling, C, INT, name##_CostScaling_INT)                   \
  MIN_COST_FLOW(CostScaling, C, LONG, name##_CostScaling_LONG)                 \
  MIN_COST_FLOW(CapacityScaling, C, INT, name##_CapacityScaling_INT)           \
  MIN_COST_FLOW(CapacityScaling, C, LONG, name##_CapacityScaling_LONG)

using namespace lemon;

extern "C" {

void deleteObject(void *ptr) { free(ptr); }

GRAPH(SmartDigraph, SmartDigraph);
GRAPH(ListDigraph, ListDigraph);
}
