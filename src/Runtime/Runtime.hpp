//
// Copyright 2018 - present @adityaatluri
//
#pragma once

#include <string>

#include <hip/hip_runtime_api.h>

namespace redox {
namespace runtime {

class Program {
private:
    hipModule_t m_module;
    hipFunction_t m_kernel_func;
    std::string m_file_name, m_kernel_name;
    void LoadKernel();
    size_t m_num_buffers;
    std::vector<size_t> m_buffer_sizes;
public:
    Program(std::string file_name, std::string kernel_name, std::vector<size_t> buffer_sizes) : \
        m_file_name(file_name), m_kernel_name(kernel_name), m_buffer_sizes(buffer_sizes) {
            LoadKernel();
        }
    void RunKernel(size_t, size_t, size_t, size_t, size_t, size_t, hipStream_t* stream, size_t);
    ~Program();
};

}
}