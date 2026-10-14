# Runs make clean, removes old results, makes all, runs tests and makes graphs

# Run make clean, remove old results, make all
function clean_make {
    make clean
    if [ $? -ne 0 ]; then
        echo "make clean failed"
        exit 1
    fi

    rm results/*

    make all
    if [ $? -ne 0 ]; then
        echo "make all failed"
        exit 1
    fi
}


# $1: num_threads $2: num_throws
# Run 1_1 test twice
function four_tests_for_1_1_instance {
    for i in {1..4}
    do
        ./bin/1_1 $1 $2
    done
}

function test_1_1 {
    # Tests for 100000000 num_throws
    four_tests_for_1_1_instance 1 100000000
    four_tests_for_1_1_instance 2 100000000
    four_tests_for_1_1_instance 4 100000000
    four_tests_for_1_1_instance 8 100000000
    four_tests_for_1_1_instance 16 100000000
}

# $1: num_threads $2: num_throws
# Run 1_2 test for both sync methods
function four_tests_for_1_2_instance {
    for i in {1..4}
    do
        ./bin/1_2 $1 $2 0
        ./bin/1_2 $1 $2 1
    done
}

function test_1_2 {
    four_tests_for_1_2_instance 1 10000000
    four_tests_for_1_2_instance 2 10000000
    four_tests_for_1_2_instance 4 10000000
    four_tests_for_1_2_instance 8 10000000
    four_tests_for_1_2_instance 16 10000000
}


function four_tests_for_1_3_instance {
    for i in {1..4}
    do
        # Check without padding
        ./bin/1_3 $1 $2 0

        # Check with padding
        ./bin/1_3 $1 $2 1
    done
}


function test_1_3 {
    four_tests_for_1_3_instance 1 1000000000
    four_tests_for_1_3_instance 2 1000000000
    four_tests_for_1_3_instance 4 1000000000
    four_tests_for_1_3_instance 8 1000000000
    four_tests_for_1_3_instance 16 1000000000
}


function four_tests_for_1_4_instance {
    for i in {1..4}
    do
        ./bin/1_4 $1 $2 $3 $4 $5 $6
    done
}

# run for those values of member
function run_tests_for_1_4_instance_members {
    four_tests_for_1_4_instance $1 $2 $3 0.99 0.01 0
    four_tests_for_1_4_instance $1 $2 $3 0.99 0.01 1
    four_tests_for_1_4_instance $1 $2 $3 0.95 0.05 0
    four_tests_for_1_4_instance $1 $2 $3 0.95 0.05 1
    four_tests_for_1_4_instance $1 $2 $3 0.90 0.10 0
    four_tests_for_1_4_instance $1 $2 $3 0.90 0.10 1
}

# Run 1_4 test for different values of threads, keys and operations
function test_1_4 {
    run_tests_for_1_4_instance_members 1 1000 50000
    run_tests_for_1_4_instance_members 2 1000 50000
    run_tests_for_1_4_instance_members 4 1000 50000
    run_tests_for_1_4_instance_members 8 1000 50000
    run_tests_for_1_4_instance_members 16 1000 50000
}


function make_graph {
    python3 graphs.py
}

# Get system information (hostname, cpu model, cpu cores, os version, gcc version)
function get_system_info {
    {   echo "Hostname: $(hostname)"
        echo "CPU Model: $(lscpu | grep 'Model name' | awk -F: '{print $2}' | xargs)"
        echo "CPU Cores: $(nproc)"
        echo "OS Version: $(lsb_release -d | awk -F: '{print $2}' | xargs)"
        echo "GCC Version: $(gcc --version | head -n 1)"
    } > results/system_info.txt
}


clean_make
get_system_info

test_1_1
test_1_2
test_1_3
test_1_4

make_graph