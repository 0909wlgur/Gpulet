import pandas as pd
import numpy as np
import csv

# models = ("vgg16", "vgg19", "resnet50", "resnet101", "resnet152", "densenet121", "densenet169", "densenet201", "inception", "mobilenet")
models = ("BERT")
batches = (1, 2, 4, 8, 16, 32, 64)
partitions = (20, 40, 50, 60, 80)

def average_data(input_file_path):
    # NCU 결과 파일 위치 
    input_file_csv_path = input_file_path + ".csv"

    # 1차 수정본(metric name, kernel name, metric unit, average만 추출)
    # 1차 수정 CSV 파일 저장 경로
    first_output_file_path = input_file_path + "_1st_preprocessing" + ".csv"
    # '/root/research/jh/igniter/implement/data/resnet50_1st_preprocessing_1000.csv'

    # 2차 수정본(kernal name 별로 metric 정리)
    # 2차 수정 CSV 파일 저장 경로
    second_output_file_path = input_file_path + "_2nd_preprocessing" + ".csv"

    # 최종 profiling 결과 저장 경로
    metric_average_output_path = input_file_path + "_final_result" + ".txt"

    # CSV 형식 데이터가 시작되는 지점 찾기
    csv_start_line = None
    with open(input_file_csv_path, 'r') as file:
        for i, line in enumerate(file):
            if '"ID"' in line:  # 줄에 "ID" 문자열이 포함되어 있는지 확인
                csv_start_line = i
                break

    # CSV 형식 데이터 읽기
    if csv_start_line is not None:
        data = pd.read_csv(input_file_csv_path, header=None, skiprows=csv_start_line, quotechar='"')
        # data.columns = ["ID","Process ID","Process Name","Host Name","Kernel Name",
        #                 "Context","Stream","Block Size","Grid Size","Device","CC",
        #                 "Section Name","Metric Name","Metric Unit","Metric Value"]
        data.columns = ["ID","Process ID","Process Name","Host Name","Kernel Name",
                        "Kernel Time","Context","Stream","Section Name","Metric Name",
                        "Metric Unit","Metric Value"]
        # 관심있는 열 선택
        first_processing_data = data[["Metric Name", "Kernel Name", "Metric Unit", "Metric Value"]]
    else:
        raise ValueError("CSV format data not found in the file")

    # 인덱스를 재설정하기 전에 불필요한 행 제거
    # first_processing_data = first_processing_data.drop(index=0)  # 예시: 인덱스 1의 행을 제거

    # 인덱스 재설정
    # first_processing_data.reset_index(drop=True, inplace=True)

    # 새로운 CSV 파일로 저장
    first_processing_data.to_csv(first_output_file_path, index=False, quoting=csv.QUOTE_ALL, quotechar='"')


    # 1차 정리본 데이터 읽기
    first_data = pd.read_csv(first_output_file_path, skiprows=[0, 0], header=None)
    first_data.columns = ["Metric Name", "Kernel Name", "Metric Unit", "Metric Value"]
    # print(len(first_data))
    
    # unique metric name extracting
    unique_metrics = first_processing_data['Metric Name'].unique()
    
    # print(unique_metrics)
    second_columns = ['Kernel Name'] + list(unique_metrics)
    second_processing_data = pd.DataFrame(columns=second_columns)
    # print(second_processing_data.columns)

    # 데이터 처리
    # print(first_data.at[0, "Metric Name"])
    for i in range(0, len(first_data), unique_metrics.size):
        # 각 "Kernel Name"에 대한 "Metric Name"과 "Metric Value" 추출
        metrics = []
        metrics.append(first_data.at[i, "Kernel Name"])
        for j in range(0, unique_metrics.size, 1):
            metrics.append(first_data.at[i + j, "Metric Value"])
        second_processing_data.loc[len(second_processing_data)] = metrics
        
    second_processing_data["gpu__time_duration.sum"] = second_processing_data["gpu__time_duration.sum"] / 1000
    second_processing_data.rename(columns={"gpu__time_duration.sum": "gpu__time_duration.sum[us]"}, inplace=True)

    # 2차 수정본(해당하는 kernel name에 맞춰서 한줄로 결과 정리) 결과 저장
    second_processing_data.to_csv(second_output_file_path, index=False)

    # 데이터 로드
    second_data = pd.read_csv(second_output_file_path)

    # 가중 평균을 계산할 컬럼
    columns_to_average = [metric for metric in unique_metrics if metric != "gpu__time_duration.sum"]

    # 각 컬럼에 대한 가중 평균 계산
    weighted_averages = {}
    for column in columns_to_average:
        weighted_sum = (second_data[column] * second_data["gpu__time_duration.sum[us]"]).sum()
        total_weight = second_data["gpu__time_duration.sum[us]"].sum()
        weighted_average = weighted_sum / total_weight
        weighted_averages[column] = weighted_average

    output_file = open(metric_average_output_path, 'w')
    for column, weighted_average in weighted_averages.items():
        output_file.write(f"Weighted average of {column}: {weighted_average:.2f}%\n")
    output_file.write(f"The overall execution time is {total_weight:.2f}us\n")
    output_file.close()
    
if __name__ == "__main__":
    
    for model in models:        
        for batch in batches:
            for partition in partitions:
                model = "BERT"
                model_path = f"/root/research/jh/gpulet/profiling/ncu/{model}/{model}_{batch}_{partition}_ncu"
                print(model_path)
                average_data(model_path)

