TARGETS = dwlocstat

DIRS = .

ALLSOURCES = $(foreach dir,$(DIRS),					   \
		$(wildcard $(dir)/*.cc $(dir)/*.hh $(dir)/*.c $(dir)/*.h)) \
	     Makefile

CCSOURCES = $(filter %.cc,$(ALLSOURCES))
DEPFILES = $(patsubst %.cc,%.cc-dep,$(CCSOURCES))

CXXFLAGS = -g -Wall
CFLAGS = -g -Wall

all: $(TARGETS)

%.cc-dep $(TARGETS): override CXXFLAGS += -std=c++0x
$(TARGETS): override LDFLAGS += -ldw

dwlocstat: locstats.o dwarfstrings.o files.o option.o section_id.o pri.o

-include $(DEPFILES)

%.cc-dep: %.cc
	$(CXX) $(CXXFLAGS) -MM -MT '$(<:%.cc=%.o) $@' $< > $@

$(TARGETS):
	$(CXX) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(foreach dir,$(DIRS),$(dir)/*.o $(dir)/*.*-dep) $(TARGETS)

.PHONY: all clean
