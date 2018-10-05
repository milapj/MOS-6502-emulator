#!/bin/bash

tar -xvf patches.tar.gz 
for i in *.patch 
	do 
		patch -p1 < $i 
done 
