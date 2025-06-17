# make file for test crypto routines

all:
	#(cd test-AES; make)
	#(cd test-RSA; make)
	(cd test-EC; make package);
	(cd test-DIGEST; make package);

documentation:
	mkdir -p ../opt/openbadger/etc

package:	all
	echo (cd test-RSA; make package)
	(cd test-EC; make package)

clean:
	# (cd test-AES; make clean)
	# (cd test-RSA; make clean)
	(cd test-EC; make clean)
	# (cd test-DIGEST; make clean)

