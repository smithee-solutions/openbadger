# make file for documentation for openbadger

DOCS=USAGE-openbadger.pdf

all:	${DOCS}

clean:
	rm -f ${DOCS}

package:	all
	mkdir -p ../opt/openbadger/etc
	cp ${DOCS} ../opt/openbadger/etc

USAGE-openbadger.pdf:	doc-src/USAGE-openbadger.md
	pandoc --toc -o USAGE-obpenbadger.pdf doc-src/USAGE-openbadger.md


