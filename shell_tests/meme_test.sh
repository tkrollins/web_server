#!/bin/bash

function test_response
{
    response=$(curl -s localhost:5566$2)
    if [ "$response" != "$3" ]; then
        echo "$2 Response is NOT correct"
        echo -n "$response" > response.txt
        echo -n "$expected_response" > expected.txt
        kill $1
        exit 1
    fi
}

../build/bin/server ./meme_config &
SERVER_PID=$!
sleep .5

# test the submit page
URI="/meme/submit"
expected_response=$'<!DOCTYPE html>\n<html lang="en">\n<head>\n    <meta charset="UTF-8">\n    <title>Create a Meme!</title>\n</head>\n<h1>Create a Meme!</h1>\n<form action="/meme/create" method="post">\n    <select name="image">\n        <option>Select a template...</option>\n        <option value="angry_lebron.jpg">Angry LeBron</option>\n        <option value="bike_crash.jpg">Bike Crash</option>\n    </select><br>\n    <input type="text" name="top" placeholder="Top text..."><br>\n    <input type="text" name="bottom" placeholder="Bottom text..."><br>\n    <input type="submit" value="Create">\n</form>\n</body>\n</html>'
test_response $SERVER_PID $URI "$expected_response"
echo "Submit page is correct"


# test meme create and viewing the created meme
CREATE_RES=$(curl -X POST localhost:5566/meme/create -d "image=angry_lebron.jpg&top=asdf&bottom=blahblahblah")

# the substring of the returned HTML file delimited by < and > contains the meme id
MEME_ID=$(echo $CREATE_RES| cut -d'>' -f 2)
MEME_ID=$(echo $MEME_ID| cut -d'<' -f 1)

URI="/meme/view?id=$MEME_ID"
expected_response=$'<style>\n  body { display: inline-block; position: relative; }\n  span { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }\n  #top { top: 0; }\n  #bottom { bottom: 10; }\n</style>\n<body>\n  <img src="/memes/angry_lebron.jpg">\n  <span id="top">asdf</span>\n  <span id="bottom">blahblahblah</span>\n<span>\n<form action="/meme/submit">\n<input type="submit" value="Edit" />\n<input type="hidden" name="update" value="'$MEME_ID$'" /></form>\n<form action="/meme/delete" onclick="return confirm(\'Are you sure?\')">\n<input type="submit" value="Delete" />\n<input type="hidden" name="id" value="'$MEME_ID$'" /></form>\n</span>\n</body>'
test_response $SERVER_PID $URI "$expected_response"
echo "Can view created meme"


# test meme edit page
URI="/meme/submit?update=$MEME_ID"
expected_response=$'<!DOCTYPE html>\n<html lang="en">\n<head>\n    <meta charset="UTF-8">\n    <title>Edit a Meme!</title>\n</head>\n<h1>Edit a Meme!</h1>\n<form action="/meme/create" method="post">\n  <select name="image">\n    <option value="angry_lebron.jpg">Angry LeBron</option>\n    <option value="bike_crash.jpg">Bike Crash</option>\n  </select><br>\n  <input type="text" name="top" placeholder="Top text..." value="asdf"><br>\n  <input type="text" name="bottom" placeholder="Bottom text..." value="blahblahblah"><br>\n  <input type="hidden" name="update" value="'$MEME_ID$'">\n  <input type="submit" value="Update">\n</form>'
test_response $SERVER_PID $URI "$expected_response"
echo "Can view edit meme form"


# test meme editing -- change the image, top, and bottom text
EDIT_RES=$(curl -X POST localhost:5566/meme/create -d "image=bike_crash.jpg&top=asdf2&bottom=blahblahblah2&update=$MEME_ID")

# confirm that meme was edited properly
URI="/meme/view?id=$MEME_ID"
expected_response=$'<style>\n  body { display: inline-block; position: relative; }\n  span { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }\n  #top { top: 0; }\n  #bottom { bottom: 10; }\n</style>\n<body>\n  <img src="/memes/bike_crash.jpg">\n  <span id="top">asdf2</span>\n  <span id="bottom">blahblahblah2</span>\n<span>\n<form action="/meme/submit">\n<input type="submit" value="Edit" />\n<input type="hidden" name="update" value="'$MEME_ID$'" /></form>\n<form action="/meme/delete" onclick="return confirm(\'Are you sure?\')">\n<input type="submit" value="Delete" />\n<input type="hidden" name="id" value="'$MEME_ID$'" /></form>\n</span>\n</body>'
test_response $SERVER_PID $URI "$expected_response"
echo "Successfully edited meme"

# test meme delete -- retrieval of the same meme id after deletion should yield a 404 response
DEL_RES=$(curl -X GET localhost:5566/meme/delete?id=$MEME_ID)
URI="/meme/view?id=$MEME_ID"
expected_response=$'<!DOCTYPE html>\n<html lang="en">\n<head>\n    <meta charset="UTF-8">\n    <title>404</title>\n</head>\n<body>\n404 - File Not Found\n</body>\n</html>'
test_response $SERVER_PID $URI "$expected_response"
echo "Successfully deleted meme"

kill $SERVER_PID
exit 0