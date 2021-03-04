# --------------------------------------------------------------------------- #

SHELL := /bin/bash

publish: sdist upload

sdist: ../cython/py_imc_termite.pyx ../cython/imc_termite.pxd ../cython/py_imc_termite.cpp
	cp -v $? ./
	cp -v $(shell ls ../lib/imc_*.hpp) ./
	tail -n 212 ../README.md > ./README.md
	cp -v ../LICENSE ./
	python3 setup.py sdist

upload:
	python3 -m twine upload dist/$(shell ls -t dist/ | head -n1)

clean:
	rm -rvf dist/
	rm -rvf *.egg-info
	rm -rvf build/
	rm -rvf cython/
	rm -vf *.pyx *.pxd
	rm -vf *.cpp *.c *.hpp
	rm -vf README.md LICENSE

# --------------------------------------------------------------------------- #
