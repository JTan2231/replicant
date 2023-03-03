#ifndef GRADIENT
#define GRADIENT

#include <stack>
#include <unordered_map>
#include <vector>

#include "matrix.h"

template <typename T>
class Tape;

template <typename T>
class Variable;

// TODO: take more than just binary operations
template <typename T>
class Operation {
  protected:
    Variable<T> *lhOperand;
    Variable<T> *rhOperand;

    Variable<T> *output;

  public:
    Operation(Variable<T> *lh, Variable<T> *rh, Variable<T> *out)
        : lhOperand(lh), rhOperand(rh), output(out) {}
    virtual void compute() = 0;
};

template <typename T>
class Constant : public Operation<T> {
  public:
    Constant(Variable<T> *lh) : Operation<T>(lh, nullptr, lh) {
        this->lhOperand = lh;
        this->output = lh;
    }

    void compute() {
        T value = this->lhOperand->getValue();

        this->output->setValue(value);
    }
};

template <typename T>
class Multiply : public Operation<T> {
  public:
    Multiply(Variable<T> *lh, Variable<T> *rh, Variable<T> *out)
        : Operation<T>(lh, rh, out) {
        this->lhOperand = lh;
        this->rhOperand = rh;
        this->output = out;
    }

    void compute() {
        T value = this->lhOperand->getValue() * this->rhOperand->getValue();

        this->output->setValue(value);
    }
};

template <typename T>
class Divide : public Operation<T> {
  public:
    Divide(Variable<T> *lh, Variable<T> *rh, Variable<T> *out)
        : Operation<T>(lh, rh, out) {
        this->lhOperand = lh;
        this->rhOperand = rh;
        this->output = out;
    }

    void compute() {
        T value = this->lhOperand->getValue() / this->rhOperand->getValue();

        this->output->setValue(value);
    }
};

template <typename T>
class Add : public Operation<T> {
  public:
    Add(Variable<T> *lh, Variable<T> *rh, Variable<T> *out)
        : Operation<T>(lh, rh, out) {
        this->lhOperand = lh;
        this->rhOperand = rh;
        this->output = out;
    }

    void compute() {
        T value = this->lhOperand->getValue() + this->rhOperand->getValue();

        this->output->setValue(value);
    }
};

template <typename T>
class Subtract : public Operation<T> {
  public:
    Subtract(Variable<T> *lh, Variable<T> *rh, Variable<T> *out)
        : Operation<T>(lh, rh, out) {
        this->lhOperand = lh;
        this->rhOperand = rh;
        this->output = out;
    }

    void compute() {
        T value = this->lhOperand->getValue() - this->rhOperand->getValue();

        this->output->setValue(value);
    }
};

template <typename T>
class Node {
    int id;
    Operation<T> *op;

  public:
    Node(int id, Operation<T> *op) : id(id), op(op) {}
    ~Node() {
        delete op;
    }

    int getId() {
        return id;
    }
    void compute() {
        op->compute();
    }
};

template <typename T>
class Variable : public Scalar<T> {
    Tape<T> *tape;
    int nodeId;

  public:
    Variable(T value, Tape<T> *tape) : Scalar<T>(value) {
        this->tape = tape;
        nodeId = tape->addVariable(this);
    }

    Variable(Tape<T> *tape) {
        this->tape = tape;
        nodeId = tape->addVariable(this);
    }

    Tape<T> *getTape() {
        return tape;
    }

    int getNodeId() {
        return nodeId;
    }

    T getValue() {
        return *this->value;
    }
};

template <typename T>
void opOverload(int nodeId1, int nodeId2, Tape<T> *t, Operation<T> *op,
                Variable<T> *output) {
    t->createNode(op);

    int lastId = t->getLastId();
    t->addEdge(lastId, nodeId1);
    t->addEdge(lastId, nodeId2);

    t->addEdge(output->getNodeId(), lastId);
}

template <typename T>
Variable<T> operator*(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Multiply<T> *op = new Multiply<T>(&v1, &v2, output);

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output);

    return *output;
}

template <typename T>
Variable<T> operator+(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Add<T> *op = new Add<T>(&v1, &v2, output);

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output);

    return *output;
}

template <typename T>
Variable<T> operator-(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Subtract<T> *op = new Subtract<T>(&v1, &v2, output);

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output);

    return *output;
}

template <typename T>
Variable<T> operator/(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Divide<T> *op = new Divide<T>(&v1, &v2, output);

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output);

    return *output;
}

// graph that holds all the operations conducted for gradient calculation
template <typename T>
class Tape {
    vector<Node<T> *> nodes;
    vector<vector<Node<T> *>> edgeMap;

    // think this should be just constants
    unordered_map<Variable<T> *, Node<T> *> variableMap;

    int idCount;

  public:
    Tape() : idCount(0) {}

    int getLastId() {
        return idCount - 1;
    }

    void createNode(Operation<T> *op) {
        Node<T> *node = new Node<T>(idCount++, op);
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
        Constant<T> *op = new Constant<T>(v);
        createNode(op);

        return idCount - 1;
    }

    void compute(Variable<T> *v) {
        typedef pair<Node<T> *, int> P;

        // TODO: ensure graph is acyclic/accommodate cycles
        //
        // in-order depth-first traversal to calculate up from constants
        // (outer nodes)

        stack<P> s;
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
};

#endif
