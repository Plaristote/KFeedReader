#!/bin/sh

cd "$(dirname "$0")"
DIR=`pwd`
packageRoot="../src"
packageName="FluxKap"

#---
echo "[build] Moving translation files to their target folder"

catalogs=`find . -name '*.po' | sort`
for cat in $catalogs; do
    echo "$cat"
    catLocale=`basename ${cat%.*}`

    target="$DIR/../po/${catLocale}/${packageName}.po"
    mkdir -p "$(dirname "$target")"
    cp "${catLocale}.po" "${target}"
done

echo "[build] Done building messages"
