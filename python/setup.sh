#!/bin/bash

cat README.md | grep '^# IMCtermite' -A 50000 > python/README.md
cp -r lib python/
cp -v LICENSE python
