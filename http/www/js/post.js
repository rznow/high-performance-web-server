console.log("post page");


let id = getPostId();


// ========================
// 页面加载
// ========================

window.addEventListener("load",()=>{

    checkLogin();

    loadPost();

});



// ========================
// 获取帖子ID
// ========================

function getPostId()
{
    let params =
        new URLSearchParams(
            window.location.search
        );

    return params.get("id");
}



// ========================
// 获取当前用户ID
// ========================

function getCurrentUserId()
{
    let token =
        localStorage.getItem("token");

    if(!token)
        return null;


    let payload =
        JSON.parse(
            atob(
                token.split(".")[1]
            )
        );


    return Number(payload.user_id);
}



// ========================
// 登录检测
// ========================

async function checkLogin()
{

    const nav =
        document.getElementById("nav-user");


    const token =
        localStorage.getItem("token");


    if(!token)
    {

        nav.innerHTML=
        `
        <a href="/index.html">首页</a>
        <a href="/login.html">登录</a>
        <a href="/register.html">注册</a>
        `;

        return;

    }



    try
    {

        let res =
            await fetch(
                "/profile",
                {
                    headers:
                    {
                        Authorization:
                        "Bearer "+token
                    }
                }
            );


        let data =
            await res.json();



        if(data.code!==0)
        {

            localStorage.removeItem("token");

            location.href="/login.html";

            return;

        }



        nav.innerHTML = `
        <div class="user-info">

            <span class="username">
            ${data.user_name}
            </span>

            <a href="/profile.html" class="profile-link">
                <img class="avatar" src="${data.avatar}">
            </a>

            <a href="#" id="logout">
                退出
            </a>

        </div>
        `;


        document
        .getElementById("logout")
        .onclick=logout;


    }
    catch(e)
    {

        console.log(e);

    }

}



// ========================
// 退出
// ========================

function logout()
{
    localStorage.removeItem("token");

    location.replace("/login.html");
}



// ========================
// 加载帖子
// ========================

async function loadPost()
{

    if(!id)
    {
        alert("帖子不存在");
        return;
    }


    try
    {

        let token =
            localStorage.getItem("token");


        let res =
            await fetch(
                "/post?id="+id,
                {
                    headers:
                    {
                        Authorization:
                        "Bearer "+token
                    }
                }
            );


        let data =
            await res.json();



        if(data.code!==0)
        {
            document
            .getElementById("title")
            .innerHTML="帖子不存在";

            return;
        }



        let post =
            data.post;



        document
        .getElementById("title")
        .innerText =
            post.title;



        document
        .getElementById("author")
        .innerText =
            "作者:"+post.author;



        document
        .getElementById("time")
        .innerText =
            post.time;



        document
        .getElementById("content")
        .innerText =
            post.content;



        document
        .getElementById("likeCount")
        .innerText =
            "👍 "+post.like_count;



        document
        .getElementById("commentCount")
        .innerText =
            "💬 "+post.comment_count;


        document
        .getElementById("viewCount")
        .innerText =
            "👀 "+post.view_count;




        let uid =
            getCurrentUserId();



        if(post.user_id===uid)
        {

            document
            .getElementById("editBtn")
            .style.display="inline-block";


            document
            .getElementById("deleteBtn")
            .style.display="inline-block";

        }



        updateLikeButton(post.liked);

        loadComments(true);


    }
    catch(e)
    {

        console.error(e);

    }

}

const commentList =
    document.getElementById("commentList");

commentList.addEventListener("scroll", () => {

    if (
        commentList.scrollTop +
        commentList.clientHeight >=
        commentList.scrollHeight - 100
    ) {
        loadComments();
    }

});

// ========================
// 更新点赞按钮
// ========================

function updateLikeButton(liked)
{

    let btn =
        document.getElementById("likeBtn");


    btn.innerText =
        liked?
        "❤️ 已点赞":
        "🤍 点赞";

}



// ========================
// 点赞
// ========================

document
.getElementById("likeBtn")
.onclick=async function()
{

    let token =
        localStorage.getItem("token");


    if(!token)
    {
        alert("请先登录");
        return;
    }



    let res =
        await fetch(
            "/post/"+id+"/like",
            {

                method:"POST",

                headers:
                {
                    Authorization:
                    "Bearer "+token
                }

            }
        );



    let json =
        await res.json();



    if(json.code===0)
    {

        document
        .getElementById("likeCount")
        .innerText=
            "👍 "+json.like_count;



        updateLikeButton(json.liked);

    }

};

// ========================
// 编辑帖子
// ========================
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


// ========================
// 删除帖子
// ========================

document
.getElementById("deleteBtn")
.onclick=async function()
{

    if(!confirm("确定删除？"))
        return;



    let token =
        localStorage.getItem("token");



    let res =
        await fetch(
            "/posts/"+id,
            {

                method:"DELETE",

                headers:
                {
                    Authorization:
                    "Bearer "+token
                }

            }
        );



    let json =
        await res.json();


    if(json.code===0)
    {
        location.href="/index.html";
    }

};



// ========================
// 加载评论
// ========================

let commentTree = [];

let commentPage = 1;
const commentSize = 10;

let loadingComment = false;
let hasMoreComment = true;

function renderComments()
{
    const list = document.getElementById("commentList");

    list.innerHTML = "";

    commentTree.forEach(c=>{

        list.appendChild(createCommentDom(c));

    });
}

