//*****************************************************************************
// Copyright 2017-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************


#include "ngraph/runtime/opv/opv_tensor.hpp"


using namespace ngraph;
using namespace std;

runtime::opv::OPVTensor::OPVTensor(const ngraph::element::Type& element_type,
                                   const Shape& shape,
                                   void* memory_pointer)
    : runtime::Tensor(std::make_shared<ngraph::descriptor::Tensor>(element_type, shape, ""))
{
    m_descriptor->set_tensor_layout(
        std::make_shared<ngraph::descriptor::layout::DenseTensorLayout>(*m_descriptor));
}


runtime::opv::OPVTensor::OPVTensor(const ngraph::element::Type& element_type, const Shape& shape)
    : OPVTensor(element_type, shape, nullptr)
{
}

runtime::opv::OPVTensor::~OPVTensor()
{
    // TODO. Is something needed?
}

void runtime::opv::OPVTensor::write(const void* source, size_t n_bytes)
{
    // Stuff from here: https://github.com/NervanaSystems/ngraph/blob/master/test/util/backend_utils.hpp#L54
    const int8_t* v = (const int8_t*)source;
    if (v == nullptr)
        return;
    m_data.resize(n_bytes);
    std::copy(v, v + n_bytes, m_data.begin());
}

void runtime::opv::OPVTensor::read(void* target, size_t n_bytes) const
{
    // Stuff from here: https://github.com/NervanaSystems/ngraph/blob/master/test/util/backend_utils.hpp#L63
    int8_t* v = (int8_t*)target;
    if (v == nullptr)
        return;
    if (n_bytes > m_data.size())
        n_bytes = m_data.size();
    std::copy(m_data.begin(), m_data.begin() + n_bytes, v);
}