<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<!--#include Virtual = "conn.asp"-->
<!--#include file="fydy.asp"-->
<%
set rs=conn.execute("select top 1 * from admin")
webmc=rs("wzmc")
bq=rs("bq")
sh=rs("sh")
fyts=clng(rs("fyts"))
if fyts=0 then fyts=5
rs.close:set rs=nothing

Function IsSelfRefer()
Dim sHttp_Referer, sServer_Name
sHttp_Referer = CStr(Request.ServerVariables("HTTP_REFERER"))
sServer_Name = CStr(Request.ServerVariables("SERVER_NAME"))
If Mid(sHttp_Referer, 8, Len(sServer_Name)) = sServer_Name Then
IsSelfRefer = True
Else
IsSelfRefer = False
End If
End Function

function getip(ip)     

       strIpArr = Split(ip,".")
 
       strGetUserIP = strIpArr(0)
       strGetUserIP = strGetUserIP & "." & strIpArr(1)
       strGetUserIP = strGetUserIP & "." & strIpArr(2)
       strGetUserIP = strGetUserIP & ".*"
       getip=strGetUserIP
end function


'创建对象 
Set mypage=new xdownpage 
'得到数据库连接 
mypage.getconn=conn 
'sql语句 
if sh=1 then
mypage.getsql="select * from book where sh=1 order by zd desc, id desc"
else
mypage.getsql="select * from book order by zd desc, id desc"
end if

'设置每一页的记录条数据为5条 
mypage.pagesize=fyts
'返回Recordset
set rs=mypage.getrs()  
'显示分页信息，这个方法可以，在set rs=mypage.getrs()以后,可在任意位置调用，可以调用多次 

%>
<head>
<meta content="text/html; charset=gb2312" http-equiv="Content-Type" />
<title><%=webmc%></title>
<meta content="<%=webmc%>" name="keywords" />
<meta content="<%=webmc%>" name="description" />
<link rel="stylesheet" type="text/css" href="www.163sy.cn.css"  />
<script type="text/javascript" src="js/jquery-1.4.2.js"></script>
<script type="text/javascript">
   $(document).ready(function() {  	   
	   $("#tj").click(function(){
		      $("#lysave").hide();$("#ts").show();
		      $.post("booksave.asp", $("#lysave").serialize(),function(data,textStatus){
                      alert(data);
                      $("#lysave").show();$("#ts").hide();
                      if(data=="您的留言成功!请等待管理员审核。"){
                      $("#lysave")[0].reset();
                      $("#yzt").attr("src","checkcode.asp?t="+new Date().getTime())
                      }
                      if(data=="验证码不正确"){
                      $("#yzt").attr("src","checkcode.asp?t="+new Date().getTime())
                      } 

					}
			);
	   })
	    
	})

</script>
</head>
<body>
<div id="book">
<div id="top" style="color:red"><%=webmc%></div>
<div class="cyleft1">
<div class="rbt1"><!--<span style="float:right;padding-right:10px;">未审核留言：<b style="color:red;"><%=conn.execute("select count(*) from book where sh<>1")(0)%>条</b></span>/-->所在位置：留言首页 → 留言列表</div>   

</div>
<div class="cyleft2">
<%

for i=1 to mypage.pagesize 
 
if not rs.eof then 
title=gotTopic(rs("TITLE"),100)

lyhf=rs("hf")
if lyhf<>"" then
lyhf=lyhf
else
if rs("sh")=1 then
lyhf="留言已审核 暂无回复！"
else
lyhf="留言处理中！"
end if
end if


response.write"<ul class='booklb'>"
response.write"<li class='lyzt'><span class='lynr1' style='color:green'>时间："&Format_Time(rs("sj"),4)&"</span>留言："&i&"  标题：<font style='color:blue'>"&title&"</font></li>"& vbCrLf

response.write"<li class='lynr'><label class='lybt'>留言内容：</label><br/>"&rs("content")&"<br/><span class='lysx'>姓名："&rs("name")&" 电话：" &left(rs("tel"),6)&"******* IP："&getip(rs("ip"))&"</span></li>"& vbCrLf
 
response.write"<li class='lyhf'><label class='lybt'><font style='color:red'>回复内容：</font></label><br/>"&lyhf&"</li>"& vbCrLf
response.write "</ul>"
 
 rs.movenext 
 else 
exit for 
end if
 next
                                                  
%>
<div class='clear'></div>
<div class="xtfy"><% mypage.showpage() %></div>
<div class="qxly">您的问题或留言</div>
<div id="ts" style="display:none;text-align:center;color:red;font-size:14px">留言提交处理中，请稍后！</div>
<form id="lysave">
<ol class="lylb">
<li>留言主题：<input class="bd" name="title" size="50" type="text" /> *</li>
<li>您的姓名：<input class="bd" name="name" type="text" /> *</li>
<li>联系电话：<input class="bd" name="tel" type="text" /> * 格式如：0592-5983163或13400693163</li> 
<li><span>留言内容：</span><textarea class="bd" cols="80" name="content" rows="5"></textarea></li>
<li>验 证 码：<input class=wenbenkuang type="text" name="code" maxLength=4 size=5> <img id="yzt" src="checkcode.asp"/></li>
<li><input style="margin-left:60px" name="Submit1" id="tj" type="button" value="提交" /> <input type="reset" value="重置"/></li>
</ol>
</form>
</div>
<div class="cyleft3"></div>
<div id="bottom"><%=bq%></div>
</div>
</body>
</html>
<%
rs.close
set rs=nothing
conn.close
set conn=nothing
%>