function createCommentDom(c)
{
    const div=document.createElement("div");

    div.className = "comment";
    div.id = `comment-${c.comment_id}`;
    div.dataset.id=c.comment_id;

    div.innerHTML=`
        <div class="comment-header">
            <img class="comment-avatar" src="${c.avatar || '/images/default_avatar.png'}" alt="avatar">
            <span class="comment-user">${c.author}</span>

            <span class="comment-time">${formatTime(c.time)}</span>
        </div>

        <div class="comment-content">${c.content}</div>

        <div class="comment-action">
            <button
                class="reply-btn"
                onclick="replyComment(
                    ${c.comment_id},
                    ${c.user_id},
                    '${c.author}'
                )">
                回复
            </button>
        </div>

        <div class="children"></div>
    `;

    const childrenBox=div.querySelector(".children");

    c.children.forEach(reply=>{

        childrenBox.appendChild(createReplyDom(reply));

    });

    return div;
}

function createReplyDom(c)
{
    const div=document.createElement("div");

    div.className="reply";
    div.id = `comment-${c.comment_id}`;

    div.dataset.id=c.comment_id;

    div.innerHTML=`
        <div class="comment-header">
            <img class="comment-avatar" src="${c.avatar || '/images/default_avatar.png'}" alt="avatar">
            <span class="comment-user">${c.author}
            </span>

            <span class="comment-time">${formatTime(c.time)}</span>

        </div>

        <div class="comment-content">回复<span class="reply-user">@${c.reply_author}</span>：
${c.content}
        </div>

        <div class="comment-action">

            <button
                class="reply-btn"
                onclick="replyComment(
                    ${c.comment_id},
                    ${c.user_id},
                    '${c.author}'
                )">

                回复

            </button>

        </div>

        <div class="children"></div>
    `;

    const box=div.querySelector(".children");

    c.children.forEach(child=>{

        box.appendChild(createReplyDom(child));

    });

    return div;
}

async function loadComments(reset = false)
{
    if(reset)
    {
        commentTree=[];
    }

    if(loadingComment || !hasMoreComment)
        return;

    loadingComment = true;

    try
    {
        let response =
            await fetch(
                `/posts/${id}/comments?page=${commentPage}&size=${commentSize}`
            );

        let data = await response.json();

        if(data.code != 0)
        {
            loadingComment = false;
            return;
        }
        
        commentTree.push(...data.comments);

        renderComments();

        if(data.comments.length < commentSize)
            hasMoreComment = false;
        else
            commentPage++;

    }
    finally
    {
        loadingComment = false;
    }
}

let currentParent=0;
let currentReplyUser=-1;

function replyComment(parentId, userId, author)
{
    currentParent = parentId;
    currentReplyUser = userId;

    const textarea =
        document.getElementById("comment");

    textarea.placeholder =
        "回复 @" + author;

    document.getElementById("replyBar").style.display = "flex";
    document.getElementById("replyName").innerText = "@" + author;

    textarea.focus();
}

document.getElementById("cancelReply").onclick = function(){

    currentParent = 0;
    currentReplyUser = -1;

    document.getElementById("comment").placeholder =
        "发表你的评论...";

    document.getElementById("replyBar").style.display = "none";
};

function formatTime(timeStr)
{
    const date = new Date(timeStr.replace(" ", "T"));
    const now = new Date();

    const pad = (n) => String(n).padStart(2, "0");

    // 不同年份
    if (date.getFullYear() !== now.getFullYear())
    {
        return `${date.getFullYear()}-${pad(date.getMonth() + 1)}-${pad(date.getDate())}`;
    }

    // 同一天
    if (
        date.getMonth() === now.getMonth() &&
        date.getDate() === now.getDate()
    )
    {
        return `${pad(date.getHours())}:${pad(date.getMinutes())}`;
    }

    // 同一年不同日期
    return `${pad(date.getMonth() + 1)}-${pad(date.getDate())}`;
}

// ========================
// 发送评论
// ========================

function insertReply(list,newComment)
{
    for(let c of list)
    {
        if(c.comment_id==newComment.parent_id)
        {
            c.children.push(newComment);

            return true;
        }

        if(insertReply(c.children,newComment))
            return true;
    }

    return false;
}

document
.getElementById("sendComment")
.onclick = async function()
{
    let token =
        localStorage.getItem("token");

    if(!token)
    {
        alert("请先登录");
        return;
    }

    let content =
        document.getElementById("comment").value.trim();

    if(content === "")
    {
        alert("评论不能为空");
        return;
    }

    let res =
    await fetch("/post/" + id + "/comments", {
        method: "POST",
        headers: {
            "Authorization": "Bearer " + token,
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            content: content,

            parent_id:currentParent,

            reply_user_id:currentReplyUser
        })
    });

    let json =
        await res.json();

    if(json.code === 0)
    {
        document.getElementById("comment").value = "";

        let c=json.comment;

        if(c.parent_id==0)
        {
            commentTree.unshift(c);
        }else
        {
            insertReply(commentTree,c);
        }

        renderComments();

        requestAnimationFrame(() => {
            scrollToComment(c.comment_id);
        });
    }
    else
    {
        alert(json.msg);
    }
};

function scrollToComment(commentId)
{
    const node = document.getElementById(`comment-${commentId}`);
    console.log(node);
    if(!node) return;

    node.scrollIntoView({
        behavior: "smooth",
        block: "center"
    });

    node.classList.add("comment-highlight");

    setTimeout(() => {
        node.classList.remove("comment-highlight");
    }, 2000);
}