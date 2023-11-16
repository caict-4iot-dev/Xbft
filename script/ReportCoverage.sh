#!/bin/sh

echo "start initialize lcov coverage"
CURRENT_DIR=$(cd "$(dirname "$0")";pwd)
TEST_DIR=${CURRENT_DIR}"/../build/test"
ROOT_DIR=$(dirname "${CURRENT_DIR}")

cd $TEST_DIR
lcov -d $ROOT_DIR -t test -o test.info -b . -c  
echo lcov --remove test.info  '/opt/rh/*' '/usr/include/*' '/usr/lib/*'  '/usr/local/*' '/home/bif-3rd/*' ${ROOT_DIR}'/src/protos/*' ${ROOT_DIR}'/test/*' ${ROOT_DIR}'/src/app/*' ${ROOT_DIR}'/src/ref/*' -o test.info
lcov --remove test.info  '/opt/rh/*' '/usr/include/*' '/usr/lib/*'  '/usr/local/*' '/home/bif-3rd/*' ${ROOT_DIR}'/src/protos/*' ${ROOT_DIR}'/test/*' ${ROOT_DIR}'/src/app/*' ${ROOT_DIR}'/src/ref/*' -o test.info

genhtml -o ${TEST_DIR}/report ${TEST_DIR}/test.info 
exit 1

