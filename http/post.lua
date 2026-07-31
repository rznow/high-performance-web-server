-- post.lua
wrk.method = "GET"

wrk.headers["Authorization"] =
"Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdmF0YXIiOiJcL2ltYWdlc1wvZGVmYXVsdF9hdmF0YXIucG5nIiwiZXhwIjoxNzg1NTk2Mzg1LCJpc3MiOiJyem5vdyIsInVzZXJfaWQiOiIyIiwidXNlcl9uYW1lIjoid2luZHkifQ.9M-d5AF1W_tgfYpneRzDCd9id77IVVN6j8pHBNCRaLo"

wrk.headers["Accept"] = "application/json"