#undef NDEBUG
#include "../main/types.h"
#include <cassert>
#include <chrono>
#include <cmath>
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
  LONG G##_ArcMap_LONG_get(void *mapPtr, void *arcPtr);                        \
  void G##_ArcMap_LONG_set(void *mapPtr, void *arcPtr, LONG value);            \
  void *G##_ArcMap_DOUBLE_construct(void *graphPtr);                           \
  void G##_ArcMap_DOUBLE_destruct(void *ptr);                                  \
  DOUBLE G##_ArcMap_DOUBLE_get(void *mapPtr, void *arcPtr);                    \
  void G##_ArcMap_DOUBLE_set(void *mapPtr, void *arcPtr, DOUBLE value);        \
  void *G##_##MCF##_LONG_DOUBLE_construct(void *graphPtr);                     \
  void G##_##MCF##_LONG_DOUBLE_destruct(void *ptr);                            \
  void G##_##MCF##_LONG_DOUBLE_setCostMap(void *algoPtr, void *mapPtr);        \
  void G##_##MCF##_LONG_DOUBLE_setLowerMap(void *algoPtr, void *mapPtr);       \
  void G##_##MCF##_LONG_DOUBLE_setUpperMap(void *algoPtr, void *mapPtr);       \
  void G##_##MCF##_LONG_DOUBLE_setSupplyMap(void *algoPtr, void *mapPtr);      \
  int G##_##MCF##_LONG_DOUBLE_run(void *algoPtr);                              \
  LONG G##_##MCF##_LONG_DOUBLE_flow(void *algoPtr, void *arcPtr);              \
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
    void *costMap = G##_ArcMap_DOUBLE_construct(graphPtr);                     \
    G##_ArcMap_DOUBLE_set(costMap, arc1, 10.5);                                \
    G##_ArcMap_DOUBLE_set(costMap, arc2, 20.0);                                \
                                                                               \
    auto a1 = G##_ArcMap_DOUBLE_get(costMap, arc1);                            \
    auto a2 = G##_ArcMap_DOUBLE_get(costMap, arc2);                            \
    assert(std::abs(a1 - 10.5) < 1e-10);                                       \
    assert(std::abs(a2 - 20.0) < 1e-10);                                       \
                                                                               \
    void *algo = G##_##MCF##_LONG_DOUBLE_construct(graphPtr);                  \
    G##_##MCF##_LONG_DOUBLE_setCostMap(algo, costMap);                         \
    G##_##MCF##_LONG_DOUBLE_setSupplyMap(algo, supplyMap);                     \
    G##_##MCF##_LONG_DOUBLE_setUpperMap(algo, upperMap);                       \
    int result = G##_##MCF##_LONG_DOUBLE_run(algo);                            \
    assert(result == 1);                                                       \
                                                                               \
    LONG flow1 = G##_##MCF##_LONG_DOUBLE_flow(algo, arc1);                     \
    LONG flow2 = G##_##MCF##_LONG_DOUBLE_flow(algo, arc2);                     \
    assert(flow1 == 1);                                                        \
    assert(flow2 == 3);                                                        \
                                                                               \
    G##_##MCF##_LONG_DOUBLE_destruct(algo);                                    \
                                                                               \
    G##_NodeMap_LONG_destruct(supplyMap);                                      \
    G##_ArcMap_LONG_destruct(upperMap);                                        \
    G##_ArcMap_DOUBLE_destruct(costMap);                                       \
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

