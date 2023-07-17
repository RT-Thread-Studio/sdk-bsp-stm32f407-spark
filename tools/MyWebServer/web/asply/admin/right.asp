
<head>
<link href="images/skin.css" rel="stylesheet" type="text/css" />
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" /><style type="text/css">
<!--
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #EEF2FB;
}
-->
</style>
<base target="_self">
</head>

<body style="background-color: #F7F8F9">
<table width="100%" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="17" valign="top" background="images/mail_leftbg.gif"><img src="images/left-top-right.gif" width="17" height="29" /></td>
    <td valign="top" background="images/content-bg.gif"><table width="100%" height="31" border="0" cellpadding="0" cellspacing="0" class="left_topbg" id="table2">
      <tr>
        <td height="31"><div class="titlebt">欢迎界面</div></td>
      </tr>
    </table></td>
    <td width="16" valign="top" background="images/mail_rightbg.gif"><img src="images/nav-right-bg.gif" width="16" height="29" /></td>
  </tr>
  <tr>
    <td valign="middle" background="images/mail_leftbg.gif">　</td>
    <td valign="top" bgcolor="#F7F8F9"><table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
      <tr>
        <td colspan="2" valign="top">　</td>
      </tr>
      <tr>
        <td colspan="2" valign="top"><span class="left_bt">感谢您使用 随缘网络科技 企业网站管理系统程序</span><br>
              <br>
            <span class="left_txt">&nbsp;<img src="images/ts.gif" width="16" height="16"> 提示：<br>
          您现在使用的是厦门随缘网络科技（<a href="http://www.116cn.cn">www.116cn.cn</a>）开发的一套用于构建企业网站的网站管理系统！
		厦门随缘网络科技主要提供网站建设、改版、虚拟主机、中英文域名注册等网络服务，如果您有相关的业务需求可与我们联系！</span></td>
      </tr>
      <tr>
        <td colspan="2">　</td>
      </tr>
      <tr>
        <td colspan="2" valign="top">
        
    <div align="center">
        
    <table border=0 cellspacing=0 class=list cellpadding="0" width="100%">
	<tr><th colspan=2 height="20">
		<p align="left">服务器的有关参数</th><th colspan=2 height="20">
		<p align="left">组件支持有关参数</th></tr>
	<tr>
		<td width="20%" height="20">服务器名：</td>
		<td width="29%" height="20"><%=Request.ServerVariables("SERVER_NAME")%></td>
		<td width="27%" height="20">ADO 数据对象：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("adodb.connection", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">服务器IP：</td>
		<td width="29%" height="20"><%=Request.ServerVariables("LOCAL_ADDR")%></td>
		<td width="27%" height="20">FSO 文本文件读写：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("scripting.filesystemobject", 0)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">服务器端口：</td>
		<td width="29%" height="20"><%=Request.ServerVariables("SERVER_PORT")%></td>
		<td width="27%" height="20">Stream 文件流：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Adodb."&"Stream", 0)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">服务器时间：</td>
		<td width="29%" height="20"><%=Now()%></td>
		<td width="27%" height="20">Microsoft.XMLHTTP：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Microsoft.XMLHTTP", 0)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">IIS版本：</td>
		<td width="29%" height="20"><%=Request.ServerVariables("SERVER_SOFTWARE")%></td>
		<td width="27%" height="20">Microsoft.XMLDOM：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Microsoft.XMLDOM", 0)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">服务器操作系统：</td>
		<td width="29%" height="20"><%=Request.ServerVariables("OS")%></td>
		<td width="27%" height="20">CDONTS 虚拟SMTP发信：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("CDONTS.NewMail", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">脚本超时时间：</td>
		<td width="29%" height="20"><%=Server.ScriptTimeout%> 秒</td>
		<td width="27%" height="20">LyfUpload 上传组件：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("LyfUpload.UploadFile", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">站点物理路径：</td>
		<td width="29%" height="20"><%=request.ServerVariables("APPL_PHYSICAL_PATH")%></td>
		<td width="27%" height="20">AspUpload 上传组件：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Persits.Upload.1", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">服务器CPU数量：</td>
		<td width="29%" height="20"><%=Request.ServerVariables("NUMBER_OF_PROCESSORS")%> 个</td>
		<td width="27%" height="20">SA-FileUp 上传组件：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("SoftArtisans.FileUp", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">服务器解译引擎：</td>
		<td width="29%" height="20"><%=ScriptEngine & "/" & ScriptEngineMajorVersion & "." & ScriptEngineMinorVersion & "." & ScriptEngineBuildVersion %></td>
		<td width="27%" height="20">AspJpeg 图像处理组件：</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Persits.Jpeg",1)%></td>
	</tr>
	</table>
    
	</div>
    
<br>
	<div align="center">
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="list">
      <tr class="hback">
        <th height="25" colspan="2" align="left" class="xingmu">使用本系统，请确认的服务器和你的浏览器满足以下要求：</th>
      </tr>
      <tr class="hback">
        <td width="48%" bgcolor="#F8F7F5" align="left">JRO.JetEngine(ACCESS&nbsp; 数据库<span class="small2">)：</span>
            <%
		On Error Resume Next
		Server.CreateObject("JRO.JetEngine")
          
		if err=0 then 
		  response.write("√")
		else
		  response.write("×")
		end if	 
		err=0
	 %>        </td>
        <td width="50%" bgcolor="#F8F7F5">数据库使用:
          <%
		On Error Resume Next
		Server.CreateObject("Adodb.Connection")
		if err=0 then 
		  response.write("√,可以使用本系统")
		else
		  response.write("×,不能使用本系统")
		end if	 
		err=0
	%>        </td>
      </tr>
      <tr class="hback">
        <td bgcolor="#F8F7F5" align="left" width="48%"><span class="small2">FSO</span>文本文件读写<span class="small2">：</span>
        <%=Get_ObjInfo("scripting.filesystemobject", 0)%>
    </td>
        <td height="20" bgcolor="#F8F7F5" width="50%"> Adodb.Stream: 
					<%Server.CreateObject("Adodb.Stream")
					if err=0 then 
					  response.write("√")
					else
					  response.write("×")
					end if	 
					err=0%>		  </td>
      </tr>
    </table>
	</div>
<%
Function Get_ObjInfo(obj, ver)
	On Error Resume Next
	Dim objTest, sTemp
	Set objTest = Server.CreateObject(obj)
	If Err.Number <> 0 Then
		Err.Clear
		Get_ObjInfo = "<font class=red><b>×</b></font>&nbsp;<font class=gray>不支持</font>"
	Else
		sTemp = ""
		If ver = 1 Then
			sTemp = objTest.version
			If IsNull(sTemp) Then sTemp = objTest.about
			sTemp = Replace(sTemp, "Version", "")
			sTemp = "&nbsp;<font class=tims><font class=blue>" & sTemp & "</font></font>"
		End If
		Get_ObjInfo = "<b>√</b>&nbsp;<font class=gray>支持</font>" & sTemp
	End If
	Set objTest = Nothing
	If Err.Number <> 0 Then Err.Clear
End Function
%>

</td>
      </tr>
      <tr>
        <td height="15" colspan="3"><table width="100%" height="1" border="0" cellpadding="0" cellspacing="0" bgcolor="#CCCCCC">
          <tr>
            <td></td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td width="1%">　</td>
        <td class="left_txt"><img src="images/icon-mail2.gif" width="16" height="11"> 
		<a href="mailto:联系邮箱：web@115cn.cn">联系邮箱：web@115cn.cn</a>&nbsp;&nbsp;&nbsp; 
		QQ咨询：84723090&nbsp; 545177708 <br>
              <img src="images/icon-phone.gif" width="17" height="14"> 官方网站：<a href="http://www.116cn.cn">http://www.116cn.cn</a>
		<a href="http://www.115cn.cn">http://www.115cn.cn</a> 
		电话：0592-5983163</td>
        <td>　</td>
      </tr>
    </table></td>
    <td background="images/mail_rightbg.gif">　</td>
  </tr>
  <tr>
    <td valign="bottom" background="images/mail_leftbg.gif"><img src="images/buttom_left2.gif" width="17" height="17" /></td>
    <td background="images/buttom_bgs.gif"><img src="images/buttom_bgs.gif" width="17" height="17"></td>
    <td valign="bottom" background="images/mail_rightbg.gif"><img src="images/buttom_right2.gif" width="16" height="17" /></td>
  </tr>
</table>
</body>
