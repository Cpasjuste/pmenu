#!/bin/bash
pkgname=$1
folder=$3
xml=$2
mkisofs -o $pkgname.iso -R $folder
cat $pkgname.iso $xml >  $pkgname
rm $pkgname.iso
