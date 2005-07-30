
#
# Makefile para compilar programas en C++
#

PROYECTO  = loader
FUENTES   = src/*.cc

C_FLAGS   = $(INCDIR) -O2 -g -s
LIBRERIAS =
LIBS	  =
INCDIR    =
COMPILAR  = g++ $(C_FLAGS) -c $< -o $@
ENLAZAR   = g++ -o $(PROYECTO) $(OBJS) $(LD_FLAGS) $(LIBS)
EJECUTAR  = ./$(PROYECTO)

LD_FLAGS = $(addprefix -l,$(LIBRERIAS))
SRCS = $(wildcard $(FUENTES))
OBJS = $(SRCS:.cc=.o)

all: $(PROYECTO)

$(PROYECTO): .depend $(OBJS)
	@echo "Enlazando $(OBJS) -> $@"
	@$(ENLAZAR)

%.o: %.cc .depend
	@echo "Compilando $<"
	@$(COMPILAR)

run: $(PROYECTO)
	@$(EJECUTAR)

clean:
	@rm -f $(OBJS)
	@rm -f .depend

.PHONY: all run clean

.DEFAULT:
	@echo "Calculando requisitos"
	@g++ -M $(FUENTES) > .depend

sinclude .depend
