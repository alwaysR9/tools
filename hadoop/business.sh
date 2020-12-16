#!/usr/bin/bash

# sh run.sh [INPUT_dict_system] [OUTPUT] [key_name_of_dict_system] [partition]
source ../util.sh
set +x

if [[ $# -ne 4 ]]; then
    echo "[ERROR] param num is wrong"
    exit 1
fi

INPUT=$1
OUTPUT=$2
KEY_NAME=$3
PARTITION=$4

check_param() {
    if [[ ! "$INPUT" ]]; then
        echo "[ERROR] param input is empty"
        exit 1
    fi
    if [[ ! "$OUTPUT" ]]; then
        echo "[ERROR] param output is empty"
        exit 1
    fi
    if [[ ! "$KEY_NAME" ]]; then
        echo "[ERROR] param key_name is empty"
        exit 1
    fi
    if [[ ! "$PARTITION" ]]; then
        echo "[ERROR] param partition is empty"
        exit 1
    fi
}

function business() {
    local input=$1
    local output=$2
    if [[ ! "$input" ]]; then
        echo "[business] input path:[$input] is empty!"
        return 1
    fi
    if [[ ! "$output" ]]; then
        echo "[business] output path:[$output] is empty!"
        return 1
    fi

    local job_name="get_dict_system_partition_key"
    local reduce_num=50
    local mapper_cmd="python scripts/get_partition_data.py ${PARTITION}"
    local reducer_cmd="python scripts/get_key.py ${KEY_NAME}"
    local files="scripts/get_partition_data.py,scripts/get_key.py"
    MapReduce "${job_name}" "${reduce_num}" "${input}" "${output}" "${mapper_cmd}" \
        "${reducer_cmd}" "${files}" "10240" "10240" "10240" "10240"
    return $?
}

function main() {
    check_param
    business ${INPUT} ${OUTPUT}
    if [[ $? -ne 0 ]]; then
        echo "[ERROR] business fail"
        exit 1
    fi
}

main
