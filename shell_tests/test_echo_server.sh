#!/bin/bash
temp="$(mktemp ./myscript.XXXXXX)"



function test_echo
{
    printf "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: length\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\nlicenseID=string&content=string&/paramsXML=string" | nc localhost 8079 > $temp
    body="$(tail -1 $temp)"
    realBody=$'licenseID=string&content=string&/paramsXML=string\r'
    rm -f $temp
    if [[ $body != $realBody ]]; then
        echo "$body"
        echo "$realBody"
        echo "Response body not correct"
        kill $1
        exit 1
    fi
}

../build/bin/server ./test_config > /dev/null &
SERVER_PID=$!
test_echo $SERVER_PID
kill $SERVER_PID
exit 0
