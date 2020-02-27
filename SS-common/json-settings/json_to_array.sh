#!/bin/bash

if [ $# -eq 0 ]; then
	echo "Provide json file name"
	exit -1 
fi

file=`basename $1`
filename="${file%.*}"
header_file="${filename}_json.h"
source_file="${filename}_json.c"

loc=$PWD
cd `dirname $1`
echo -e "#include \"${header_file}\"\n" > $source_file
tr -d '[:space:]' < $file > tmp.json
mv $file tmp
mv tmp.json $file
xxd -i $file >> $source_file
mv tmp $file
sed -e "s/unsigned //g" $source_file > tmp
sed -e 's/\(\[[^]]*\)\(\)/\11024/' tmp > $source_file
rm tmp

echo "#ifndef _${filename^^}_JSON_H" > $header_file
echo -e "#define _${filename^^}_JSON_H\n" >> $header_file
echo "extern char ${filename}_json[];" >> $header_file
echo "extern int ${filename}_json_len;" >> $header_file
echo -e "\n#endif" >> $header_file
cd $loc
sleep .3
