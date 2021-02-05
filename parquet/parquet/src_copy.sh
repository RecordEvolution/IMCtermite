#!/bin/bash

mkdir src
cat deps.log | while read f; do cp --parents $f src/; done;
mv src/home/mario/Desktop/arrow/cpp/* src/
rm -r src/home/
