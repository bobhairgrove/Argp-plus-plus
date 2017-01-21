noinst_PROGRAMS = \
               ../argp_test \
               ../step_0 \
               ../step_1 \
               ../step_2 \
               ../step_3 \
               ../step_4 \
               ../step_5 \
               ../step_6 \
               ../step_7 \
               ../step_8

noinst_LTLIBRARIES = libboilerplate.la
libboilerplate_la_SOURCES = ../boilerplate/boilerplate.cpp \
                            ../../src/argpp.cpp \
                            ../../src/argppbaseimpl.cpp \
                            ../../include/argpp.hpp \
                            ../../include/argppbaseimpl.hpp

examples_CXXFLAGS = $(CXXFLAGS) -I../../include
libboilerplate_la_CXXFLAGS = $(examples_CXXFLAGS)

___argp_test_CXXFLAGS = $(examples_CXXFLAGS)
___step_0_CXXFLAGS = $(examples_CXXFLAGS)
___step_1_CXXFLAGS = $(examples_CXXFLAGS)
___step_2_CXXFLAGS = $(examples_CXXFLAGS)
___step_3_CXXFLAGS = $(examples_CXXFLAGS)
___step_4_CXXFLAGS = $(examples_CXXFLAGS)
___step_5_CXXFLAGS = $(examples_CXXFLAGS)
___step_6_CXXFLAGS = $(examples_CXXFLAGS)
___step_7_CXXFLAGS = $(examples_CXXFLAGS)
___step_8_CXXFLAGS = $(examples_CXXFLAGS)

___argp_test_SOURCES = ../ex_argp_test/ex_argp_test.cpp
___argp_test_LDADD   = libboilerplate.la

___step_0_SOURCES = ../ex_step_0/ex_step_0.cpp
___step_0_LDADD   = libboilerplate.la

___step_1_SOURCES = ../ex_step_1/ex_step_1.cpp
___step_1_LDADD   = libboilerplate.la

___step_2_SOURCES = ../ex_step_2/ex_step_2.cpp
___step_2_LDADD   = libboilerplate.la

___step_3_SOURCES = ../ex_step_3/ex_step_3.cpp
___step_3_LDADD   = libboilerplate.la

___step_4_SOURCES = ../ex_step_4/ex_step_4.cpp
___step_4_LDADD   = libboilerplate.la

___step_5_SOURCES = ../ex_step_5/ex_step_5.cpp
___step_5_LDADD   = libboilerplate.la

___step_6_SOURCES = ../ex_step_6/ex_step_6.cpp
___step_6_LDADD   = libboilerplate.la

___step_7_SOURCES = ../ex_step_7/ex_step_7.cpp
___step_7_LDADD   = libboilerplate.la

___step_8_SOURCES = ../ex_step_8/ex_step_8.cpp
___step_8_LDADD   = libboilerplate.la

.PHONY: examples

examples: $(noinst_PROGRAMS)
