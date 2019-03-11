#!/bin/bash

strindex() { 
  x="${1%%$2*}"
  [[ "$x" = "$1" ]] && echo -1 || echo "${#x}"
}

../build/bin/server ./test_config &
SERVER_PID=$!
sleep .5

# test create response
URI="/meme/create"
PostParameter="image=angry_lebron.jpg&top=asdf&bottom=blahblahblah"
expected_response1=$"Created meme"
expected_response2=$"<a href="
response=$(curl -X POST localhost:5566$URI -d "$PostParameter")
if [[ "$response" != *"$expected_response1"* ]]||[[ "$response" != *"$expected_response2"* ]]; then
    echo "$URI Response is NOT correct"
    echo -n "$response" > response.txt
    echo -n "$expected_response" > expected.txt
    kill $SERVER_PID
    exit 1
fi
echo "$response"
echo "Create response is correct"

hyperLink="<a href="
idPos=$(expr $(strindex "$response" "$hyperLink") + 24) # 24 is the length from <a href to start of id

id=$(echo "$response" | cut -c$idPos-$(expr $idPos + 35)) # 35 is the lenght of random ID
sleep .1

# echo $(curl -X GET localhost:5566/meme)

# test delete response
URI="/meme/delete?id=$id"
expected_response=$"Delete Success"
response=$(curl -X POST localhost:5566$URI)
if [[ "$response" != *"$expected_response"* ]]; then
    echo "$URI Response is NOT correct"
    echo -n "$response" > response.txt
    echo -n "$expected_response" > expected.txt
    kill $SERVER_PID
    exit 1
fi
echo "Delete response is correct"

# check view response
URI="/meme/view?id=$id"
expected_response=$"File Not Found"
response=$(curl -X GET localhost:5566$URI)
if [[ "$response" != *"$expected_response"* ]]; then
    echo "$URI Response is NOT correct"
    echo -n "$response" > response.txt
    echo -n "$expected_response" > expected.txt
    kill $SERVER_PID
    exit 1
fi
echo "View response is correct"



kill $SERVER_PID
exit 0
