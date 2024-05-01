#!/bin/bash

PROJECT_ROOT=$(cd $(dirname "$0"); pwd)/../../
COMMON_TEST_DIR=${PROJECT_ROOT}/common/test
BIN_DIR=${PROJECT_ROOT}/output/bin/common_test/

cp ${COMMON_TEST_DIR}/config.yaml ${BIN_DIR}

cd ${BIN_DIR}

TEST_EXEC="archive_base_test \
           archive_binary_test \
           archive_text_test \
           channel_test \
           config_test \
           factory_test \
           fs_local_test \
           hash_test \
           line_reader_test \
           random_test \
           scope_exit_test \
           sem_test \
           shell_test \
           string_tools_test \
           thread_barrier_test \
           thread_group_async_channel_test \
           thread_group_async_condition_test \
           thread_group_test \
           thread_managed_test \
           time_related_test"

for test in ${TEST_EXEC}; do
    echo "run "${test}
    ${BIN_DIR}/${test}
    echo ""
done
