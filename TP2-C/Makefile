# TP2 : Fichier Makefile
#
include ../Makefile.inc

EXE = exercice1 exercice2 serveur client fifo

${EXE): ${SEXP_LIB}

all: ${EXE}

fifo:
	mkfifo -m 600 fifo

clean:
	rm -f *.o *~ ${EXE} fifo journal.log


