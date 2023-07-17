<!--#include file="conn.asp"-->
<%
if not session("check")="checked" then
response.Redirect "login.asp"
response.end
end if
action=request.querystring("action")
id=request.querystring("id")
if action="qxtj" then
delnews="update book set zd=0 where id="&clng(id)
conn.execute(delnews)
elseif action="tj" then
delnews="update book set zd=1 where id="&clng(id)
conn.execute(delnews)
end if
rs.close
conn.close
set rs=nothing
set conn=nothing
Response.Write("<SCRIPT language=JavaScript>alert('提示：操作成功！');this.location.href='"&request.ServerVariables("HTTP_REFERER")&"';</SCRIPT>")
Response.End
%>
