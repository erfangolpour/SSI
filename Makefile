ssi: ssi.c bgproc.c tokenizer.c
	gcc ssi.c tokenizer.c bgproc.c -lreadline -lhistory -ltermcap -o ssi