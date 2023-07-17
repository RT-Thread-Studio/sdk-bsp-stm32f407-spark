<title>对不起，密码错误</title>
<!--#include file="conn.asp"-->
<%
username=Get_SafeStr(request.form("username"))
password=Get_SafeStr(request.form("password"))
code=Get_SafeStr(request.form("code"))
'msgbox session("CheckCode")
'msgbox code
if Code<>CStr(session("CheckCode")) then

session("checkcode")=""
' msgbox 1
response.write "<script language=JavaScript>" & "alert('验证码不正确');location.href='login.asp';" & "</script>" 
'msgbox 2
'response.flush
response.end
 'msgbox 3
end if
'response.end
'  msgbox 4
if username="" then
response.write "<script language=JavaScript>" & "alert('用户名不能为空');location.href='login.asp';"&"</script>" 
response.end
end if

if password="" then
response.write "<script language=JavaScript>" & "alert('密码不能为空');location.href='login.asp';"&"</script>" 
response.end
end if

sql="select * from admin where(username='"&username&"' and password='"&password&"')"
set rs=server.createobject("adodb.recordset")
rs.open sql,conn,1,1
if not rs.eof then 
Session("Admin")=rs("username") 
session("check")="checked"
response.Redirect "index.asp"
else
session("check")=""
session("admin")=""
response.write ("对不起，帐号或密码错误，请返回重新输入！")
end if
rs.close
set rs=nothing

%>
<p align="center"><a href="login.asp">
<span style="font-size: 9pt; text-decoration: none">返回重新登陆</span></a></p>