-- post.lua
wrk.method = "GET"

wrk.headers["Authorization"] =
"Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdmF0YXIiOiJcL2ltYWdlc1wvZGVmYXVsdF9hdmF0YXIucG5nIiwiZXhwIjoxNzg1NjgyMDEzLCJpc3MiOiJyem5vdyIsInVzZXJfaWQiOiI3IiwidXNlcl9uYW1lIjoicmFpbnkifQ.JjT5ZTtSQFkHmsE-WsVbsSskIj0k6dHDZdGlVFX7Qa8"

wrk.headers["Accept"] = "application/json"