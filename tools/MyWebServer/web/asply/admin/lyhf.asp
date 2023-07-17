<!--#include file="conn.asp"-->
<%
if not session("check")="checked" then
response.Redirect "login.asp"
end if

if request("no")="modi" then
newsid=request("newsid")
title=request("title")
name=request.form("name")
content=request.form("content")
hf=request.form("hf")
tel=request.form("tel")
sh=request.form("sh")
zd=request.form("zd")
if sh="" then sh=0
if zd="" then zd=0
' msgbox newsid
set rs=server.createobject("adodb.recordset")
sql="select * from book where id=" & newsid
rs.open sql,conn,1,3
rs("title")=title
rs("hf")=hf
rs("name")=name
rs("tel")=tel
rs("content")=content	
rs("sh")=sh
rs("zd")=zd
'msgbox err
rs.update
 'msgbox content
rs.close
set rs=nothing
conn.close
set conn=nothing
response.write "<script language='javascript'>" & chr(13)
		response.write "alert('操作成功！');" & Chr(13)
        response.write "window.document.location.href='book.asp';"&Chr(13)
		response.write "</script>" & Chr(13)
Response.End
end if

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
-->
</style>
<link href="images/skin.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../js/jquery-1.4.2.js"></script>
<script type="text/javascript" src="../js/xheditor.js"></script>
<script type="text/javascript" language="javascript">
$(pageInit);
function pageInit()
{
$('#elm1').xheditor({skin:'vista',forcePtag:false,upMultiple:1,upLinkUrl:'../up2.asp',upImgUrl:'../up2.asp',upFlashUrl:'../up2.asp',upMediaUrl:'../up2.asp'});
$('#elm2').xheditor({skin:'vista',forcePtag:false,upMultiple:1,upLinkUrl:'../up2.asp',upImgUrl:'../up2.asp',upFlashUrl:'../up2.asp',upMediaUrl:'../up2.asp'});
}
</script>
</head>

<body>
<table width="100%" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="17" valign="top" background="images/mail_leftbg.gif"><img src="images/left-top-right.gif" width="17" height="29" /></td>
    <td valign="top" background="images/content-bg.gif"><table width="100%" height="31" border="0" cellpadding="0" cellspacing="0" class="left_topbg" id="table2">
      <tr>
        <td height="31"><div class="titlebt">留言回复</div></td>
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
            <td class="left_txt">当前位置：留言管理 - 留言回复</td>
          </tr>
          <tr>
            <td >
<% 
newsid=request("id")
Set rso=Server.CreateObject("ADODB.RecordSet") 
sql="select * from book where  id="&newsid
rso.Open sql,conn,1,1
if rso.eof and rso.bof then 
  response.write"<SCRIPT language=JavaScript>alert('暂无该留言信息！');"
  response.write"javascript:history.go(-1)</SCRIPT>"
  response.end
end if
title=rso("title")
tel=rso("tel")
content=rso("content")
ip=rso("ip")
tj=rso("tj")
sj=rso("sj")
hf=rso("hf")
name=rso("name")
sh=rso("sh")

zd=rso("zd")
rso.close
set rso=nothing
%>            
<div align="center">           
<table width="100%" border="1" cellpadding="0" class="list" style="border-collapse: collapse" bordercolor="#CCCCCC">
  <form name="addNEWS" id="AddPro" method="post" action="?no=modi"  >
    <tr bgcolor="#FFFFFF">
      <th height="25" width="99%" colspan="2" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#EEF2FB"><div align="center" class="style1">
		留言回复</div>
		</th>
    </tr>
    <tr bgcolor="#FFFFFF"> 
      <td height="25" width="17%" align="right" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
        留言主题：</td>
      <td height="25" width="162%" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
        <input name="title" type="text" class="input" value="<%=title%>" size="50"> </td>
    </tr>
    <tr>
      <td height="25" width="14%" align="right" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
      留言姓名：</td>
      <td height="25"  width="85%" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9"><input name="name" type="text" class="input" value="<%=name%>" size="30"></td>
      </tr>
    <tr bgcolor="#FFFFFF"> 
      <td height="25" width="17%" align="right" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
      联系电话：</td>
      <td height="25" valign="top" width="162%" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
        <input name="tel" type="text" class="input" value="<%=tel%>" size="30"></td>
    </tr>
    <tr bgcolor="#FFFFFF"> 
      <td height="25" width="17%" align="right" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
      留言时间：</td>
      <td height="25" valign="top" width="162%" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
        <input name="sj" disabled type="text" class="input" value="<%=sj%>" size="30"></td>
    </tr>
    <tr bgcolor="#FFFFFF"> 
      <td height="25" width="17%" align="right" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
      留言者IP：</td>
      <td height="25"  valign="top" width="162%" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
        <input name="IP" disabled type="text" class="input" value="<%=IP%>" size="30">
		<a target="_blank" href="ip.asp?ip=<%=ip%>">点击查看IP来源</a></td>
    </tr>
    <tr bgcolor="#FFFFFF"> 
      <td height="25" width="17%" align="right" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
      留言设置：</td>
      <td height="25" valign="top" width="162%" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
<input type="checkbox" name="sh" value="1" <%if sh=1 then response.write  "checked" %>> 审核通过&nbsp; 
<input type="checkbox" name="zd" value="1" <%if zd=1 then response.write  "checked" %>> 留言置顶</td>
    </tr>
    <tr bgcolor="#FFFFFF"> 
      <td height="25" width="17%" align="right" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
      留言内容：</td>
      <td height="25" valign="top" width="162%" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9">
<textarea  name="content" rows="15" cols="40" id="elm1"  style="width:100%"><%=content%></textarea>
</td>
    </tr>
    <tr>
      <td height="31" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9" > 
        <p align="right">留言回复：</td>
      <td height="31" style="padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px" bgcolor="#F7F8F9" > 
        <p align="left">
<textarea  name="hf" rows="15" cols="40" id="elm2" style="width:100%"><%=hf%></textarea> </td>
    </tr>
    <tr bgcolor="#FFFFFF"> 
      <td height="30" align="center" colspan="2" bgcolor="#F7F8F9"> 
        <input type="submit" name="Submit" value="提交" class="input">
        　  <input type="hidden" name="newsId" value="<%=newsId%>">
        <input type="reset" name="Submit2" value="重置" class="input"> 
</td>
    </tr>
  </form> 
</table>
                       </div>
           
            
            </td>
          </tr>
          </table>
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