#!/bin/bash

models=("densenet121" "resnet50" "vgg19")
batches=(1 2 4 8 16 32 64)
partitions=(20 40 50 60 80)

basic_filepath=$(pwd)
for model in "${models[@]}"
do
    model_filepath="${basic_filepath}/${model}"
    for batch in "${batches[@]}";
    do
        batch_filepath="${model_filepath}/${batch}"
        for partition in "${partitions[@]}"
        do
            partition_filepath="${batch_filepath}/${partition}"

            cd ${partition_filepath}

            # for file in *.txt;
            # do
            #     # 파일이 존재하지 않으면 스크립트 종료
            #     [ -e "$file" ] || continue
            #     # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            #     newname=$(echo "$file" | sed -e 's/resnet_50_/resnet50_/' \
            #                            | sed -e 's/vgg_19_/vgg19_/' \
            #                            | sed -e 's/densenet_121_/densenet121_/')
            #     # 파일 이름 변경
            #     mv "$file" "$newname"
            # done

            # for file in *_inception1_*;
            # do
            #     # 파일이 존재하지 않으면 스크립트 종료
            #     [ -e "$file" ] || continue
            #     # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            #     newname=$(echo "$file" | sed -e 's/_inception1_/_inception_/')
            #     # 파일 이름 변경
            #     mv "$file" "$newname"
            # done

            # for file in *_mobilenet1_*;
            # do
            #     # 파일이 존재하지 않으면 스크립트 종료
            #     [ -e "$file" ] || continue
            #     # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            #     newname=$(echo "$file" | sed -e 's/_mobilenet1_/_mobilenet_/')
            #     # 파일 이름 변경
            #     mv "$file" "$newname"
            # done

            # for file in *_resnet_50_*;
            # do
            #     # 파일이 존재하지 않으면 스크립트 종료
            #     [ -e "$file" ] || continue
            #     # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            #     newname=$(echo "$file" | sed -e 's/_resnet_50_/_resnet50_/')
            #     # 파일 이름 변경
            #     mv "$file" "$newname"
            # done

            # for file in *_vgg_19_*;
            # do
            #     # 파일이 존재하지 않으면 스크립트 종료
            #     [ -e "$file" ] || continue
            #     # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
            #     newname=$(echo "$file" | sed -e 's/_vgg_19_/_vgg19_/')
            #     # 파일 이름 변경
            #     mv "$file" "$newname"
            # done

            for file in *_inception_1_*;
            do
                # 파일이 존재하지 않으면 스크립트 종료
                [ -e "$file" ] || continue
                # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
                newname=$(echo "$file" | sed -e 's/_inception_1_/_inception_/')
                # 파일 이름 변경
                mv "$file" "$newname"
            done

            for file in *_mobilenet_1_*;
            do
                # 파일이 존재하지 않으면 스크립트 종료
                [ -e "$file" ] || continue
                # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
                newname=$(echo "$file" | sed -e 's/_mobilenet_1_/_mobilenet_/')
                # 파일 이름 변경
                mv "$file" "$newname"
            done
        done
    done
done
