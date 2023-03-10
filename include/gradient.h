#ifndef GRADIENT
#define GRADIENT

#include <assert.h>

#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ops.h"
#include "variable.h"

template <typename T>
class Node;
template <typename T>
class GradNode;
template <typename T>
class Tape;
template <typename T>
class Variable;
template <typename T>
class Operation;

template <typename T>
using NodePtr = std::shared_ptr<Node<T>>;
template <typename T>
using GradNodePtr = std::shared_ptr<GradNode<T>>;
template <typename T>
using OpPtr = std::shared_ptr<Operation<T>>;

template <typename T>
class Node {
    int id;
    OpPtr<T> op;

  public:
    Node(int id, OpPtr<T> op) : id(id), op(op) {}

    int getId() {
        return id;
    }
    OpPtr<T> getOp() {
        return op;
    }
    bool isConstant() {
        return op->isConstant();
    }

    void compute() {
        op->compute();
    }
};

template <typename T>
class GradNode {
    int id;
    OpPtr<T> op;
    T gradValue;

  public:
    GradNode(int id, OpPtr<T> op) : id(id), op(op) {}

    int getId() {
        return id;
    }
    void setGradValue(T du, T dv) {
        T u = op->getLhValue();
        T v = op->getRhValue();

        gradValue = op->differentiate(u, du, v, dv);
    }

    void setConstantGradValue(int wrtId, GradNodePtr<T> prevNode) {
        if (id == wrtId) {
            gradValue = 1;
        } else if (prevNode) {
            gradValue = prevNode->getGradValue();
        } else {
            gradValue = 0;
        }
    }

    T getGradValue() {
        return gradValue;
    }
    bool isConstant() {
        return op->isConstant();
    }
};

template <typename T>
class GradSubgraph {
    using PtrVec = std::vector<std::shared_ptr<GradNode<T>>>;
    PtrVec nodes;
    std::vector<PtrVec> edgeMap;

    std::unordered_map<int, int> idMap;

    int headId = 0;
    int wrtId = 0;

    int idCount = 0;

  public:
    ~GradSubgraph() {}

    void createNode(NodePtr<T> node) {
        OpPtr<T> op = node->getOp();
        GradNodePtr<T> gradNode(new GradNode(idCount, op));
        idCount++;

        nodes.push_back(gradNode);
        edgeMap.push_back({});

        idMap[node->getId()] = gradNode->getId();
    }

    void addEdge(int from, int to) {
        edgeMap[from].push_back(nodes[to]);
    }
    int getIdMapping(int from) {
        if (idMap.find(from) == idMap.end()) {
            return -1;
        }

        return idMap[from];
    }

    void setHead(int id) {
        assert(idMap.find(id) != idMap.end());

        headId = idMap[id];
    }

    T computeGradient() {
        using P = std::pair<GradNodePtr<T>, int>;

        // TODO: ensure graph is acyclic/accommodate cycles
        //
        // in-order depth-first traversal to calculate up from constants
        // (outer nodes)

        std::stack<P> s;
        s.emplace(nodes[headId], 0);  // start from target node

        while (!s.empty()) {
            auto &[node, index] = s.top();
            int id = node->getId();

            if (index == edgeMap[id].size()) {
                PtrVec &edges = edgeMap[id];
                T du, dv;
                // TODO: handle more than binary operands
                if (edges.size() == 0) {
                    du = 0;
                    dv = 0;
                } else if (edges.size() == 1) {
                    du = edges[0]->getGradValue();
                    dv = 0;
                } else {
                    du = edges[0]->getGradValue();
                    dv = edges[1]->getGradValue();
                }

                GradNodePtr<T> prevNode = nullptr;
                if (edgeMap[id].size() > 0) {
                    prevNode = edgeMap[id][0];
                }

                if (node->isConstant()) {
                    node->setConstantGradValue(wrtId, prevNode);
                } else {
                    node->setGradValue(du, dv);
                }

                s.pop();
            } else {
                s.emplace(edgeMap[id][index++], 0);
            }
        }

        // NOTE: nodes[headId] will always be constant

        return edgeMap[headId][0]->getGradValue();
    }

    void setWrt(int id) {
        assert(idMap.find(id) != idMap.end());
        wrtId = idMap[id];
    }

    void printNodes() {
        std::cout << "SUBGRAPH HAS " << nodes.size() << " NODES" << std::endl;
        for (auto node : nodes) {
            int id = node->getId();
            for (auto edge : edgeMap[id]) {
                std::cout << id << " -> " << edge->getId() << std::endl;
            }
        }
    }
};

