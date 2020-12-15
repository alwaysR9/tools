function MapReduce()
{
<<!
example:

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

    local job_name="business"
    local reduce_num=50
    local mapper_cmd="python sku_store_info_test/wareid_partition_mapper.py"
    local reducer_cmd="python sku_store_info_test/merger.py"
    local files="sku_store_info_test/wareid_partition_mapper.py,sku_store_info_test/merger.py"
    MapReduce "${job_name}" "${reduce_num}" "${input}" "${output}" "${mapper_cmd}" \
        "${reducer_cmd}" "${files}" "10240" "10240" "10240" "10240"
    return $?
!
    local job_name=$1
    local reduce_num=$2
    local input=$3
    local output=$4
    local mapper_cmd=$5
    local reducer_cmd=$6
    local files=$7
    local map_mem=$8
    local map_java_mem=$9
    local reduce_mem=${10}
    local reduce_java_mem=${11}

    echo "job_name=$job_name"
    echo "reduce_num=$reduce_num"
    echo "input=$input"
    echo "output=$output"
    echo "mapper_cmd=$mapper_cmd"
    echo "reducer_cmd=$reducer_cmd"
    echo "files=$files"
    echo "map_mem=$map_mem"
    echo "map_java_mem=$map_java_mem"
    echo "reduce_mem=$reduce_mem"
    echo "reduce_java_mem=$reduce_java_mem"

    if [[ ! "${job_name}" ]]; then
        echo "job_name is empty"
        return 1
    fi
    if [[ ! "${reduce_num}" ]]; then
        echo "reduce_num is empty"
        return 1
    fi
    if [[ ! "${input}" ]]; then
        echo "input is empty"
        return 1
    fi
    if [[ ! "${output}" ]]; then
        echo "output is empty"
        return 1
    fi
    if [[ ! "${mapper_cmd}" ]]; then
        echo "mapper_cmd is empty"
        return 1
    fi
    if [[ ! "${reducer_cmd}" ]]; then
        echo "reducer_cmdis empty"
        return 1
    fi
    if [[ ! "${files}" ]]; then
        echo "files is empty"
        return 1
    fi
    if [[ ! "${map_mem}" ]]; then
        echo "map_mem is empty"
        return 1
    fi
    if [[ ! "${map_java_mem}" ]]; then
        echo "map_java_mem is empty"
        return 1
    fi
    if [[ ! "${reduce_mem}" ]]; then
        echo "reduce_mem is empty"
        return 1
    fi
    if [[ ! "${reduce_java_mem}" ]]; then
        echo "reduce_java_mem is empty"
        return 1
    fi

    $HADOOP_HOME/bin/hadoop fs -rm -r ${output}
    $HADOOP_HOME/bin/hadoop jar $HADOOP_STREAMING \
    -D mapred.job.name="${job_name}" \
    -D mapred.output.key.comparator.class=org.apache.hadoop.mapred.lib.KeyFieldBasedComparator \
    -D mapred.reduce.tasks=$reduce_num \
    -D mapred.task.timeout=7200000 \
    -D mapred.min.split.size=1073741824 \
    -D mapred.compress.map.output=true \
    -D mapreduce.map.cpu.vcores=2 \
    -D mapreduce.map.memory.mb=${map_mem} \
    -D mapreduce.map.java.opts=-Xmx${map_java_mem}M \
    -D mapreduce.reduce.cpu.vcores=2 \
    -D mapreduce.reduce.memory.mb=${reduce_mem} \
    -D mapreduce.reduce.java.opts=-Xmx${reduce_java_mem}M \
    -D yarn.app.mapreduce.am.resource.cpu-vcores=4 \
    -D yarn.app.mapreduce.am.resource.mb=7168 \
    -D yarn.app.mapreduce.am.command-opts=-Xmx6144m \
    -D mapreduce.task.io.sort.mb=1024 \
    -D mapreduce.job.reduce.slowstart.completedmaps=1 \
    -D mapreduce.input.fileinputformat.split.maxsize=512000000 \
    -D mapreduce.job.reduce.slowstart.completedmaps=1.0 \
    -D mapreduce.reduce.shuffle.input.buffer.percent=0.4 \
    -D mapred.min.split.size=1073741824 \
    -D mapred.compress.map.output=true \
    -D mapreduce.reduce.speculative=false \
    -D stream.num.map.output.key.fields=1 \
    -D stream.map.output.field.separator=\t \
    -files ${files} \
    -partitioner org.apache.hadoop.mapred.lib.KeyFieldBasedPartitioner \
    -mapper "${mapper_cmd}" \
    -reducer "${reducer_cmd}" \
    -input $input \
    -output $output
    if [ $? -ne 0 ]; then
        return 1
    fi
    return 0
}
