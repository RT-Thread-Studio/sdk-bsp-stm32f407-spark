<!--#include file="conn.asp"-->
<%
if not session("check")="checked" then
response.Redirect "login.asp"
end if

theid=request("adid")
del=request("del")
id=request.querystring("id")
if del="data" then
sql="delete from book where id in("&theid&")"
conn.execute(sql)
else
delnews="delete * from book where id="&id
conn.execute(delnews)
end if
rs.close
conn.close
set rs=nothing
set conn=nothing
response.redirect request.ServerVariables("HTTP_REFERER")
Response.End
%>
