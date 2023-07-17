<html>
<head>
<title>随缘网络网站管理中心</title>
<meta http-equiv=Content-Type content=text/html;charset=gb2312>
</head>
<%
if not session("check")="checked" then
response.Redirect "login.asp"
end if
%>
<frameset rows="64,*"  frameborder="NO" border="0" framespacing="0">
	<frame src="admin_top.asp" noresize="noresize" frameborder="NO" name="topFrame" scrolling="no" marginwidth="0" marginheight="0" target="main" />
  <frameset  rows="560,*" id="frame">
	<frameset cols="200,*" id="frame">
	<frame src="left.html" name="leftFrame" noresize="noresize" marginwidth="0" marginheight="0" frameborder="0" scrolling="auto" target="main" />
	<frame src="right.asp" name="main" marginwidth="0" marginheight="0" frameborder="0" scrolling="auto" target="_self" />
  	</frameset>
	<frameset id="frame">
	</frameset>
  </frameset>
<noframes>
  <body></body>
    </noframes>
</html>
