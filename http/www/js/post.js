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
let liked=false;

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
        
        if(post.user_id==getCurrentUserId())
        {
            document
                .getElementById("deleteBtn")
                .style.display="inline-block";
        }

        document.getElementById("likeCount")
        .innerText="👍 "+post.like_count;

        const likeBtn =
            document.getElementById("likeBtn");

        liked = post.liked
        if(liked)
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
    liked = !liked
    if(liked)
    {
        likeBtn.innerText="❤️ 已点赞";
    }
    else
    {
        likeBtn.innerText="🤍 点赞";
    }
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
