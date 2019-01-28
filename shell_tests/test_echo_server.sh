#!/bin/bash -x

function test_echo
{
    response="./shell_tests/actual_response"
    expected_response="./shell_tests/expected_response"
    curl -s localhost:5566 > $response
    if cmp -s $response $expected_response; then
        echo "Response body is correct"
        rm -f $response
        kill $1
        exit 0
    fi
}

./server ./shell_tests/test_config > /dev/null &
test_echo $SERVER_PID
echo "Response body is NOT correct"
kill $SERVER_PID
exit 1
