#!/bin/bash

docker build . --tag=pyarrowbuild:latest

docker run -it pyarrowbuild:latest /bin/bash
