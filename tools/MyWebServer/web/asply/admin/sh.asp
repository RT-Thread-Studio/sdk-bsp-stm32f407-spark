<!--#include file="conn.asp"-->
<%
if not session("check")="checked" then
response.Redirect "login.asp"
response.end
end if
action=request.querystring("action")
id=request.querystring("id")
theid=request("adid")
sh=request("sh")

if sh="data" then
delnews="update book set sh=1 where id in("&theid&")"
conn.execute(delnews)
end if

if action="qxtj" then
delnews="update book set sh=0 where id="&clng(id)
conn.execute(delnews)
elseif action="tj" then
delnews="update book set sh=1 where id="&clng(id)
conn.execute(delnews)
end if
rs.close
conn.close
set rs=nothing
set conn=nothing
Response.Write("<SCRIPT language=JavaScript>alert('提示：操作成功！');this.location.href='"&request.ServerVariables("HTTP_REFERER")&"';</SCRIPT>")
Response.End
%>
