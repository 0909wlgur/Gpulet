#!/bin/bash

batches=(1 2 4 8 16 32 64)
partitions=(20 40 50 60 80 100)

for batch in "${batches[@]}";
do
    for partition in "${partitions[@]}"
    do
        for file in vgg_19_*;
        do
            # 파일이 존재하지 않으면 스크립트 종료
            [ -e "$file" ] || continue
            # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            newname=$(echo "$file" | sed -e 's/vgg_19_/vgg19_/')
            # 파일 이름 변경
            mv "$file" "$newname"

        done
    done
done
            

