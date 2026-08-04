wrk.method = "POST"

wrk.body = [[
{
    "username":"admin",
    "password":"123456"
}
]]

wrk.headers["Content-Type"] = "application/json"