UNAME := $(shell uname)
ifeq ($(UNAME),$(filter $(UNAME),Linux Darwin))
ifeq ($(UNAME),$(filter $(UNAME),Darwin))
OS=darwin
else
OS=linux
endif
else
OS=windows
endif

program_NAME := mu-test
program_SRCS := $(shell find test -type f -name '*.cpp')
program_OBJS := ${program_SRCS:.cpp=.o}
program_DEPS := ${program_OBJS:.o=.dep}
program_INCLUDE_DIRS := include external/catch
program_LIBRARY_DIRS :=
program_LIBRARIES :=

CXXFLAGS += -g

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach libdir,$(program_LIBRARY_DIRS),-L$(libdir))
LDFLAGS += $(foreach lib,$(program_LIBRARIES),-l$(lib))

.PHONY: all clean distclean

depend: .depend

.depend: $(program_SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $^ -MF  ./.depend;

include .depend

all: $(program_NAME)

-include $(program_DEPS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MT $@ -MF $(patsubst %.o,%.dep,$@) $<
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(program_NAME): $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)
	@- $(RM) .depend

distclean: clean
