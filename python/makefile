
setup:
	cat ../README.md | grep '^# IMCtermite' -A 50000 > ./README.md
	#pandoc -f markdown -t rst -o README.rst README.md
	#python -m rstvalidator README.rst
	cp -r ../lib ./
	cp -v ../LICENSE ./

setup-clean:
	rm -vf README.md README.rst LICENSE
	rm -rf lib/

build: setup
	python setup.py build

build-inplace: setup
	python setup.py build_ext --inplace

build-sdist: setup
	python setup.py sdist
	python -m twine check dist/*

build-bdist: setup
	python setup.py bdist
	python -m twine check dist/*

build-clean:
	python setup.py clean --all
	rm -vf imctermite*.so imctermite*.cpp
	rm -vf IMCtermite*.so IMCtermite*.cpp
	rm -rvf dist/ IMCtermite.egg-info/

cibuildwheel-build: setup
	cibuildwheel --platform linux

cibuildwheel-clean:
	rm -rvf wheelhouse/

pypi-upload:
	python -m twine upload dist/$(shell ls -t dist/ | head -n1)

clean: setup build-clean cibuildwheel-clean setup-clean

run-example:
	PYTHONPATH=$(pwd) python examples/usage_files.py
