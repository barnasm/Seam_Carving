#!/bin/bash

IMG="images"
GPUS="_out"
CPUS="_cuda_out"
imgs=("10x10" "img" "img2")
imgsb=("img" "img2")

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

# remove 0 px
for i in "${imgs[@]}"
do
    ./main -s 0 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done

# remove 1 px
for i in "${imgs[@]}"
do
    ./main -s 1 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done

# remove 5 px
for i in "${imgsb[@]}"
do
    ./main -s 5 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done

# remove 100 px
for i in "${imgsb[@]}"
do
    ./main -s 100 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done

# remove 250 px
for i in "${imgsb[@]}"
do
    ./main -s 250 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done



