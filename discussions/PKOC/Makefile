SPECS=pkoc-osdp-acu.pdf

all:	${SPECS}

clean:
	rm -f *.pdf

pkoc-osdp-acu.pdf:	spec-src/pkoc-osdp-acu.md
	pandoc --toc --toc-depth=4 -o pkoc-osdp-acu.pdf spec-src/pkoc-osdp-acu.md

