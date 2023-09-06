#!/bin/sh
filesdir=$1
searchstr=$2

if [ $# -ne 2 ]
then
    echo "insufficient number of input parameters."
    exit 1
fi
if [ ! -d "$filesdir" ] 
then
  echo "specified path is not a directory."
  exit 1
fi
linesIncluded=$(grep -r "$searchstr" "$filesdir" | wc -l)
filesIncluded=$(grep -r -l "$searchstr" "$filesdir" | wc -l)
echo "The number of files are $filesIncluded and the number of \
matching lines are $linesIncluded."