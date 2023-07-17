<html>
<head>
<title>随缘网络企业网站管理系统 - 管理页面</title>
<script language=JavaScript>
function logout(){
	if (confirm("您确定要退出控制面板吗？"))
	top.location = "loginout.asp";
	return false;
}
</script>
<meta http-equiv=Content-Type content=text/html;charset=gb2312>
<meta http-equiv="refresh" content="60">
<base target="main">
<link href="images/skin.css" rel="stylesheet" type="text/css">
</head>
<body leftmargin="0" topmargin="0">
<table width="100%" height="64" border="0" cellpadding="0" cellspacing="0" class="admin_topbg">
  <tr>
    <td width="61%" height="64"><img src="images/logo.gif" width="262" height="64"></td>
    <td width="39%" valign="top"><table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="74%" height="38" class="admin_txt">管理员：<b><%=  Session("Admin")%></b> 您好,感谢登陆使用！</td>
        <td width="22%"><a href="#" target="_self" onClick="logout();"><img src="images/out.gif" alt="安全退出" width="46" height="20" border="0"></a></td>
        <td width="4%">&nbsp;</td>
      </tr>
      <tr>
        <td height="19" colspan="3">&nbsp;</td>
        </tr>
    </table></td>
  </tr>
</table>
</body>
</html>
