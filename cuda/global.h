/*  This file is part of the Vc library. {{{
Copyright © 2015 Jan Stephan <jan.stephan.dd@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the names of contributing organizations nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

}}}*/

#ifndef CUDA_GLOBAL_H
#define CUDA_GLOBAL_H

#include "macros.h"

#define CUDA_VECTOR_SIZE    32
#ifdef VC_NVCC
#define CUDA_CALLABLE __device__
#else
#define CUDA_CALLABLE
#endif

namespace Vc_VERSIONED_NAMESPACE
{
namespace CUDA
{
namespace Internal
{
    __device__ Vc_ALWAYS_INLINE int getThreadId()
    {
        return blockIdx.x * blockDim.x + threadIdx.x;
    }

    // allocates memory that is to be shared within a thread of blocks --
    // this is likely to be slower than pre-allocating the memory on
    // the host, so use with caution
    __device__ void* block_malloc(std::size_t bytes)
    {
        __shared__ void *ptr;
        if(threadIdx.x == 0)
            ptr = malloc(bytes);
        
        __syncthreads();
        return ptr;
    }

    __device__ void block_free(void *ptr)
    {
        __syncthreads();
        if(threadIdx.x == 0)
           free(ptr);
    }

} // namespace Internal

template <typename F, typename... Arguments>
Vc_ALWAYS_INLINE void spawn(F&& kernel, Arguments&& ... args)
{
    kernel<<<1, CUDA_VECTOR_SIZE>>>(std::forward<Arguments>(args) ...);
}

template <typename F>
Vc_ALWAYS_INLINE void spawn(F&& kernel)
{
    kernel<<<1, CUDA_VECTOR_SIZE>>>();
}

} // namespace CUDA
} // namespace Vc

#include "undomacros.h"

#endif
