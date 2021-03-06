// Copyright (c) 2018 Hartmut Kaiser
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Fixing #543: Calling compile more than once in a row references wrong code

#include <phylanx/phylanx.hpp>

#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <string>
#include <sstream>

std::string const codestr = R"(
    define(ultimate_answer, 42)
    debug(ultimate_answer)
    define(ultimate_answer, lambda(debug("'42'")))
    ultimate_answer
)";

std::string const blocked_codestr = R"(block(
    define(ultimate_answer_plus_one, 43),
    debug(ultimate_answer_plus_one),
    define(ultimate_answer_plus_one, lambda(debug("'43'"))),
    ultimate_answer_plus_one
))";

int hpx_main(int argc, char* argv[])
{
    phylanx::execution_tree::compiler::function_list snippets;

    auto const& code = phylanx::execution_tree::compile(
        "code", codestr, snippets);
    auto const& blocked_code = phylanx::execution_tree::compile(
        "blocked_code", blocked_codestr, snippets);

    auto ultimate_answer = code.run();
    auto blocked_ultimate_answer = blocked_code.run();

    ultimate_answer();
    blocked_ultimate_answer();

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    HPX_TEST_EQ(hpx::init(argc, argv), 0);

    std::stringstream const& strm = hpx::get_consolestream();
    HPX_TEST_EQ(strm.str(), std::string("42\n43\n'42'\n'43'\n"));

    return hpx::util::report_errors();
}
