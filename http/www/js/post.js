console.log("post page");



window.onload=function(){

    loadPost();

};




// 获取帖子ID

function getPostId()
{

    let params =
        new URLSearchParams(
            window.location.search
        );


    return params.get("id");

}




async function loadPost()
{


    let id=getPostId();


    if(!id)
    {

        alert("帖子不存在");

        return;

    }



    try{


        let response =
            await fetch(
                "/post?id="+id
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