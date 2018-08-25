//
// Copyright 2018 - present @adityaatluri
//

#include "Runtime.hpp"

constexpr size_t num_args = 2;

void redox::runtime::Program::LoadKernel() {
    hipInit(0);
    hipDevice_t device;
    hipCtx_t context;
    
    hipDeviceGet(&device, 0);
    hipCtxCreate(&context, 0, device);

    hipModuleLoad(&m_module, m_file_name);
    hipModuleGetFunction(&m_kernel_func, m_module, m_kernel_name);

    void** host_ptr = new void*[m_buffer_sizes.size()];

    for(size_t i = 0; i < m_buffer_sizes.size(); i++) {
        host_ptr[i] = reinterpret_cast<void*>(new char[m_buffer_sizes[i]]);
    }

    hipDeviceptr_t* args = new hipDeviceptr_t[m_buffer_sizes.size()];

    for(size_t i = 0; i < m_buffer_sizes.size(); i++) {
        hipMalloc(&args[i], m_buffer_sizes[i]);
    }


}


void redox::runtime::Program::~Program() {
    for(size_t i = 0; i < m_buffer_sizes.size(); i++) {
        delete host_ptr[i];
    }
    delete host_ptr;

    hipFree();
}