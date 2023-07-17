<!--#include file="../fydy.asp"-->
<!--#include file="conn.asp"-->
<%
session("url")=GetUrl() 
if not session("check")="checked" then
response.Redirect "login.asp"
end if
action=request("action")
key=request("key")
classid=clng(request("classid"))

'创建对象 
Set mypage=new xdownpage 
'得到数据库连接 
mypage.getconn=conn 
'sql语句 

if key="" then
mypage.getsql="select * from book order by id desc"
else
mypage.getsql="select * from book where Title like '%" & key & "%'  order by id desc"
end if


'设置每一页的记录条数据为5条 
mypage.pagesize=20
'返回Recordset
set rs=mypage.getrs()  
'显示分页信息，这个方法可以，在set rs=mypage.getrs()以后,可在任意位置调用，可以调用多次 

%>
<head>
<style type="text/css">
<!--
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #F8F9FA;
}
.style1 {
	border-width: 1px;
}
-->
</style>
<SCRIPT language=javascript>
function CheckAll(form)
{
  for (var ii=0;ii<form.elements.length;ii++)
    {
    var e = form.elements[ii];
    if (e.Name != "chkAll")
       e.checked = form.chkAll.checked;
    }
}
function Checked()
{
	var jj = 0
	for(ii=0;ii < document.form.elements.length;ii++){
		if(document.form.elements[ii].name == "adid"){
			if(document.form.elements[ii].checked){
				jj++;
			}
		}
	}
	return jj;
}

function DelAll()
{
	if(Checked()  <= 0){
		alert("您至少选择1条信息!");
	}	
	else{
		if(confirm("确定要删除选择的留言吗？\n此操作不可以恢复！")){
			form.action="delbook.asp?del=data";
			form.submit();
		}
	}
}
function sh()
{
	if(Checked()  <= 0){
		alert("您至少选择1条信息!");
	}	
	else{
		if(confirm("确定要通过所选择的留言吗？")){
			form.action="sh.asp?sh=data";
			form.submit();
		}
	}
}
</SCRIPT>
<link href="images/skin.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../js/jquery-1.4.2.js"></script>
<script type="text/javascript">
$(function(){
$("#bs tr").hover(function(){
$(this).addClass("bs");
},function(){
$(this).removeClass();
});
});
</script>
</head>

<body>
<table width="100%" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="17" valign="top" background="images/mail_leftbg.gif"><img src="images/left-top-right.gif" width="17" height="29" /></td>
    <td valign="top" background="images/content-bg.gif"><table width="100%" height="31" border="0" cellpadding="0" cellspacing="0" class="left_topbg" id="table2">
      <tr>
        <td height="31"><div class="titlebt">留言管理</div></td>
      </tr>
    </table></td>
    <td width="16" valign="top" background="images/mail_rightbg.gif"><img src="images/nav-right-bg.gif" width="16" height="29" /></td>
  </tr>
  <tr>
    <td height="71" valign="middle" background="images/mail_leftbg.gif">　</td>
    <td valign="top" bgcolor="#F7F8F9"><table width="100%" height="138" border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td height="13" valign="top">&nbsp;</td>
      </tr>
      <tr>
        <td valign="top"><table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td class="left_txt" colspan="2">当前位置：留言管理</td>
          </tr>
          <tr>
            <td >
            
 <div align="center">          
            
 <table width="100%" border="1" cellpadding="0" cellspacing="0" class="list" bordercolor="#C0C0C0" style="border-collapse: collapse">
   <form method="get" action="">
  <tr> 
    <th align=left width="100%" height="25" align="center" colspan="9" style="padding-left:5px; padding-right:5px; padding-top:2px; padding-bottom:2px">
    <span style="float:right"><INPUT title=审核 onclick=sh() type=button value=审核 name=Submit>&nbsp;<INPUT title=删除 onclick=DelAll() type=button value=删除 name=Submit></span>
<input type="text" name="key" value="<%=key%>" size="20"> 
<input type="submit" value=" 搜 索 " name="B1">
	</th>
  </tr>
  </form>
<form id="bs" name=form method=post action="?action=xg">  
  <tr> 
    <td height="25" align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b><span class="style1">ID</span></b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<span class="style1"><b>留言主题</b></span></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b>姓名</b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b>电话</b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b><span class="style1">日期</span></b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b>置顶</b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b>审核</b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB" width="10%">
	<b><span class="style1">操作</span></b></td>
    <td width="5%" align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
                  <input id=chkAll onClick=CheckAll(this.form) type=checkbox value=checkbox name=chkAll style="font-weight: 700"></td>
  </tr>
  <%

for i=1 to mypage.pagesize 
if not rs.eof then 

%>
  <tr bgcolor="#FFFFFF"> 
    <td height="22" align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><%=rs("id")%></td>
	<input name=id type=hidden value="<%=rs("id")%>" >
    <td style="padding-left: 2px; padding-right: 2px" class="style1"><%=left(rs("title"),30) %></td>
    <td style="padding-left: 2px; padding-right: 2px;text-align:center" class="style1"><%=rs("name")%></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><%=rs("tel")%></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><%=rs("sj")%></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><%
    if rs("zd")=1 then
    response.write "<a href=top.asp?action=qxtj&id="&rs("id")&"><font color=red>是</font></a>"
    else
    response.write "<a href=top.asp?action=tj&id="&rs("id")&">否</a>"
    end if
    %>
    </td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1">
    <%if rs("sh")=1 then
    response.write "<a href=sh.asp?action=qxtj&id="&rs("id")&">是</a>"
    else
    response.write "<a href=sh.asp?action=tj&id="&rs("id")&"><font color=red>否</font></a>"
    end if
    %>

    </td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1">
    <a href="lyhf.asp?id=<%=rs("id")%>">回复</a> <a href="delbook.asp?id=<%=rs("id")%>">删除</a> </td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><input type="checkbox" name="adid" value="<%=rs("id")%>" onClick=Checked(form)></td>
  </tr>
<%
 rs.movenext 
 else 
exit for 
end if
 next                                                    
%>

</table>
	
	</div>
	<div align="center">
	<table border=0 cellspacing=1 class=navi width="100%">
	<tr>
	<td align=left height=30 width="120">
		<p align="center">
		　</td>
	  <td align=center height=30><% mypage.showpage() %></td>
	</tr>
	</table>
	</div>
<% 
rs.close
set rs=nothing
conn.close
set conn=nothing
%>
           
            
            </td>
         <td >
            
 　</td>
          </tr>
          </table>
          </form>
          </td>
      </tr>
    </table></td>
    <td background="images/mail_rightbg.gif">　</td>
  </tr>
  <tr>
    <td valign="middle" background="images/mail_leftbg.gif"><img src="images/buttom_left2.gif" width="17" height="17" /></td>
      <td height="17" valign="top" background="images/buttom_bgs.gif"><img src="images/buttom_bgs.gif" width="17" height="17" /></td>
    <td background="images/mail_rightbg.gif"><img src="images/buttom_right2.gif" width="16" height="17" /></td>
  </tr>
</table>

</body>
</html>