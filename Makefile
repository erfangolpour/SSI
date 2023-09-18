.phony all:
all: ssi

ssi: ssi.c backgroundproc.c
	gcc ssi.c backgroundproc.c -lreadline -lhistory -ltermcap -o ssi

.phony clean:
clean:
	-rm -rf *.o *.exe
