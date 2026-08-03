// index.js

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
        <div class="user-info">

            <a href="/profile.html" class="profile-link">
                <img class="avatar" src="${data.avatar}">
            </a>

            <a href="#" id="logout">
                退出
            </a>

        </div>
        `;

        document.getElementById("username").textContent = data.user_name;

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

    location.replace("/index.html");
}

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

//载入帖子
let page = 1;        // 当前页

let size = 10;       // 每次加载数量

let loading = false; // 防止重复请求

let hasMore = true;  // 是否还有数据
async function loadPosts()
{
    if(loading || !hasMore)
        return;
    loading = true;
    try
    {
        const response =
            await fetch(
                `/posts?page=${page}&size=${size}`
            );
        const data =
            await response.json();
        if(data.code !== 0)
        {
            return;
        }

        if(data.posts.length == 0)
        {
            return;
        }

        const postList =
            document.getElementById("postList");

        data.posts.forEach(post=>{
            const card =
                document.createElement("div");
            card.className =
                "post-card";
            card.innerHTML = `

                <h3>${post.title}</h3>

                <p>${post.content}</p>
                
                <div class="post-footer">

                    <span>
                    作者:${post.author}
                    </span>

                    <span>
                    ${post.time}
                    </span>

                </div>
            `;
            card.onclick=function(){
                window.location.href =
                "/post.html?id="
                +post.post_id;
            };
            postList.appendChild(card);
        });
        // 判断还有没有下一页

        if(data.posts.length < size)
        {
            hasMore=false;
        }
        else
        {
            page++;
        }
    }
    catch(e)
    {
        console.error(e);
    }
    loading=false;
}

async function init()
{
    await checkLogin();     // 更新在线状态

    await loadSiteInfo();   // 再统计在线人数

    loadPosts();            // 不必等待
}

window.addEventListener("load", init);

window.addEventListener(
    "scroll",
    function(){
    
        const scrollTop =
            document.documentElement.scrollTop;

        const windowHeight =
            window.innerHeight;

        const documentHeight =
            document.documentElement.scrollHeight;
        // 距离底部100px
    
        if(
            scrollTop + windowHeight
            >=
            documentHeight - 100
        )
        {
            loadPosts();
        }
    }
);

function renderSiteInfo(onlineCount, postCount)
{
    const siteInfo = document.getElementById("siteInfo");

    siteInfo.innerHTML = `
        <div class="card">
            <h3>站点信息</h3>

            <p>在线人数：${onlineCount}</p>

            <p>帖子数量：${postCount}</p>
        </div>
    `;
}

async function loadSiteInfo()
{
    const resp = await fetch("/siteInfo");
    const data = await resp.json();

    if(data.code !== 0)
        return;

    renderSiteInfo(
        data.online_count,
        data.post_count
    );
}

