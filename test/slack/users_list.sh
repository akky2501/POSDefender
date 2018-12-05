#!/bin/bash
secret=$(cat .secret)
for user in {"UD4KRT0JD","USLACKBOT","UD4K8SX7F"}
do
    echo $user
    curl -X GET \
        -H "Authorization: Bearer $secret" \
        -H 'Content-type: application/x-www-form-urlencoded' \
        https://slack.com/api/users.info?user=$user
done
