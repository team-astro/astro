UNAME := $(shell uname)
CWD := $(shell pwd)
ifeq ($(UNAME),$(filter $(UNAME),Linux Darwin))
	ifeq ($(UNAME),$(filter $(UNAME),Darwin))
		# TODO: Check for iOS build flags.
		OS=osx
	else
		OS=linux
	endif
	CXXFLAGS += -std=c++11
else
	OS=windows
endif

program_NAME := astro-tests
program_SRCS := $(shell find `pwd`/test -type f -name '*.cpp')
program_OBJS := ${program_SRCS:.cpp=.o}
program_DEPS := ${program_OBJS:.o=.dep}
program_INCLUDE_DIRS := $(CWD)/include $(CWD)/deps
program_LIBRARY_DIRS :=
program_LIBRARIES :=

CXXFLAGS += -g -O0

ifeq ($(OS),$(filter $(OS),osx ios))
	CXXFLAGS += -stdlib=libc++
	LDFLAGS +=
endif

CXXFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach libdir,$(program_LIBRARY_DIRS),-L$(libdir))
LDFLAGS += $(foreach lib,$(program_LIBRARIES),-l$(lib))

.PHONY: all clean distclean

all: generate_linter_flags $(program_NAME)

-include $(program_DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -x c++ -MM -MT $@ -MF $(patsubst %.o,%.dep,$@) $<
	$(CXX) $(CXXFLAGS) -x c++ -c -o $@ $<

%.o: %.mm
	$(CXX) $(CXXFLAGS) -x objective-c++ -MM -MT $@ -MF $(patsubst %.o,%.dep,$@) $<
	$(CXX) $(CXXFLAGS) -x objective-c++ -c -o $@ $<

$(program_NAME): $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME)

generate_linter_flags: Makefile
	echo "$(CXXFLAGS)" | tr ' ' '\n' > .linter-clang-flags

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(shell find test -type f -name '*.o')
	@- $(RM) $(shell find test -type f -name '*.dep')

distclean: clean
