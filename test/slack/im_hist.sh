#!/bin/bash
secret=$(cat .secret)
channel=DD6QSBALB
curl -X GET \
    -H "Authorization: Bearer $secret" \
    -H 'Content-type: application/x-www-form-urlencoded' \
    https://slack.com/api/im.history?channel=$channel
