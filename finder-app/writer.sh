#!/bin/sh
writefile=$1
writestr=$2
if [ $# -ne 2 ]
then
    echo "insufficient number of input arguments."
    exit 1
fi
mkdir -p "$(dirname "$writefile")" && touch "$writefile"    
echo $writestr >> $writefile