extern "C" {
void *PV_construct();
void PV_destruct(void *ptr);
void PV_push_back(void *ptr, int first, int second);
void *SG_construct();
void SG_destruct(void *ptr);
void SG_build(void *graphPtr, int nodeCount, void *arcsPtr);
void *SG_NodeMap_LONG_construct(void *graphPtr);
void SG_NodeMap_LONG_destruct(void *ptr);
LONG SG_NodeMap_LONG_get(void *mapPtr, int nodeIdx);
void SG_NodeMap_LONG_set(void *mapPtr, int nodeIdx, LONG value);
void *SG_ArcMap_LONG_construct(void *graphPtr);
void SG_ArcMap_LONG_destruct(void *ptr);
LONG SG_ArcMap_LONG_get(void *mapPtr, int arcIdx);
void SG_ArcMap_LONG_set(void *mapPtr, int arcIdx, LONG value);
void *SG_ArcMap_DOUBLE_construct(void *graphPtr);
void SG_ArcMap_DOUBLE_destruct(void *ptr);
DOUBLE SG_ArcMap_DOUBLE_get(void *mapPtr, int arcIdx);
void SG_ArcMap_DOUBLE_set(void *mapPtr, int arcIdx, DOUBLE value);
void *SG_CostScaling_LONG_DOUBLE_construct(void *graphPtr);
void SG_CostScaling_LONG_DOUBLE_destruct(void *ptr);
void SG_CostScaling_LONG_DOUBLE_setCostMap(void *algoPtr, void *mapPtr);
void SG_CostScaling_LONG_DOUBLE_setLowerMap(void *algoPtr, void *mapPtr);
void SG_CostScaling_LONG_DOUBLE_setUpperMap(void *algoPtr, void *mapPtr);
void SG_CostScaling_LONG_DOUBLE_setSupplyMap(void *algoPtr, void *mapPtr);
int SG_CostScaling_LONG_DOUBLE_run(void *algoPtr);
LONG SG_CostScaling_LONG_DOUBLE_flow(void *algoPtr, int arcIdx);
}
void SG_CostScaling_test() {
  PROFILE_BLOCK("SG_CostScaling");
  void *graphPtr = SG_construct();

  void *arcs = PV_construct();
  PV_push_back(arcs, 0, 1);
  PV_push_back(arcs, 1, 2);
  PV_push_back(arcs, 0, 2);
  PV_push_back(arcs, 0, 3);

  SG_build(graphPtr, 4, arcs);
  PV_destruct(arcs);

  void *supplyMap = SG_NodeMap_LONG_construct(graphPtr);

  SG_NodeMap_LONG_set(supplyMap, 0, 3);
  SG_NodeMap_LONG_set(supplyMap, 1, 2);
  SG_NodeMap_LONG_set(supplyMap, 2, -3);
  SG_NodeMap_LONG_set(supplyMap, 3, -2);

  assert(SG_NodeMap_LONG_get(supplyMap, 0) == 3);
  assert(SG_NodeMap_LONG_get(supplyMap, 1) == 2);
  assert(SG_NodeMap_LONG_get(supplyMap, 2) == -3);
  assert(SG_NodeMap_LONG_get(supplyMap, 3) == -2);

  void *upperMap = SG_ArcMap_LONG_construct(graphPtr);
  SG_ArcMap_LONG_set(upperMap, 0, 1);
  SG_ArcMap_LONG_set(upperMap, 1, 3);
  SG_ArcMap_LONG_set(upperMap, 2, 0);
  SG_ArcMap_LONG_set(upperMap, 3, 2);

  void *costMap = SG_ArcMap_DOUBLE_construct(graphPtr);
  SG_ArcMap_DOUBLE_set(costMap, 0, 10.5);
  SG_ArcMap_DOUBLE_set(costMap, 1, 20.0);
  SG_ArcMap_DOUBLE_set(costMap, 2, 100.0);
  SG_ArcMap_DOUBLE_set(costMap, 3, 20.0);

  auto a1 = SG_ArcMap_DOUBLE_get(costMap, 0);
  auto a2 = SG_ArcMap_DOUBLE_get(costMap, 1);
  auto a3 = SG_ArcMap_DOUBLE_get(costMap, 2);
  auto a4 = SG_ArcMap_DOUBLE_get(costMap, 3);
  assert(std::abs(a1 - 10.5) < 1e-10);
  assert(std::abs(a2 - 20.0) < 1e-10);
  assert(std::abs(a3 - 100.0) < 1e-10);
  assert(std::abs(a4 - 20.0) < 1e-10);

  std::cout << "Here1!" << (long)graphPtr << std::endl;
  void *algo = SG_CostScaling_LONG_DOUBLE_construct(graphPtr);
  std::cout << "Here!\n";
  SG_CostScaling_LONG_DOUBLE_setCostMap(algo, costMap);
  SG_CostScaling_LONG_DOUBLE_setSupplyMap(algo, supplyMap);
  SG_CostScaling_LONG_DOUBLE_setUpperMap(algo, upperMap);

  int result = SG_CostScaling_LONG_DOUBLE_run(algo);
  assert(result == 1);

  LONG flow1 = SG_CostScaling_LONG_DOUBLE_flow(algo, 0);
  LONG flow2 = SG_CostScaling_LONG_DOUBLE_flow(algo, 1);
  LONG flow3 = SG_CostScaling_LONG_DOUBLE_flow(algo, 2);
  LONG flow4 = SG_CostScaling_LONG_DOUBLE_flow(algo, 3);
  assert(flow1 == 1);
  assert(flow2 == 3);
  assert(flow3 == 0);
  assert(flow4 == 2);

  SG_CostScaling_LONG_DOUBLE_destruct(algo);

  SG_NodeMap_LONG_destruct(supplyMap);
  SG_ArcMap_LONG_destruct(upperMap);
  SG_ArcMap_DOUBLE_destruct(costMap);
  SG_destruct(graphPtr);
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
  SG_CostScaling_test();

  std::cout << "Tests passed succesfully!\n";

  return 0;
}
