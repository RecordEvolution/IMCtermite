##!/bin/bash/

fildir=$1

if [ -z "${fildir}" ]; then
  echo "CLI argument missing: provide file or directory" >&2
  exit 1
fi

list-markers()
{
  arg="$1"
  if [ -z "${arg}" ]; then
    echo "list-markers: missing file argument" >&2
    exit 1
  else
    if [ -d "${arg}" ]; then
      echo "list-markers: file argument is a directory" >&2
      exit 1
    fi
  fi
  
  echo -e "\n${arg}\n"
  mrks=$(cat ${arg} | grep -a "|[A-Z][a-zA-Z]," -o)
  mrksnum=$(echo "${mrks}" | wc -l)
  echo -e "(${mrksnum})\n${mrks}"
}

if [ -f "${fildir}" ]; then

  echo "analyzing single file ${fildir} ..."
  list-markers "${fildir}"

elif [ -d "${fildir}" ]; then

  echo "analyzing all *.raw files in directory ${fildir} ..."
  lsfls=$(ls ${fildir}/*.raw | sed 's/\/\//\//g')
  echo -e "\n${lsfls}"

  for fl in ${lsfls}; do
    list-markers "${fl}"
  done

else

  echo "does not exist: ${fildir}" >&2
  exit 1

fi

