
PROJECT  = loader
SOURCES   = src/*.cc

C_FLAGS   = $(INCDIR) -O2 -g -s
LIBRARIES =
LIBS	  =
INCDIR    =
COMPILER  = g++ $(C_FLAGS) -c $< -o $@
LINKER    = g++ -o $(PROJECT) $(OBJS) $(LD_FLAGS) $(LIBS)
EXEC      = ./$(PROJECT)

LD_FLAGS = $(addprefix -l,$(LIBRARIES))
SRCS = $(wildcard $(SOURCES))
OBJS = $(SRCS:.cc=.o)

all: $(PROJECT)

$(PROJECT): .depend $(OBJS)
	@echo "Linking $(OBJS) -> $@"
	@$(LINKER)

%.o: %.cc .depend
	@echo "Compiling $<"
	@$(COMPILER)

run: $(PROJECT)
	@$(EXEC)

clean:
	@rm -f $(OBJS)
	@rm -f .depend

.PHONY: all run clean

.DEFAULT:
	@echo "Computing requirements"
	@g++ -M $(SOURCES) > .depend

sinclude .depend
