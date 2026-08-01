async function loadProfile(){

    console.log("loadProfile start");

    const token=localStorage.getItem("token");

    const resp=await fetch("/profile",{

        headers:{
            Authorization:"Bearer "+token
        }

    });

    const data=await resp.json();

    if(data.code!=0){

        location.href="/login.html";

        return;
    }

    avatar.src = avatarUrl(data.avatar);

    username.innerText=data.user_name;

    // uid.innerText="UID:"+data.user_id;

    registerTime.innerText="注册时间："+data.register_time;

    postCount.innerText=data.post_count;

    commentCount.innerText=data.comment_count;

    likeCount.innerText=data.like_count;

}
window.addEventListener("load",()=>{

    loadProfile();

});


changeAvatar.onclick=()=>{

    avatarInput.click();

}

avatarInput.onchange=async function(){

    let file=this.files[0];

    if(!file)return;

    let form=new FormData();

    form.append("avatar",file);

    let token=localStorage.getItem("token");

    let resp=await fetch("/avatar",{

        method:"POST",

        headers:{
            Authorization:"Bearer "+token
        },

        body:form

    });

    let data=await resp.json();
    console.log(data);
    if (data.code == 0)
    {
        console.log("upload success");

        await loadProfile();

        console.log("loadProfile finished");
    }

}

function avatarUrl(url)
{
    return url + "?t=" + Date.now();
}