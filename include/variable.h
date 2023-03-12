#ifndef VARIABLE
#define VARIABLE

#include "ops.h"
#include "scalar.h"

template <typename T> class Tape;
template <typename T> class Operation;
template <typename T> class Multiply;
template <typename T> class Divide;
template <typename T> class Add;
template <typename T> class Subtract;

template <typename T> class Variable : public Scalar<T> {
    Tape<T> *tape;
    int nodeId;

  public:
    Variable(T value, Tape<T> *tape) : Scalar<T>(value) {
        this->tape = tape;
        nodeId = tape->addVariable(this);
    }

    Variable(Tape<T> *tape) : Scalar<T>() {
        this->tape = tape;
        nodeId = tape->addVariable(this);
    }

    Variable(const Variable<T> &v) : Scalar<T>(v) {
        tape = v.tape;
        nodeId = v.nodeId;
    }

    friend void swap(Variable<T> &lh, Variable<T> &rh) {
        using std::swap;

        swap(lh.tape, rh.tape);
        swap(lh.nodeId, rh.nodeId);
        swap(lh.value, rh.value);
    }

    Variable<T> &operator=(Variable<T> rh) {
        swap(*this, rh);

        return *this;
    }

    Tape<T> *getTape() { return tape; }

    Buffer<T> *getBuffer() { return this->value; }

    int getNodeId() { return nodeId; }

    T getValue() { return this->value->getValue(); }
};

template <typename T>
void opOverload(int nodeId1, int nodeId2, Tape<T> *t, Operation<T> *op,
                int outputNodeId) {
    t->createNode(op);

    int lastId = t->getLastId();
    t->addEdge(lastId, nodeId1);
    t->addEdge(lastId, nodeId2);

    t->addEdge(outputNodeId, lastId);
}

template <typename T> Variable<T> operator*(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Multiply<T> *op =
        new Multiply<T>(v1.getBuffer(), v2.getBuffer(), output->getBuffer());

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output->getNodeId());

    return *output;
}

template <typename T> Variable<T> operator*(Variable<T> &v1, T v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *rhVar = new Variable<T>(v2, tape);

    return v1 * (*rhVar);
}

template <typename T> Variable<T> operator*(T &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *lhVar = new Variable<T>(v1, tape);

    return (*lhVar) * v2;
}

template <typename T> Variable<T> operator+(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Add<T> *op =
        new Add<T>(v1.getBuffer(), v2.getBuffer(), output->getBuffer());

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output->getNodeId());

    return *output;
}

template <typename T> Variable<T> operator+(Variable<T> &v1, T v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *rhVar = new Variable<T>(v2, tape);

    return v1 + (*rhVar);
}

template <typename T> Variable<T> operator+(T &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *lhVar = new Variable<T>(v1, tape);

    return (*lhVar) + v2;
}

template <typename T> Variable<T> operator-(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Subtract<T> *op =
        new Subtract<T>(v1.getBuffer(), v2.getBuffer(), output->getBuffer());

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output->getNodeId());

    return *output;
}

template <typename T> Variable<T> operator-(Variable<T> &v1, T v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *rhVar = new Variable<T>(v2, tape);

    return v1 - (*rhVar);
}

template <typename T> Variable<T> operator-(T &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *lhVar = new Variable<T>(v1, tape);

    return (*lhVar) - v2;
}

template <typename T> Variable<T> operator/(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Divide<T> *op =
        new Divide<T>(v1.getBuffer(), v2.getBuffer(), output->getBuffer());

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output->getNodeId());

    return *output;
}

template <typename T> Variable<T> operator/(Variable<T> &v1, T v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *rhVar = new Variable<T>(v2, tape);

    return v1 / (*rhVar);
}

template <typename T> Variable<T> operator/(T &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *lhVar = new Variable<T>(v1, tape);

    return (*lhVar) / v2;
}

template <typename T> Variable<T> operator^(Variable<T> &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *output = new Variable<T>(tape);
    Power<T> *op =
        new Power<T>(v1.getBuffer(), v2.getBuffer(), output->getBuffer());

    opOverload(v1.getNodeId(), v2.getNodeId(), tape, op, output->getNodeId());

    return *output;
}

template <typename T> Variable<T> operator^(Variable<T> &v1, T v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *rhVar = new Variable<T>(v2, tape);

    return v1 ^ (*rhVar);
}

template <typename T> Variable<T> operator^(T &v1, Variable<T> &v2) {
    Tape<T> *tape = v1.getTape();
    Variable<T> *lhVar = new Variable<T>(v1, tape);

    return (*lhVar) ^ v2;
}

#endif
