# Based on c_src.mk from erlang.mk by Loic Hoguin <essen@ninenines.eu>

CURDIR := $(shell pwd)
BASEDIR := $(abspath $(CURDIR)/..)

PROJECT ?= $(notdir $(BASEDIR))
PROJECT := $(strip $(PROJECT))

SOURCES := src/gb_info_util.cc \
	   src/markdown_leaf_node.cc \
	   src/markdown_parent_node.cc \
	   src/gb_markdown_analyzer.cc \
	   src/gb_markdown_nif.cc

ERTS_INCLUDE_DIR ?= $(shell erl -noshell -s init stop -eval "io:format(\"~s/erts-~s/include/\", [code:root_dir(), erlang:system_info(version)]).")
ERL_INTERFACE_INCLUDE_DIR ?= $(shell erl -noshell -s init stop -eval "io:format(\"~s\", [code:lib_dir(erl_interface, include)]).")
ERL_INTERFACE_LIB_DIR ?= $(shell erl -noshell -s init stop -eval "io:format(\"~s\", [code:lib_dir(erl_interface, lib)]).")

PROJECT_INCLUDES = -I $(ERTS_INCLUDE_DIR) -I $(ERL_INTERFACE_INCLUDE_DIR) -I deps/hoedown/src -I include

C_SRC_OUTPUT ?= $(CURDIR)/../priv/$(PROJECT).so

# System type and C compiler/flags.

UNAME_SYS := $(shell uname -s)
ifeq ($(UNAME_SYS), Darwin)
	CC ?= cc
	CFLAGS = -O3 -std=c99 -arch x86_64 -Wall -Wmissing-prototypes
	CXXFLAGS = -O3 -arch x86_64 -Wall -pedantic -ansi
	LDFLAGS = -arch x86_64 -flat_namespace -undefined suppress
else ifeq ($(UNAME_SYS), FreeBSD)
	CC ?= cc
	CFLAGS = -O3 -std=c99 -Wall -Wmissing-prototypes
	CXXFLAGS = -O3 -Wall -pedantic -ansi
else ifeq ($(UNAME_SYS), Linux)
	CC ?= gcc
	CFLAGS = -O3 -std=c99 -Wall -Wmissing-prototypes
	CXXFLAGS = -O3 -Wall -pedantic -ansi
endif

CFLAGS += -fPIC
CXXFLAGS += -std=c++11 -fPIC

LDLIBS += -L $(ERL_INTERFACE_LIB_DIR) -lerl_interface -lei
LDFLAGS += -shared

.PHONY: clean really-clean

# Verbosity.

c_verbose_0 = @echo " C     " $(?F);
c_verbose = $(c_verbose_$(V))

cpp_verbose_0 = @echo " CPP   " $(?F);
cpp_verbose = $(cpp_verbose_$(V))

link_verbose_0 = @echo " LD    " $(@F);
link_verbose = $(link_verbose_$(V))

OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

COMPILE_C = $(c_verbose) $(CC) $(CFLAGS) $(CPPFLAGS) $(PROJECT_INCLUDES) -c
COMPILE_CPP = $(cpp_verbose) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(PROJECT_INCLUDES) -c

$(C_SRC_OUTPUT): deps/hoedown/libhoedown.a $(OBJECTS)
	@mkdir -p $(BASEDIR)/priv/
	$(link_verbose) $(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $(C_SRC_OUTPUT) deps/hoedown/libhoedown.a

%.o: %.cc include/gb_common.hpp
	$(COMPILE_CPP) $(OUTPUT_OPTION) $<

# Source file dependencies

src/gb_markdown_nif.cc: src/gb_markdown_analyzer.cc include/gb_common.hpp

src/markdown_leaf_node.cc: src/gb_info_util.cc
src/markdown_parent_node.cc: src/gb_info_util.cc
src/gb_markdown_analyzer.cc: src/markdown_leaf_node.cc src/markdown_parent_node.cc include/markdown_analyzer.hpp
src/gb_info_util.cc: include/gb_common.hpp include/markdown_node.hpp

clean:
	rm -f $(C_SRC_OUTPUT) $(OBJECTS)

really-clean: clean
	make -C deps/hoedown clean

deps/hoedown:
	git clone git://github.com/hoedown/hoedown deps/hoedown

deps/hoedown/libhoedown.a: deps/hoedown
	make -C deps/hoedown libhoedown.a -j2

check-syntax:
	$(cpp_verbose) $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(PROJECT_INCLUDES) -S $(CHK_SOURCES)
