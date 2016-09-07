# Based on c_src.mk from erlang.mk by Loic Hoguin <essen@ninenines.eu>

CURDIR := $(shell pwd)
BASEDIR := $(abspath $(CURDIR)/..)

PROJECT ?= $(notdir $(BASEDIR))
PROJECT := $(strip $(PROJECT))

SOURCES := nif/gb_info_util.cc \
	   nif/gb_markdown_leaf_info.cc \
	   nif/gb_markdown_parent_info.cc \
	   nif/gb_markdown_analyzer.cc \
	   nif/gb_markdown_nif.cc

ERTS_INCLUDE_DIR ?= $(shell erl -noshell -s init stop -eval "io:format(\"~s/erts-~s/include/\", [code:root_dir(), erlang:system_info(version)]).")
ERL_INTERFACE_INCLUDE_DIR ?= $(shell erl -noshell -s init stop -eval "io:format(\"~s\", [code:lib_dir(erl_interface, include)]).")
ERL_INTERFACE_LIB_DIR ?= $(shell erl -noshell -s init stop -eval "io:format(\"~s\", [code:lib_dir(erl_interface, lib)]).")

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

CFLAGS += -fPIC -I $(ERTS_INCLUDE_DIR) -I $(ERL_INTERFACE_INCLUDE_DIR) -I deps/hoedown/src -I nif/include
CXXFLAGS += -std=c++11 -fPIC -I $(ERTS_INCLUDE_DIR) -I $(ERL_INTERFACE_INCLUDE_DIR) -I deps/hoedown/src -I nif/include

LDLIBS += -L $(ERL_INTERFACE_LIB_DIR) -lerl_interface -lei
LDFLAGS += -shared

# Verbosity.

c_verbose_0 = @echo " C     " $(?F);
c_verbose = $(c_verbose_$(V))

cpp_verbose_0 = @echo " CPP   " $(?F);
cpp_verbose = $(cpp_verbose_$(V))

link_verbose_0 = @echo " LD    " $(@F);
link_verbose = $(link_verbose_$(V))

OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

COMPILE_C = $(c_verbose) $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE_CPP = $(cpp_verbose) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c

$(C_SRC_OUTPUT): deps/hoedown/libhoedown.a $(OBJECTS)
	@mkdir -p $(BASEDIR)/priv/
	$(link_verbose) $(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $(C_SRC_OUTPUT) deps/hoedown/libhoedown.a

%.o: %.cc nif/include/gb_common.hpp
	$(COMPILE_CPP) $(OUTPUT_OPTION) $<

# Source file dependencies

nif/gb_markdown_nif.cc: nif/gb_markdown_analyzer.cc nif/include/gb_common.hpp

nif/gb_markdown_leaf_info.cc: nif/gb_info_util.cc
nif/gb_markdown_parent_info.cc: nif/gb_info_util.cc
nif/gb_markdown_analyzer.cc: nif/gb_markdown_leaf_info.cc nif/gb_markdown_parent_info.cc nif/include/markdown_analyzer.hpp
nif/gb_info_util.cc: nif/include/gb_common.hpp nif/include/markdown_info.hpp

clean:
	@rm -f $(C_SRC_OUTPUT) $(OBJECTS)

deps/hoedown/libhoedown.a:
	mkdir -p deps/lib
	git clone git://github.com/hoedown/hoedown deps/hoedown
	cd deps/hoedown && make -j2
