DOCS=testing-challenge-response.pdf USAGE-obtest-pkoc.pdf

all:	${DOCS}

clean:
	rm -f ${DOCS}

package:	all
	mkdir -p ../opt/tester/etc
	cp ${DOCS} ../opt/tester/etc

testing-challenge-response.pdf:	testing-challenge-response.md
	pandoc --toc -o testing-challenge-response.pdf testing-challenge-response.md

USAGE-obtest-pkoc.pdf:	USAGE-obtest-pkoc.md
	pandoc --toc -o USAGE-obtest-pkoc.pdf USAGE-obtest-pkoc.md

