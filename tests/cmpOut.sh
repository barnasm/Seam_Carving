#!/bin/bash

IMG="images"
GPUS="_out"
CPUS="_cuda_out"
imgs=("10x10" "img" "img2")
imgsb=("img" "img2")

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    --mcc|--mem-check-cuda)
    	MEMCHECK=true
    	MCCUDA=true
    	shift # past argument
    	;;
    -m|--mc|--mem-check)
	MEMCHECK=true
	shift # past argument
	;;
    --default)
    MEMCHECK=false
    shift # past argument
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done


if $MEMCHECK = true && $MCCUDA = true; then
    run="cuda-memcheck ./main"
elif $MEMCHECK = true; then
    run="valgrind ./main"
else
    run="./main"
fi

# remove 0 px
for i in "${imgs[@]}"
do
    $run -s 0 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done

# remove 1 px
for i in "${imgs[@]}"
do
    $run -s 1 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done

# remove 5 px
for i in "${imgsb[@]}"
do
    $run -s 5 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done

# remove 100 px
for i in "${imgsb[@]}"
do
    $run -s 100 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done

# remove 250 px
for i in "${imgsb[@]}"
do
    $run -s 250 -f $i.bmp
    cmp --silent $IMG/$i$CPUS.bmp $IMG/$i$GPUS.bmp \
	&& echo "${green}### SUCCESS: Files Are Identical! ###${reset}" \
	|| echo "${red}### WARNING: Files Are Different! ###${reset}"
    echo""
done



