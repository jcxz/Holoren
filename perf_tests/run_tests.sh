#!/usr/bin/bash

EXE_NAME="../build/release/holoren"
OF_W=1024
OF_H=1024
INPUT="../input/img/FIT-logo_160x160.png"
KERNEL="../src/OpenCL/holoren_obj_wave.cl"


# run the first set of tests (MUlti pass algorithm on CPU)
PERF_LOG="./Ubuntu/perf_MultiPassCPU.txt"
OUTPUT="./Ubuntu/MultiPassCPU.df"
ALG=MultiPassCPU

for i in `seq 5`; do
  "${EXE_NAME}" -z 0.5 -s 0.00002 -w ${OF_W} -h ${OF_H} -i "${INPUT}" -a ${ALG} -r opencl -o "${OUTPUT}" -f "${KERNEL}" -p "${PERF_LOG}"
done


# run the second set of tests (multipass algorithm on GPU)
PERF_LOG="./Ubuntu/perf_MultiPass.txt"
OUTPUT="./Ubuntu/MultiPass.df"
ALG=MultiPass

for i in `seq 5`; do
  "${EXE_NAME}" -z 0.5 -s 0.00002 -w ${OF_W} -h ${OF_H} -i "${INPUT}" -a ${ALG} -r opencl -o "${OUTPUT}" -f "${KERNEL}" -p "${PERF_LOG}"
done


# run the third set of tests  (multipass algorithm on GPU utilising native instructions)
PERF_LOG="./Ubuntu/perf_MultiPassNative.txt"
OUTPUT="./Ubuntu/MultiPassNative.df"
ALG=MultiPassNative

for i in `seq 5`; do
  "${EXE_NAME}" -z 0.5 -s 0.00002 -w ${OF_W} -h ${OF_H} -i "${INPUT}" -a ${ALG} -r opencl -o "${OUTPUT}" -f "${KERNEL}" -p "${PERF_LOG}"
done
