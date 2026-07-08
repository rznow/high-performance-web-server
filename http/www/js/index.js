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

//检验token
async function checkLogin()
{
    const token = localStorage.getItem("token");

    const nav = document.getElementById("nav-user");

    // 没登录
    if(!token)
    {
        nav.innerHTML = `
            <a href="/login.html">登录</a>
            <a href="/register.html">注册</a>
        `;

        return;
    }

    try
    {
        const response = await fetch("/profile",{

            headers:{
                Authorization:"Bearer " + token
            }

        });

        const data = await response.json();

        if(data.code != 0)
        {
            localStorage.removeItem("token");

            nav.innerHTML = `
                <a href="/login.html">登录</a>
                <a href="/register.html">注册</a>
            `;

            return;
        }

        nav.innerHTML = `
            <span class="username">
                欢迎，${data.user_name}
            </span>

            <a href="#" id="logout">
                退出
            </a>
        `;

        document.getElementById("logout")
            .onclick = logout;
    }
    catch(err)
    {
        console.log(err);
    }
}

//退出登录
function logout()
{
    localStorage.removeItem("token");

    location.reload();
}

window.onload = function () {
    checkLogin();
};


//发帖
async function gotoNewPost()
{
    const token = localStorage.getItem("token");

    if(!token)
    {
        location.href="/login.html";
        return;
    }

    const response = await fetch("/profile",{

        headers:{
            Authorization:"Bearer "+token
        }

    });

    const data = await response.json();

    if(data.code==0)
    {
        location.href="/newPost.html";
    }
    else
    {
        localStorage.removeItem("token");
        location.href="/login.html";
    }
}

document
.getElementById("newPost")
.addEventListener("click",gotoNewPost);