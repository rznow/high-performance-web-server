console.log("post page");



window.onload=function(){

    loadPost();

};

//检验token
async function checkLogin()
{
    const token = localStorage.getItem("token");

    const nav = document.getElementById("nav-user");

    // 没登录
    if(!token)
    {
        nav.innerHTML = `
        <a href="/index.html">首页</a>
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

            location.href="/login.html";

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

    location.replace("/login.html");
}

window.addEventListener(
    "load",
    function(){

        checkLogin();

    }
);


// 获取帖子ID

function getPostId()
{

    let params =
        new URLSearchParams(
            window.location.search
        );


    return params.get("id");

}

let id=getPostId();

//获取当前用户id
function getCurrentUserId()
{
    const token =
        localStorage.getItem("token");

    if(!token)
        return null;

    const payload =
        JSON.parse(
            atob(
                token.split('.')[1]
            )
        );

    return Number(payload.user_id);
}


async function loadPost()
{


    if(!id)
    {

        alert("帖子不存在");

        return;

    }



    try{
        const token =
        localStorage.getItem("token");


        let response =
            await fetch("/post?id="+id,{
                
            method:"GET",

            headers:{

                Authorization:
                    "Bearer " + token

            }
                }

            );

        

        let data =
            await response.json();


        if(data.code!==0)
        {

            document.getElementById(
                "title"
            ).innerHTML =
                "帖子不存在";


            return;

        }




        let post=data.post;



        document.getElementById(
            "title"
        ).innerHTML =
            post.title;



        document.getElementById(
            "author"
        ).innerHTML =
            "作者:"
            +post.author;



        document.getElementById(
            "time"
        ).innerHTML =
            post.time;



        document.getElementById(
            "content"
        ).innerHTML =
            post.content;
        
        if(post.user_id == getCurrentUserId())
        {
            document.getElementById("editBtn").style.display = "inline-block";
            document.getElementById("deleteBtn").style.display = "inline-block";
        }
        
        document.getElementById("likeCount")
        .innerText="👍 "+post.like_count;

        const likeBtn =
            document.getElementById("likeBtn");

        if(post.liked)
        {
            likeBtn.innerText="❤️ 已点赞";
        }
        else
        {
            likeBtn.innerText="🤍 点赞";
        }
    }
    catch(e)
    {

        console.error(e);


        document.getElementById(
            "content"
        ).innerHTML =
        "服务器连接失败";

    }
}

document
.getElementById("likeBtn")
.onclick=async()=>{

    let token=
        localStorage.getItem("token");

    let res=
        await fetch(
            "/post/"+id+"/like",
            {
                method:"POST",

                headers:{
                    Authorization:
                    "Bearer "+token
                }
            }
        );

    let json=
        await res.json();

    if(json.code==0)
    {
        document
            .getElementById("likeCount")
            .innerText=
            "👍 "+json.like_count;
        
    }
    document.getElementById("likeBtn").innerText =
    json.liked ?
    "❤️ 已点赞" :
    "🤍 点赞";
}

document
.getElementById("deleteBtn")
.onclick=async()=>{

    if(!confirm("确定删除该帖子？"))
        return;

    let token=
        localStorage.getItem("token");

    let res=
        await fetch(
            "/posts/"+id,
            {
                method:"DELETE",

                headers:{
                    Authorization:
                    "Bearer "+token
                }
            }
        );

    let json=
        await res.json();

    if(json.code==0)
    {
        // alert("删除成功");

        location.href="/index.html";
    }

}

document
.getElementById("editBtn")
.onclick=function(){

    document.getElementById("titleEdit").value =
        document.getElementById("title").innerText;

    document.getElementById("contentEdit").value =
        document.getElementById("content").innerText;

    document.getElementById("title").style.display="none";
    document.getElementById("content").style.display="none";

    document.getElementById("titleEdit").style.display="block";
    document.getElementById("contentEdit").style.display="block";

    document.getElementById("editAction").style.display="block";

    document.getElementById("editBtn").style.display="none";
}

document
.getElementById("saveBtn")
.onclick=async()=>{

    const token=
        localStorage.getItem("token");

    const title=
        document.getElementById("titleEdit").value;

    const content=
        document.getElementById("contentEdit").value;

    let res=
        await fetch(
            "/posts/"+id,
            {

                method:"PUT",

                headers:{

                    Authorization:
                    "Bearer "+token,

                    "Content-Type":
                    "application/json"

                },

                body:JSON.stringify({

                    title:title,

                    content:content

                })

            }
        );

    let json=
        await res.json();

    if(json.code==0)
    {
        loadPost();

        document.getElementById("title").style.display="block";
        document.getElementById("content").style.display="block";

        document.getElementById("titleEdit").style.display="none";
        document.getElementById("contentEdit").style.display="none";

        document.getElementById("editAction").style.display="none";

        document.getElementById("editBtn").style.display="inline-block";
    }
}

document
.getElementById("cancelBtn")
.onclick=function(){

    document.getElementById("title").style.display="block";
    document.getElementById("content").style.display="block";

    document.getElementById("titleEdit").style.display="none";
    document.getElementById("contentEdit").style.display="none";

    document.getElementById("editAction").style.display="none";

    document.getElementById("editBtn").style.display="inline-block";
}
