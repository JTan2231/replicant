#ifndef OPS
#define OPS

#include <iostream>

template <typename T> class Buffer;

// TODO: take more than just binary operations
template <typename T> class Operation {
  protected:
    Buffer<T> *lhOperand;
    Buffer<T> *rhOperand;

    Buffer<T> *output;

  public:
    Operation(Buffer<T> *lh, Buffer<T> *rh, Buffer<T> *out)
        : lhOperand(lh), rhOperand(rh), output(out) {}
    virtual void compute() = 0;
    virtual T differentiate(T u, T du, T v, T dv) = 0;
    virtual bool isConstant() { return false; }

    T getOutputValue() { return output->getValue(); }

    T getLhValue() { return lhOperand ? lhOperand->getValue() : 0; }
    T getRhValue() { return rhOperand ? rhOperand->getValue() : 0; }
};

template <typename T> class Constant : public Operation<T> {
  public:
    Constant(Buffer<T> *lh) : Operation<T>(lh, nullptr, lh) {
        this->lhOperand = lh;
        this->output = lh;
    }

    void compute() {
        T value = this->lhOperand->getValue();

        this->output->setValue(value);
    }

    T differentiate(T u, T du, T v, T dv) { return 0; }

    bool isConstant() { return true; }
};

template <typename T> class Multiply : public Operation<T> {
  public:
    Multiply(Buffer<T> *lh, Buffer<T> *rh, Buffer<T> *out)
        : Operation<T>(lh, rh, out) {
        this->lhOperand = lh;
        this->rhOperand = rh;
        this->output = out;
    }

    void compute() {
        T value = this->lhOperand->getValue() * this->rhOperand->getValue();

        this->output->setValue(value);
    }

    T differentiate(T u, T du, T v, T dv) { return du * v + u * dv; }
};

template <typename T> class Divide : public Operation<T> {
  public:
    Divide(Buffer<T> *lh, Buffer<T> *rh, Buffer<T> *out)
        : Operation<T>(lh, rh, out) {
        this->lhOperand = lh;
        this->rhOperand = rh;
        this->output = out;
    }

    void compute() {
        T value = this->lhOperand->getValue() / this->rhOperand->getValue();

        this->output->setValue(value);
    }

    T differentiate(T u, T du, T v, T dv) {
        return (du * v - u * dv) / (v * v);
    }
};

template <typename T> class Add : public Operation<T> {
  public:
    Add(Buffer<T> *lh, Buffer<T> *rh, Buffer<T> *out)
        : Operation<T>(lh, rh, out) {
        this->lhOperand = lh;
        this->rhOperand = rh;
        this->output = out;
    }

    void compute() {
        T value = this->lhOperand->getValue() + this->rhOperand->getValue();

        this->output->setValue(value);
    }

    T differentiate(T u, T du, T v, T dv) { return du + dv; }
};

template <typename T> class Subtract : public Operation<T> {
  public:
    Subtract(Buffer<T> *lh, Buffer<T> *rh, Buffer<T> *out)
        : Operation<T>(lh, rh, out) {
        this->lhOperand = lh;
        this->rhOperand = rh;
        this->output = out;
    }

    void compute() {
        T value = this->lhOperand->getValue() - this->rhOperand->getValue();

        this->output->setValue(value);
    }

    T differentiate(T u, T du, T v, T dv) { return du - dv; }
};

#endif