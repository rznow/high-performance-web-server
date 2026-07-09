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


帖子的生成
console.log("MiniWebServer");

// 页面加载完成后自动获取帖子
window.onload = function () {

    loadPosts();

};


// 获取帖子列表
async function loadPosts() {

    const postList = document.getElementById("postList");

    postList.innerHTML = "";

    try {

        const response = await fetch("/posts");

        const data = await response.json();

        if (data.code !== 0) {

            postList.innerHTML =
                "<p>帖子加载失败</p>";

            return;
        }

        data.posts.forEach(post => {

            const card = document.createElement("div");

            card.className = "post-card";

            card.innerHTML = `
                <h3>${post.title}</h3>

                <p>${post.content}</p>

                <div class="post-footer">

                    <span>作者：${post.author}</span>

                    <span>${post.time}</span>

                </div>
            `;

            // 点击进入帖子详情
            card.onclick = function () {

                window.location.href =
                    "/post.html?id=" + post.post_id;

            };

            postList.appendChild(card);

        });

    }
    catch (e) {

        console.error(e);

        postList.innerHTML =
            "<p>服务器连接失败</p>";

    }

}