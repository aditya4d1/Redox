//
// Copyright 2018 - present @adityaatluri
//

#pragma once

#include "Redox/Utils.hpp"
#include "Redox/IR.hpp"

namespace Redox {

template<typename T>
class Buffer {
private:
    size_t m_num_elements;
    T* m_memory;
public:
    Internal::BufferExprAST* expr;
    Buffer(size_t num_elements) : m_num_elements(num_elements), m_memory(nullptr) {
        expr = new Internal::BufferExprAST(m_num_elements);
    }
    size_t GetNumElements() const { return m_num_elements; }
    Buffer<T> Reduce();
    void Realize();
    ~Buffer() {
        // free m_memory;
        delete expr;
    }
};

} // end namespace Redox