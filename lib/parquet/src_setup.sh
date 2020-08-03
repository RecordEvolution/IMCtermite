#!/bin/bash

#-----------------------------------------------------------------------------#

# NOTE: before starting to extract the minimal required sources and dependencies
#       run
#       $ cd cpp/
#       $ cmake -D ARROW_PARQUET=ON
#      in the arrow repository

# provide
# - local path of clone of https://github.com/apache/arrow.git
# - name/path of main .hpp file of cython extension
repo="$1"
main="$2"
depf="$3"

# check CLI arguments
if [ -z "$repo" ] || [ -z "$main" ] || [ -z "$depf" ]; then
  echo "please provide..."
  echo "1. local path of arrow repository"
  echo "2. name of main .hpp/.cpp"
  echo "3. desired name of dependency file"
  echo -e "example:\n./setup-sources.sh /home/mario/Desktop/Record_Evolution/arrow/ reader-writer.cc deps.log"
  exit 1
fi

echo -e "extracting sources from/for \n1: ${repo}\n2: ${main}\n3: ${depf}\n"

# make sure the dependency file is empty
rm -f ${depf}
touch ${depf}

# define maximal recursion depth
maxdep=8

#-----------------------------------------------------------------------------#

# define function to list dependencies of source file in repository recursively
listDependencies()
{
  rep="$1"
  src="$2"
  dep="$3"
  rec="$4"

  echo -e "\nstarting 'listDependencies()' for\n1. ${rep}\n2. ${src}\n3. ${dep}\n4. ${rec}"

  # generate dependency file (and remove resulting object file)
  echo -e "g++ -c -MMD ${src} -I ${rep}cpp/src/\n"
  g++ -c -MMD ${src} -I ${rep}cpp/src/

  # derive name of dependency and object files
  depf=$(basename ${src} | sed 's/.cc/.d/g')
  objf=$(basename ${src} | sed 's/.cc/.o/g')
  rm ${objf}

  # list dependencies by
  # 1. removing header
  # 2. remove source itself
  # 3. delete leading spaces
  # 4. delete trailing backslashs
  # 5. remove empty lines
  cat ${depf} | grep ${rep} | grep -v ${src} | tr -d "^ " | tr -d "\\\\" | awk 'NF' > listdep.log
  # rm ${depf}

  while IFS= read -r fs
  do
    echo "$fs"
    # check if dependency is already in the list
    if grep -Fxq "$fs" "$dep"
    then
      echo "dep exist"
    else
      echo "dep does not exist yet => adding it"
      # add dependency to list
      echo "$fs" >> ${dep}
      # check for corresponding source file
      fssourc=$(echo ${fs} | sed 's/.h$/.cc/g' | sed 's/.hpp$/.cpp/g')
      echo ${fssourc}
      if [ -f "$fssourc" ]
      then
        echo "source file exists"
        # list nested dependencies
        if [ "$rec" -lt "$maxdep" ]
        then
          # increment recursion depth
          recinc=$(($rec+1))
          # call recursion
          listDependencies ${rep} ${fssourc} ${dep} ${recinc}
        else
          echo "maximal recursion depth exceeded"
        fi
      else
        echo "source file does not exist"
      fi
    fi
    echo ""
  done < listdep.log

  # cat listdep.log | while read fs
  # do
  #   echo $fs
  #   # check if dependency is already in the list
  #   inlist=$(cat listdep.log | grep ${fs} | wc -l)
  #   echo ${inlist}
  #   # check for any corresponding source files
  #   # if [ -f ]
  # done
}

#-----------------------------------------------------------------------------#

# call function to list dependencies (recursively)
listDependencies ${repo} ${main} ${depf} 0

# # generate dependency file (and remove resulting object file)
# echo -e "generate dependencies:\ng++ -c -MMD ${main} -I ./ -I ${repo}cpp/src/\n"
# g++ -c -MMD ${main} -I ${repo}cpp/src/
# rm $(echo ${main} | sed 's/.cc/.o/g')
#
# # derive name of dependency file
# dep=$(echo ${main} | sed 's/.cc/.d/g')
#
# if [ -f "$dep" ]; then
#
#   # list dependencies
#   cat ${dep} | sed 's/ /\n/g' | awk 'NF' | grep -v '\\' | grep '\/' > deps.log
#
#   # extract list of headers
#   cat deps.log | sed ':a;N;$!ba;s/\n/ /g' > deps-headers.log
#   echo "list of required headers ($(cat deps.log | wc -l))"
#   cat deps-headers.log
#   echo ""
#
#   # imply list of sources
#   cat deps.log | sed 's/.h$/.cc/g' | sed 's/.hpp$/.cpp/g' > sources_raw.log
#   cat sources_raw.log | while read f
#   do
#     if [ -f "$f" ]; then
#       echo $f >> sources_check.log
#     fi
#   done
#   cat sources_check.log | sed ':a;N;$!ba;s/\n/ /g' > deps-sources.log
#   echo "list of required sources ($(cat sources_check.log | wc -l))"
#   cat deps-sources.log
#   echo ""
#
#   # remove all temporary files
#   rm ${dep} deps.log
#   rm sources_raw.log sources_check.log
#
#   # copy required headers and sources
#   echo -e "copy required headers and sources"
#   mkdir temp/
#   cp --parents `cat deps-headers.log` temp/
#   cp --parents `cat deps-sources.log` temp/
#   mv temp${repo}cpp/src/* ./
#   rm -r temp
#
#   # remove dependencies
#   #rm deps-headers.log deps-sources.log
#
#   # show files
#   ls -lh
#
# else
#
#   echo -e "\nERROR: failed to generate dependency file\n"
#
# fi
