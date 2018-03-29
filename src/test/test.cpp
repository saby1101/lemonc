#undef NDEBUG
#include "../main/types.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

struct profiler {
  std::string name;
  std::chrono::high_resolution_clock::time_point p;
  profiler(std::string const &n)
      : name(n), p(std::chrono::high_resolution_clock::now()) {}
  ~profiler() {
    using dura = std::chrono::duration<double>;
    auto d = std::chrono::high_resolution_clock::now() - p;
    std::cout << name << ": " << std::chrono::duration_cast<dura>(d).count()
              << std::endl;
  }
};

#define PROFILE_BLOCK(pbn) profiler _pfinstance(pbn)

extern "C" void deleteObject(void *ptr);

#define TEST(G, MCF, name)                                                     \
  extern "C" {                                                                 \
  void *G##_construct();                                                       \
  void G##_destruct(void *ptr);                                                \
  void *G##_addNode(void *graphPtr);                                           \
  void *G##_addArc(void *graphPtr, void *node1, void *node2);                  \
  void *G##_NodeMap_LONG_construct(void *graphPtr);                            \
  void G##_NodeMap_LONG_destruct(void *ptr);                                   \
  LONG G##_NodeMap_LONG_get(void *mapPtr, void *nodePtr);                      \
  void G##_NodeMap_LONG_set(void *mapPtr, void *nodePtr, LONG value);          \
  void *G##_ArcMap_LONG_construct(void *graphPtr);                             \
  void G##_ArcMap_LONG_destruct(void *ptr);                                    \
  LONG G##_ArcMap_LONG_get(void *mapPtr, void *nodePtr);                       \
  void G##_ArcMap_LONG_set(void *mapPtr, void *nodePtr, LONG value);           \
  void *G##_##MCF##_LONG_construct(void *graphPtr);                            \
  void G##_##MCF##_LONG_destruct(void *ptr);                                   \
  void G##_##MCF##_LONG_setCostMap(void *algoPtr, void *mapPtr);               \
  void G##_##MCF##_LONG_setLowerMap(void *algoPtr, void *mapPtr);              \
  void G##_##MCF##_LONG_setUpperMap(void *algoPtr, void *mapPtr);              \
  void G##_##MCF##_LONG_setSupplyMap(void *algoPtr, void *mapPtr);             \
  INT G##_##MCF##_LONG_run(void *algoPtr);                                     \
  LONG G##_##MCF##_LONG_flow(void *algoPtr, void *arcPtr);                     \
  }                                                                            \
  void name##_test() {                                                         \
    PROFILE_BLOCK(#name);                                                      \
    void *graphPtr = G##_construct();                                          \
                                                                               \
    void *node1 = G##_addNode(graphPtr);                                       \
    void *node2 = G##_addNode(graphPtr);                                       \
    void *node3 = G##_addNode(graphPtr);                                       \
                                                                               \
    void *arc1 = G##_addArc(graphPtr, node1, node2);                           \
    void *arc2 = G##_addArc(graphPtr, node2, node3);                           \
                                                                               \
    void *supplyMap = G##_NodeMap_LONG_construct(graphPtr);                    \
                                                                               \
    G##_NodeMap_LONG_set(supplyMap, node1, 1);                                 \
    G##_NodeMap_LONG_set(supplyMap, node2, 2);                                 \
    G##_NodeMap_LONG_set(supplyMap, node3, -3);                                \
                                                                               \
    assert(G##_NodeMap_LONG_get(supplyMap, node1) == 1);                       \
    assert(G##_NodeMap_LONG_get(supplyMap, node2) == 2);                       \
    assert(G##_NodeMap_LONG_get(supplyMap, node3) == -3);                      \
                                                                               \
    void *upperMap = G##_ArcMap_LONG_construct(graphPtr);                      \
    G##_ArcMap_LONG_set(upperMap, arc1, 1);                                    \
    G##_ArcMap_LONG_set(upperMap, arc2, 3);                                    \
                                                                               \
    void *costMap = G##_ArcMap_LONG_construct(graphPtr);                       \
    G##_ArcMap_LONG_set(costMap, arc1, 10);                                    \
    G##_ArcMap_LONG_set(costMap, arc2, 20);                                    \
                                                                               \
    assert(G##_ArcMap_LONG_get(costMap, arc2) == 20);                          \
                                                                               \
    void *simplexAlgo = G##_##MCF##_LONG_construct(graphPtr);                  \
    G##_##MCF##_LONG_setCostMap(simplexAlgo, costMap);                         \
    G##_##MCF##_LONG_setSupplyMap(simplexAlgo, supplyMap);                     \
    G##_##MCF##_LONG_setUpperMap(simplexAlgo, upperMap);                       \
    INT result = G##_##MCF##_LONG_run(simplexAlgo);                            \
    assert(result == 1);                                                       \
                                                                               \
    LONG flow1 = G##_##MCF##_LONG_flow(simplexAlgo, arc1);                     \
    LONG flow2 = G##_##MCF##_LONG_flow(simplexAlgo, arc2);                     \
    assert(flow1 == 1);                                                        \
    assert(flow2 == 3);                                                        \
                                                                               \
    G##_##MCF##_LONG_destruct(simplexAlgo);                                    \
                                                                               \
    G##_NodeMap_LONG_destruct(supplyMap);                                      \
    G##_ArcMap_LONG_destruct(upperMap);                                        \
    G##_ArcMap_LONG_destruct(costMap);                                         \
                                                                               \
    deleteObject(node1);                                                       \
    deleteObject(node2);                                                       \
    deleteObject(node3);                                                       \
    deleteObject(arc1);                                                        \
    deleteObject(arc2);                                                        \
                                                                               \
    std::vector<void *> nodes;                                                 \
    std::vector<void *> edges;                                                 \
    for (int i = 0; i < 1000; i++) {                                           \
      nodes.push_back(G##_addNode(graphPtr));                                  \
    }                                                                          \
    for (int i = 0; i < (int)nodes.size(); i++) {                              \
      for (int j = i; j < (int)nodes.size(); j++) {                            \
        edges.push_back(G##_addArc(graphPtr, nodes[i], nodes[j]));             \
      }                                                                        \
    }                                                                          \
    for (void *edge : edges) {                                                 \
      deleteObject(edge);                                                      \
    }                                                                          \
    for (void *node : nodes) {                                                 \
      deleteObject(node);                                                      \
    }                                                                          \
                                                                               \
    G##_destruct(graphPtr);                                                    \
  }

TEST(SmartDigraph, NetworkSimplex, SmartDigraph_NetworkSimplex);
TEST(ListDigraph, NetworkSimplex, ListDigraph_NetworkSimplex);

TEST(SmartDigraph, CostScaling, SmartDigraph_CostScaling);
TEST(ListDigraph, CostScaling, ListDigraph_CostScaling);

TEST(SmartDigraph, CapacityScaling, SmartDigraph_CapacityScaling);
TEST(ListDigraph, CapacityScaling, ListDigraph_CapacityScaling);

int main() {
  std::cout << "Starting tests...\n";

  SmartDigraph_NetworkSimplex_test();
  ListDigraph_NetworkSimplex_test();
  SmartDigraph_CostScaling_test();
  ListDigraph_CostScaling_test();
  SmartDigraph_CapacityScaling_test();
  ListDigraph_CapacityScaling_test();

  std::cout << "Tests passed succesfully!\n";

  return 0;
}
