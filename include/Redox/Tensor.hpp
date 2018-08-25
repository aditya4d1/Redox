//
// Copyright 2018 - @adityaatluri
//
#pragma once

#include <cstdlib>

namespace Redox {

template<typename T>
class Tensor {
private:
    size_t m_num_batches, m_num_channels, m_height, m_width;
public:
    Tensor(size_t num_batches, size_t num_channels, size_t height, size_t width) : m_num_batches(num_batches), m_num_channels(num_channels), m_height(height), m_width(width) {
    }
    size_t Batches() const { return m_num_batches; }
    size_t Channels() const { return m_num_channels; }
    size_t Height() const { return m_height; }
    size_t Width() const { return m_width; }
};
}
