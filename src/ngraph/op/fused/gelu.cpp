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
#include "ngraph/op/fused/gelu.hpp"

#include <cmath>
#include "ngraph/builder/make_constant.hpp"
#include "ngraph/op/add.hpp"
#include "ngraph/op/constant.hpp"
#include "ngraph/op/divide.hpp"
#include "ngraph/op/erf.hpp"
#include "ngraph/op/exp.hpp"
#include "ngraph/op/multiply.hpp"
#include "ngraph/op/subtract.hpp"

using namespace std;
using namespace ngraph;

op::Gelu::Gelu(const shared_ptr<Node>& data)
    : FusedOp("Gelu", {data})
{
    constructor_validate_and_infer_types();
}

// f(x) = 0.5 * x * (1.0 + erf( x / sqrt(2.0) )
NodeVector op::Gelu::decompose_op() const
{
    auto data = get_argument(0);

    shared_ptr<ngraph::Node> half =
        builder::make_constant(data->get_element_type(), data->get_shape(), 0.5);

    shared_ptr<ngraph::Node> one =
        builder::make_constant(data->get_element_type(), data->get_shape(), 1.0);

    shared_ptr<ngraph::Node> sqrt_two =
        builder::make_constant(data->get_element_type(), data->get_shape(), std::sqrt(2.0));

    return {half * data * (one + make_shared<ngraph::op::Erf>(data / sqrt_two))};
}

shared_ptr<Node> op::Gelu::copy_with_new_args(const NodeVector& new_args) const
{
    if (new_args.size() != 1)
    {
        throw ngraph_error("Incorrect number of new arguments");
    }
    return make_shared<Gelu>(new_args.at(0));
}

void op::Gelu::generate_adjoints(autodiff::Adjoints& adjoints, const NodeVector& deltas)
{
    auto delta = deltas.at(0);
    auto data = get_argument(0);

    auto backprop = make_shared<op::GeluBackprop>(data, delta);
    adjoints.add_delta(data, backprop);
}

void op::Gelu::pre_validate_and_infer_types()
{
    element::Type input_element_type = get_input_element_type(0);

    NODE_VALIDATION_CHECK(this,
                          input_element_type.is_dynamic() || input_element_type == element::f32 ||
                              input_element_type == element::f64 ||
                              input_element_type == element::f16 ||
                              input_element_type == element::bf16,
                          "Argument element type must be f16, bf16, f32, f64 or dynamic (got ",
                          input_element_type,
                          ").");
}

op::GeluBackprop::GeluBackprop(const shared_ptr<Node>& data, const shared_ptr<Node>& delta)
    : FusedOp("GeluBackprop", check_single_output_args({data, delta}))
{
    constructor_validate_and_infer_types();
}

/// f'(x) = 0.5 * (1 + erf( x / sqrt(2)) + x * sqrt(2 / pi) * exp (-(x / sqrt(2))^2))
NodeVector op::GeluBackprop::decompose_op() const
{
    const double pi = 3.14159265358979323846264338327950288;
    auto data = get_argument(0);

    shared_ptr<ngraph::Node> half =
        builder::make_constant(data->get_element_type(), data->get_shape(), 0.5);

    shared_ptr<ngraph::Node> one =
        builder::make_constant(data->get_element_type(), data->get_shape(), 1.0);

    shared_ptr<ngraph::Node> neg_one =
        builder::make_constant(data->get_element_type(), data->get_shape(), -1.0);

    shared_ptr<ngraph::Node> sqrt_two_over_pi =
        builder::make_constant(data->get_element_type(), data->get_shape(), std::sqrt(2.0 / pi));

    shared_ptr<ngraph::Node> sqrt_two =
        builder::make_constant(data->get_element_type(), data->get_shape(), std::sqrt(2.0));

    shared_ptr<ngraph::Node> tmp = data / sqrt_two;

    return {half * (one + make_shared<ngraph::op::Erf>(tmp) +
                    data * sqrt_two_over_pi * make_shared<ngraph::op::Exp>(neg_one * tmp * tmp))};
}

shared_ptr<Node> op::GeluBackprop::copy_with_new_args(const NodeVector& new_args) const
{
    if (new_args.size() != 2)
    {
        throw ngraph_error("Incorrect number of new arguments");
    }
    return make_shared<GeluBackprop>(new_args.at(0), new_args.at(1));
}

void op::GeluBackprop::pre_validate_and_infer_types()
{
    element::Type input_element_type = get_input_element_type(0);

    NODE_VALIDATION_CHECK(this,
                          input_element_type.is_dynamic() || input_element_type == element::f32 ||
                              input_element_type == element::f64 ||
                              input_element_type == element::f16 ||
                              input_element_type == element::bf16,
                          "Argument element type must be f16, bf16, f32, f64 or dynamic (got ",
                          input_element_type,
                          ").");
}
