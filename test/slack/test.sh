secret=$(cat .secret)
curl -X POST \
    -H "Authorization: Bearer $secret" \
    -H 'Content-type: application/json' \
    https://slack.com/api/api.test

