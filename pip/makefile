# --------------------------------------------------------------------------- #

SHELL := /bin/bash

publish-source: sdist upload
publish-binary: bdist upload

prepdist: ../cython/py_imc_termite.pyx ../cython/imc_termite.pxd ../cython/py_imc_termite.cpp
	cp -v $? ./
	cp -v $(shell ls ../lib/imc_*.hpp) ./
	tail -n 212 ../README.md > ./README.md
	cp -v ../LICENSE ./

sdist: prepdist
	python3 setup.py sdist

# TODO use manylinux wheel to avoid ERROR "unsupported platform tag 'linux_x86_64'" 
# see: - https://github.com/pypa/manylinux
bdist: prepdist
	python3 setup.py bdist_wheel

# authentication:
# - username: __token__
# - password: <token value including pypi-prefix>
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
