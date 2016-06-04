# TP6 : Fichier Makefile
#
include ../Makefile.inc

EXE = ecritube exercice1 tube1 tube2

${EXE): ${PSE}

all: ${EXE}

tube1:
	mkfifo -m 600 tube1

tube2:
	mkfifo -m 600 tube2

clean:
	rm -f *.o *~ ${EXE} journal.log