// graph that holds all the operations conducted for gradient calculation
template <typename T>
class Tape {
    std::vector<NodePtr<T>> nodes;
    std::vector<std::vector<NodePtr<T>>> edgeMap;

    int idCount;

    GradSubgraph<T> buildGradSubgraph(Variable<T> &target, Variable<T> &wrt) {
        using P = std::pair<int, std::vector<int>>;

        std::unordered_set<int> dependent;
        std::stack<P> s;
        s.emplace(target.getNodeId(), std::vector<int>{});

        while (!s.empty()) {
            auto [nodeId, path] = s.top();
            s.pop();

            if (nodeId == wrt.getNodeId()) {
                for (int i = 0; i < path.size(); i++) {
                    dependent.insert(path[i]);
                }

                dependent.insert(nodeId);
            }

            path.push_back(nodeId);
            for (int i = 0; i < edgeMap[nodeId].size(); i++) {
                s.emplace(edgeMap[nodeId][i]->getId(), path);
            }
        }

        GradSubgraph<T> graph;
        for (auto id : dependent) {
            graph.createNode(nodes[id]);
            if (id == target.getNodeId()) {
                graph.setHead(id);
            } else if (id == wrt.getNodeId()) {
                graph.setWrt(id);
            }
        }

        for (auto id : dependent) {
            for (auto edge : edgeMap[id]) {
                int edgeId = edge->getId();
                if (!nodes[id]->isConstant() ||
                    dependent.find(edgeId) != dependent.end()) {
                    int from = graph.getIdMapping(id);
                    int to = graph.getIdMapping(edgeId);

                    // if a node isn't part of the dependent set but is needed
                    // create the node here
                    // TODO: this feels gross. is there a better way to do this?
                    if (from == -1) {
                        graph.createNode(nodes[id]);
                        from = graph.getIdMapping(id);
                    }

                    if (to == -1) {
                        graph.createNode(nodes[edgeId]);
                        to = graph.getIdMapping(edgeId);
                    }

                    graph.addEdge(from, to);
                }
            }
        }

        return graph;
    }

  public:
    Tape() : idCount(0) {}

    int getNodeCount() {
        return nodes.size();
    }

    int getLastId() {
        return idCount - 1;
    }

    void createNode(OpPtr<T> op) {
        NodePtr<T> node(new Node<T>(idCount++, op));
        nodes.push_back(node);

        // edgeMap must always be length == nodes.size()
        edgeMap.push_back({});

        assert(nodes.size() == edgeMap.size());
    }

    void createNode(Operation<T> *op) {
        OpPtr<T> opPtr(op);
        NodePtr<T> node(new Node<T>(idCount++, opPtr));
        nodes.push_back(node);

        // edgeMap must always be length == nodes.size()
        edgeMap.push_back({});

        assert(nodes.size() == edgeMap.size());
    }

    void addEdge(int from, int to) {
        edgeMap[from].push_back(nodes[to]);
    }

    void addEdge(Variable<T> *v1, Variable<T> *v2) {
        int from = v1->getNodeId();
        int to = v2->getNodeId();

        edgeMap[from].push_back(nodes[to]);
    }

    int addVariable(Variable<T> *v) {
        std::shared_ptr<Constant<T>> op(new Constant<T>(v->getBuffer()));
        createNode(op);

        return idCount - 1;
    }

    void compute(Variable<T> *v) {
        using P = std::pair<NodePtr<T>, int>;

        // TODO: ensure graph is acyclic/accommodate cycles
        //
        // in-order depth-first traversal to calculate up from constants
        // (outer nodes)

        std::stack<P> s;
        s.emplace(nodes[v->getNodeId()], 0);

        while (!s.empty()) {
            auto &[node, index] = s.top();
            int id = node->getId();

            if (index == edgeMap[id].size()) {
                node->compute();
                s.pop();
            } else {
                s.emplace(edgeMap[id][index++], 0);
            }
        }
    }

    // get path to target from wrt
    // clone graph
    // use path to replace dependent nodes with derivative
    T gradient(Variable<T> &target, Variable<T> &wrt) {
        GradSubgraph<T> graph = buildGradSubgraph(target, wrt);
        return graph.computeGradient();
    }

    void printNodes() {
        std::cout << "GRAPH HAS " << nodes.size() << " NODES" << std::endl;
        for (auto node : nodes) {
            int id = node->getId();
            for (auto edge : edgeMap[id]) {
                std::cout << id << " -> " << edge->getId() << std::endl;
            }
        }
    }
};

#endif
