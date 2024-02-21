for file in resnet_50_*.csv;
do
    # 파일이 존재하지 않으면 스크립트 종료
    [ -e "$file" ] || continue
    # 파일 이름에서 'vgg_19'를 'vgg19'로, 'resnet_50'을 'resnet50'로 변경
    newname=$(echo "$file" | sed -e 's/resnet_50/resnet50/')
    # 파일 이름 변경
    mv "$file" "$newname"   
done