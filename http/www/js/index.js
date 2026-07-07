// index.js

async function loadUser()
{

    const token =
        localStorage.getItem("token");

    if(!token)
    {
        location.href="/login.html";
        return;
    }

    const response =
        await fetch("/profile",{

        method:"GET",

        headers:{

            Authorization:
                "Bearer " + token

        }

    });

    const data =
        await response.json();

    if(data.code!=0)
    {
        localStorage.removeItem("token");

        location.href="/login.html";

        return;
    }

    document.getElementById("username").innerHTML =
        data.username;

}

loadUser();