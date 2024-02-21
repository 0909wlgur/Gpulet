#!/bin/bash

batches=(1 2 4 8 16 32 64)
partitions=(20 40 50 60 80)

for batch in "${batches[@]}";
do
    cd "$batch"
    for partition in "${partitions[@]}"
    do
        cd $partition
        for file in *_infer_by_*.txt;
        do
            # 파일이 존재하지 않으면 스크립트 종료
            [ -e "$file" ] || continue
            # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            newname=$(echo "$file" | sed -e 's/vgg_19/vgg19/' \
                                        -e 's/resnet_50/resnet50/' \
                                        -e 's/densenet_121/densenet121/')
            # 파일 이름 변경
            mv "$file" "$newname"

        done

        for file in *_by_resnet_50_*.txt;
        do
            # 파일이 존재하지 않으면 스크립트 종료
            [ -e "$file" ] || continue
            # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            newname=$(echo "$file" | sed -e 's/resnet_50/resnet50/')
            # 파일 이름 변경
            mv "$file" "$newname"
        done

        for file in *_infer_by_inception_1_*.txt;
        do
            # 파일이 존재하지 않으면 스크립트 종료
            [ -e "$file" ] || continue
            # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            newname=$(echo "$file" | sed -e 's/inception_1_/inception_/')
            # 파일 이름 변경
            mv "$file" "$newname"
        done

        for file in *_infer_by_mobilenet_1_*.txt;
        do
            # 파일이 존재하지 않으면 스크립트 종료
            [ -e "$file" ] || continue
            # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            newname=$(echo "$file" | sed -e 's/mobilenet_1_/mobilenet_/')
            # 파일 이름 변경
            mv "$file" "$newname"
        done

        cd ..
    done
    cd ..
done
            

