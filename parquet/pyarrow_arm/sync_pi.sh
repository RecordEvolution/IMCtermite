#!/bin/bash

if [ -z "$1" ]
then
	exit 1
fi

scp $1 pirate@mf-pi-40:/home/pirate/pyarrow/
