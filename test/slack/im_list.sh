secret=$(cat .secret)
curl -X GET \
    -H "Authorization: Bearer $secret" \
    -H 'Content-type: application/x-www-form-urlencoded' \
    https://slack.com/api/im.list

