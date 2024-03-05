DOCS=openbadger-operation.pdf testing-challenge-response.pdf USAGE-obtest-pkoc.pdf \
  USAGE-obtest-piv.pdf

all:	${DOCS}

clean:
	rm -f ${DOCS}

package:	all
	mkdir -p ../opt/tester/etc
	cp ${DOCS} ../opt/tester/etc

openbadger-operation.pdf:	openbadger-operation.md
	pandoc --toc -o openbadger-operation.pdf openbadger-operation.md

testing-challenge-response.pdf:	testing-challenge-response.md
	pandoc --toc -o testing-challenge-response.pdf testing-challenge-response.md

USAGE-obtest-pkoc.pdf:	USAGE-obtest-pkoc.md
	pandoc --toc -o USAGE-obtest-pkoc.pdf USAGE-obtest-pkoc.md

USAGE-obtest-piv.pdf:	USAGE-obtest-piv.md
	pandoc --toc -o USAGE-obtest-piv.pdf USAGE-obtest-piv.md

