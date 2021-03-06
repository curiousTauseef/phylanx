# Copyright (c) 2018 Parsa Amini
# Copyright (c) 2018 Hartmut Kaiser
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(example_programs
    physl
   )

foreach(example_program ${example_programs})

  set(${example_program}_FLAGS DEPENDENCIES iostreams_component)

  set(sources ${example_program}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_phylanx_executable(${example_program}
                     SOURCES ${sources}
                     ${${example_program}_FLAGS}
                     FOLDER "Examples/Interpreter"
                     COMPONENT_DEPENDENCIES iostreams)

  # add a custom target for this example
  add_phylanx_pseudo_target(examples.interpreter_.${example_program})

  # make pseudo-targets depend on master pseudo-target
  add_phylanx_pseudo_dependencies(examples.interpreter_
                              examples.interpreter_.${example_program})

  # add dependencies to pseudo-target
  add_phylanx_pseudo_dependencies(examples.interpreter_.${example_program}
                              ${example_program}_exe)
endforeach()

