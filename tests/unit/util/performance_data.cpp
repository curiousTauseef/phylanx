//  Copyright (c) 2018 Parsa Amini
//  Copyright (c) 2018 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <phylanx/phylanx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/include/agas.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include <blaze/Math.h>

///////////////////////////////////////////////////////////////////////////////
char const* const fib_code = R"(block(
    define(fib_test,
        block(
            define(x, 1.0),
            define(z, 0.0),
            define(y, 1.0),
            define(temp, 0.0),
            define(step, 0),
            while(
                step < 10,
                block(
                    store(z, x + y),
                    store(temp, y),
                    store(y, z),
                    store(x, temp),
                    store(step, step + 1)
                )
            ),
            z
        )
    ),
    fib_test
))";

std::map<std::string, std::vector<std::size_t>> expected_counts =
{
    { "access-variable#0", { 1, 0, } },
    { "access-variable#1", { 0, 0, } },
    { "access-variable#1", { 0, 0, } },
    { "access-variable#2", { 1, 0, } },
    { "access-variable#2", { 1, 0, } },
    { "access-variable#2", { 1, 0, } },
    { "access-variable#3", { 0, 0, } },
    { "access-variable#3", { 0, 0, } },
    { "access-variable#3", { 0, 0, } },
    { "access-variable#4", { 10, 0, } },
    { "access-variable#4", { 10, 0, } },
    { "access-variable#5", { 10, 0, } },
    { "access-variable#5", { 10, 0, } },
    { "access-variable#5", { 10, 0, } },
    { "add#0", { 10, 0, } },
    { "add#1", { 10, 0, } },
    { "block#0", { 0, 1, } },
    { "block#1", { 0, 1, } },
    { "block#2", { 10, 0, } },
    { "define-variable#0", { 2, 0, } },
    { "define-variable#1", { 11, 0, } },
    { "define-variable#2", { 12, 0, } },
    { "define-variable#3", { 21, 0, } },
    { "define-variable#4", { 11, 0, } },
    { "define-variable#5", { 22, 0, } },
    { "lt#0", { 11, 0, } },
    { "store#0", { 10, 0, } },
    { "store#1", { 10, 0, } },
    { "store#2", { 10, 0, } },
    { "store#3", { 10, 0, } },
    { "store#4", { 10, 0, } },
    { "while#0", { 1, 0, } },
};

int main()
{
    // Compile the given code
    phylanx::execution_tree::compiler::function_list snippets;

    auto const fibonacci = phylanx::execution_tree::compile(
        phylanx::ast::generate_ast(fib_code), snippets);

    // Evaluate Fibonacci using the read data
    auto const result = fibonacci();

    // List of existing primitive instances
    std::vector<std::string> existing_primitive_instances;

    // Retrieve all primitive instances
    for (auto const& entry :
        hpx::agas::find_symbols(hpx::launch::sync, "/phylanx/*#*"))
    {
        existing_primitive_instances.push_back(entry.first);
    }

    for (auto const& entry :
        phylanx::util::retrieve_counter_data(existing_primitive_instances,
            std::vector<std::string>{"count/eval", "time/eval",
                "count/eval_direct", "time/eval_direct"},
            hpx::find_here()))
    {
        auto const tags =
            phylanx::execution_tree::compiler::parse_primitive_name(
                entry.first);

        std::string const expected_key(
            tags.primitive + "#" + std::to_string(tags.sequence_number));
        auto const& expected_values = expected_counts[expected_key];

        HPX_TEST_EQ(entry.second[0], expected_values[0]);
        HPX_TEST((entry.second[1] != 0) == (expected_values[0] != 0));
        HPX_TEST_EQ(entry.second[2], expected_values[1]);
        HPX_TEST((entry.second[3] != 0) == (expected_values[1] != 0));
    }

    return hpx::util::report_errors();
}
