#!/bin/bash

# provide
# - local path of clone of https://github.com/apache/arrow.git
# - name/path of main .hpp file of cython extension
repo="$1"
main="$2"

# check CLI arguments
if [ -z "$repo" ] || [ -z "$main" ]; then
  echo "please provide local path of arrow repository and name of main .hpp/.cpp"
  echo -e "example:\n./setup-sources.sh /home/mario/Desktop/Record_Evolution/arrow/ reader-writer.cc"
  exit 1
fi

echo -e "extracting sources from/for \n1: ${repo}\n2: ${main}\n"

# generate dependency file
echo -e "generate dependencies:\ng++ -c -MMD ${main} -I ./ -I ${repo}cpp/src/\n"
g++ -c -MMD ${main} -I ${repo}cpp/src/

# derive name of dependency file
dep=$(echo ${main} | sed 's/.cc/.d/g')

if [ -f "$dep" ]; then

  # list dependencies
  cat ${dep} | sed 's/ /\n/g' | awk 'NF' | grep -v '\\' | grep '\/' > deps.log

  # extract list of headers
  cat deps.log | sed ':a;N;$!ba;s/\n/ /g' > deps-headers.log
  echo "list of required headers ($(cat deps.log | wc -l))"
  cat deps-headers.log
  echo ""

  # imply list of sources
  cat deps.log | sed 's/.h$/.cc/g' | sed 's/.hpp$/.cpp/g' > sources_raw.log
  cat sources_raw.log | while read f
  do
    if [ -f "$f" ]; then
      echo $f >> sources_check.log
    fi
  done
  cat sources_check.log | sed ':a;N;$!ba;s/\n/ /g' > deps-sources.log
  echo "list of required sources ($(cat sources_check.log | wc -l))"
  cat deps-sources.log
  echo ""

  # remove all temporary files
  rm ${dep} deps.log sources_raw.log sources_check.log

  # copy required headers and sources
  echo -e "copy required headers and sources"
  mkdir temp/
  cp --parents `cat deps-headers.log` temp/
  cp --parents `cat deps-sources.log` temp/
  mv temp${repo}cpp/src/* ./
  rm -r temp

  # remove dependencies
  rm deps-headers.log deps-sources.log

  # show files
  ls -lh

else

  echo -e "\nERROR: failed to generate dependency file\n"

fi
