//*****************************************************************************
// Copyright 2017-2019 Intel Corporation
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

#pragma once

#include "ngraph/op/op.hpp"

namespace ngraph
{
    namespace op
    {
        /// \brief Add updates to slices from inputs addressed by indices
        class ScatterNDAdd : public Op
        {
        public:
            NGRAPH_API
            static const std::string type_name;
            const std::string& description() const override { return type_name; }
            ScatterNDAdd() = default;
            /// \param inputs Tensor
            /// \param indices Index tensor: Data type must be `element::i32` or `element::i64`
            /// \param updates Tensor: Must have same type as inputs
            ScatterNDAdd(const Output<Node>& inputs,
                         const Output<Node>& indices,
                         const Output<Node>& updates)
                : Op({inputs, indices, updates})
            {
                constructor_validate_and_infer_types();
            }

            void validate_and_infer_types() override;

            void generate_adjoints(autodiff::Adjoints& adjoints, const NodeVector& deltas) override
            {
                throw ngraph_error("Not yet implemented");
            }

            virtual std::shared_ptr<Node>
                copy_with_new_args(const NodeVector& new_args) const override;
        };
    }
}
