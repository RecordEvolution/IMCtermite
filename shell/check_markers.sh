##!/bin/bash/

dir=$1

#ls ${dir} | while read fn; do echo $fn; cat ${dir}$fn | grep -a "|[A-Z][A-Z]," -o | wc -l; done;

#ls ${dir} | while read fn; do echo $fn; cat ${dir}$fn | grep -a "|[A-Z][A-Z]," -o; done;

#ls ${dir} | while read fn; do echo $fn; cat ${dir}$fn | xxd | head -n10 | tail -n3; done;

ls ${dir} | while read fn; do echo $fn; cat ${dir}$fn | grep -a "|[A-Z][a-zA-Z]," -o | wc -l; done;
ls ${dir} | while read fn; do echo $fn; cat ${dir}$fn | grep -a "|[A-Z][a-zA-Z]," -o; done;
