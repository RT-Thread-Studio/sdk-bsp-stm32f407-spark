<%@ Language="VBScript" %>
<%
' *************************************************

'  阿江ASP探针 V1.93 20060602
'  阿江守候 http://www.ajiang.net

' *************************************************

' 不使用输出缓冲区，直接将运行结果显示在客户端
'ON ERROR RESUME NEXT
server.scripttimeout=-1
Response.Buffer =true 'false

' 网页立即超时，防止缓存导致测速失败。
Response.Expires = -1
response.write Request("style")':response.end
' 将检测的组件的列表
Dim OtT(3,15,1)
' 服务器变量
dim okCPUS, okCPU, okOS
' 检测组件变量
dim isobj,VerObj,TestObj

T = Request("T")
if T="" then T="ABGH"
%>

<HTML>
<HEAD>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<TITLE>ASP探针V1.93－阿江http://www.ajiang.net</TITLE>
<style>
<!--
h1 {font-size:14px;color:#3F8805;font-family:Arial;margin:15px 0px 5px 0px}
h2 {font-size:12px;color:#000000;margin:15px 0px 8px 0px}
h3 {font-size:12px;color:#3F8805;font-family:Arial;margin:7px 0px 3px 12px;font-weight: normal;}
BODY,TD{FONT-FAMILY: 宋体;FONT-SIZE: 12px;word-break:break-all}
tr{BACKGROUND-COLOR: #EEFEE0}
A{COLOR: #3F8805;TEXT-DECORATION: none}
A:hover{COLOR: #000000;TEXT-DECORATION: underline}
A.a1{COLOR: #000000;TEXT-DECORATION: none}
A.a1:hover{COLOR: #3F8805;TEXT-DECORATION: underline}
table{BORDER: #3F8805 1px solid;background-color:#3F8805;margin-left:12px}
p{margin:5px 12px;color:#000000}
.input{BORDER: #111111 1px solid;FONT-SIZE: 9pt;BACKGROUND-color: #F8FFF0}
.backs{BACKGROUND-COLOR: #3F8805;COLOR: #ffffff;text-align:center}
.backq{BACKGROUND-COLOR: #EEFEE0}
.backc{BACKGROUND-COLOR: #3F8805;BORDER: medium none;COLOR: #ffffff;HEIGHT: 18px;font-size: 9pt}
.fonts{	COLOR: #3F8805}
-->
</STYLE>
</HEAD>
<body>



<h1><a href="http://www.ajiang.net/">阿江</a><a href="http://www.ajiang.net/aspcheck.asp">ASP 探针</a> V 1.93 - 20060602</h1>
<%
call mmenu()
for qq=1 to len(T)
  call BodyGo(mid(T,qq,1))
next
call mmenu()
%>
<br>
<br>
<table border=0 width=512 cellspacing=1 cellpadding=3 style="margin-left:0px;border:none;background:none">
  <tr style="background:none" align="center"><td>
  <hr width="512" size="1">
  阿江守候 (www.ajiang.net) 版权所有 &copy; 2001-2005
  <br>
  <a href="http://www.ajiang.net/">阿江守候</a>
  | <a href="http://www.ajstat.com/">阿江统计</a>
  | <a href="http://www.51.la/">我要啦免费统计</a>
  | <a href="http://www.ajiang.net/products/aspcheck/">阿江探针</a>
  | <a href="http://www.ajiang.net/products/aspcheck/">下载最新版</a>
  <hr width="512" size="1">
  </td></tr>
</table>  
</body>
</html>

<%












' *******************************************************************************
' 　　[ A ] 是否支持ASP
' *******************************************************************************
sub aspyes()
%>
<h2>是否支持ASP</h2>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr><td>
    出现以下情况即表示您的空间不支持ASP：
    <br>1、访问本文件时提示下载。
    <br>2、访问本文件时看到类似“&lt;&#x25;&#x40;&#x20;&#x4C;&#x61;&#x6E;&#x67;&#x75;&#x61;&#x67;&#x65;&#x3D;&#x22;&#x56;&#x42;&#x53;&#x63;&#x72;&#x69;&#x70;&#x74;&#x22;&#x20;&#x25;&gt;”的文字。
    </td></tr>
  </table>
<%
end sub






' *******************************************************************************
' 　　[ B ] 服务器概况
' *******************************************************************************
sub servinfo()
on error resume next
%>
  <h2>服务器概况</h2>
	<table border=0 width=500 cellspacing=1 cellpadding=3>
	  <tr>
		<td width=250>服务器地址</td><td width=350>名称 <%=Request.ServerVariables("SERVER_NAME")%>(IP:<%=Request.ServerVariables("LOCAL_ADDR")%>) 端口:<%=Request.ServerVariables("SERVER_PORT")%></td>
	  </tr>
	  <%
	  tnow = now():oknow = cstr(tnow)
	  if oknow <> year(tnow) & "-" & month(tnow) & "-" & day(tnow) & " " & hour(tnow) & ":" & right(FormatNumber(minute(tnow)/100,2),2) & ":" & right(FormatNumber(second(tnow)/100,2),2) then oknow = oknow & " (日期格式不规范)"
	  %>
	  <tr>
		<td>服务器时间</td><td><%=oknow%></td>
	  </tr>
	  <tr>
		<td>IIS版本</td><td><%=Request.ServerVariables("SERVER_SOFTWARE")%></td>
	  </tr>
	  <tr>
		<td>脚本超时时间</td><td><%=Server.ScriptTimeout%> 秒</td>
	  </tr>
	  <tr>
		<td>本文件路径</td><td><%=Request.ServerVariables("PATH_TRANSLATED")%></td>
	  </tr>
	  <tr>
		<td>服务器脚本引擎</td><td><%=ScriptEngine & "/"& ScriptEngineMajorVersion &"."&ScriptEngineMinorVersion&"."& ScriptEngineBuildVersion %> ,<%="JScript/"   %></td>
	  </tr>
	  <%getsysinfo()  '获得服务器数据%>
	  <tr>
		<td>服务器操作系统</td><td><%=okOS%></td>
	  </tr>
	  <tr>
		<td>全局和会话变量</td><td>Application 变量 <%=Application.Contents.count%> 个<% if Application.Contents.count>0 then Response.Write "[<a href=""?T=C"">列表</a>]"%>, 
		Session 变量 <%=Session.Contents.count%> 个  <%if Session.Contents.count>0 then Response.Write "[<a href=""?T=D"">列表</a>]"%></td>
	  </tr>
	  <tr>
		<td>ServerVariables</td><td><%=Request.ServerVariables.Count%> 个  <%if Request.ServerVariables.Count>0 then Response.Write "[<a href=""?T=E"">Request.ServerVariables 列表</a>]"%></td>
	  </tr>
	  <tr>
		<td>服务器CPU通道数</td><td><%=okCPUS%> 个</td>
	  </tr>
	  <%

	  call ObjTest("WScript.Shell")
	  if isobj then
	    set WSshell=server.CreateObject("WScript.Shell")
	  %>
	  <tr>
		<td>服务器CPU详情</td><td><%=okCPU%></td>
	  </tr>
	  <tr>
		<td>全部服务器环境</td><td><%=WSshell.Environment.count%> 个  <%if WSshell.Environment.count>0 then Response.Write "[<a href=""?T=F"">WSshell.Environment 列表</a>]"%></td>
	  </tr>
	  <%
	  end if
	  %>
	</table>
<%
end sub

%>
<SCRIPT language="JScript" runat="server">
function getJVer(){
  //获取JScript 版本
  return ScriptEngineMajorVersion() +"."+ScriptEngineMinorVersion()+"."+ ScriptEngineBuildVersion();
}
</SCRIPT>
<%

' 获取服务器常用参数
sub getsysinfo()
  on error resume next
  Set WshShell = server.CreateObject("WScript.Shell")
  Set WshSysEnv = WshShell.Environment("SYSTEM")
  okOS = cstr(WshSysEnv("OS"))
  okCPUS = cstr(WshSysEnv("NUMBER_OF_PROCESSORS"))
  okCPU = cstr(WshSysEnv("PROCESSOR_IDENTIFIER"))
  if isempty(okCPUS) then
    okCPUS = Request.ServerVariables("NUMBER_OF_PROCESSORS")
  end if
  if okCPUS & "" = "" then
    okCPUS = "(未知)"
  end if
  if okOS & "" = "" then
    okOS = "(未知)"
  end if
end sub






' *******************************************************************************
' 　　[ C ] Application 变量列表
' *******************************************************************************
sub applist()
%>
<h2>Application 变量列表</h2>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="110">变 量 名 称</td><td width="390">值</td></tr>
  <%for each apps in Application.Contents%>
  <tr><td width="110"><%=apps%></td><td width="390"><%
  if isobject(Application.Contents(apps)) then
    Response.Write "[对象]"
  elseif isarray(Application.Contents(apps)) then
    Response.Write "[数组]"
  else
    Response.Write cHtml(Application.Contents(apps))
  end if
  %></td></tr><%next%>
</table>
<%
end sub






' *******************************************************************************
' 　　[ D ] Session 变量列表
' *******************************************************************************
sub seslist()
%>
<h2>Session 变量列表</h2>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="110">变 量 名 称</td><td width="390">值</td></tr>
  <%for each sens in Session.Contents%>
  <tr><td width="110"><%=sens%></td><td width="390"><%
  if isobject(Session.Contents(sens)) then
    Response.Write "[对象]"
  elseif isarray(Session.Contents(sens)) then
    Response.Write "[数组]"
  else
    Response.Write cHtml(Session.Contents(sens))
  end if
  %></td></tr><%next%>
</table>
<%
end sub






' *******************************************************************************
' 　　[ E ] Request.ServerVariables 变量列表
' *******************************************************************************
sub sevalist()
%>
<h2>Request.ServerVariables 变量列表(含客户端信息)</h2>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="110">变 量 名 称</td><td width="390">值</td></tr>
  <%for each apps in Request.ServerVariables%>
  <tr><td width="110"><%=apps%></td><td width="390"><%=cHtml(Request.ServerVariables(apps))%></td></tr><%next%>
</table>
<%
end sub






' *******************************************************************************
' 　　[ F ] Request.ServerVariables 变量列表
' *******************************************************************************
sub wsslist()
  on error resume next
  Set WSshell = server.CreateObject("WScript.Shell")
%>
<h2>WScript.Shell.Environments 变量列表</h2>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="110">变 量 名 称</td><td width="390">值</td></tr>
  <%for each envs in WSshell.Environment
  envsa = split(envs,"=")
  %>
  <tr><td width="110"><%=envsa(0)%></td><td width="390"><%=cHtml(envsa(1))%></td></tr><%next%>
</table>
<%
end sub






' *******************************************************************************
' 　　[ G ] 组件检测
' *******************************************************************************
sub comlist()
  on error resume next
  OtT(0,0,0) = "MSWC.AdRotator"
  OtT(0,1,0) = "MSWC.BrowserType"
  OtT(0,2,0) = "MSWC.NextLink"
  OtT(0,3,0) = "MSWC.Tools"
  OtT(0,4,0) = "MSWC.Status"
  OtT(0,5,0) = "MSWC.Counters"
  OtT(0,6,0) = "IISSample.ContentRotator"
  OtT(0,7,0) = "IISSample.PageCounter"
  OtT(0,8,0) = "MSWC.PermissionChecker"
  OtT(0,9,0) = "Microsoft.XMLHTTP"
	OtT(0,9,1) = "(Http 组件, 常在采集系统中用到)"
  OtT(0,10,0) = "WScript.Shell"
	OtT(0,10,1) = "(Shell 组件, 可能涉及安全问题)"
  OtT(0,11,0) = "Scripting.FileSystemObject"
	OtT(0,11,1) = "(FSO 文件系统管理、文本文件读写)"
  OtT(0,12,0) = "Adodb.Connection"
	OtT(0,12,1) = "(ADO 数据对象)"
  OtT(0,13,0) = "Adodb.Stream"
	OtT(0,13,1) = "(ADO 数据流对象, 常见被用在无组件上传程序中)"
	
  OtT(1,0,0) = "SoftArtisans.FileUp"
	OtT(1,0,1) = "(SA-FileUp 文件上传)"
  OtT(1,1,0) = "SoftArtisans.FileManager"
	OtT(1,1,1) = "(SoftArtisans 文件管理)"
  OtT(1,2,0) = "Ironsoft.UpLoad"
	OtT(1,2,1) = "(国产免费, 上传组件)"
  OtT(1,3,0) = "LyfUpload.UploadFile"
	OtT(1,3,1) = "(刘云峰的文件上传组件)"
  OtT(1,4,0) = "Persits.Upload.1"
	OtT(1,4,1) = "(ASPUpload 文件上传)"
  OtT(1,5,0) = "w3.upload"
	OtT(1,5,1) = "(Dimac 文件上传)"

  OtT(2,0,0) = "JMail.SmtpMail"
	OtT(2,0,1) = "(Dimac JMail 邮件收发) <a href='http://www.ajiang.net/products/aspcheck/coms.asp'>中文手册下载</a>"
  OtT(2,1,0) = "CDONTS.NewMail"
	OtT(2,1,1) = "(CDONTS)"
  OtT(2,2,0) = "CDO.Message"
	OtT(2,2,1) = "(CDOSYS)"
  OtT(2,3,0) = "Persits.MailSender"
	OtT(2,3,1) = "(ASPemail 发信)"
  OtT(2,4,0) = "SMTPsvg.Mailer"
	OtT(2,4,1) = "(ASPmail 发信)"
  OtT(2,5,0) = "DkQmail.Qmail"
	OtT(2,5,1) = "(dkQmail 发信)"
  OtT(2,6,0) = "SmtpMail.SmtpMail.1"
	OtT(2,6,1) = "(SmtpMail 发信)"
	
  OtT(3,0,0) = "SoftArtisans.ImageGen"
	OtT(3,0,1) = "(SA 的图像读写组件)"
  OtT(3,1,0) = "W3Image.Image"
	OtT(3,1,1) = "(Dimac 的图像读写组件)"
  OtT(3,2,0) = "Persits.Jpeg"
	OtT(3,2,1) = "(ASPJpeg)"
  OtT(3,3,0) = "XY.Graphics"
	OtT(3,3,1) = "(国产免费, 图像/图表处理)"
  OtT(3,4,0) = "Ironsoft.DrawPic"
	OtT(3,4,1) = "(国产免费, 图像/图形处理)"
  OtT(3,5,0) = "Ironsoft.FlashCapture"
	OtT(3,5,1) = "(国产免费, 多功能 FLASH 截图)"
  OtT(3,6,0) = "dyy.zipsvr"
	OtT(3,6,1) = "(国产免费, 呆呆文件压缩解压组件)"
  OtT(3,7,0) = "hin2.com_iis"
	OtT(3,7,1) = "(国产免费, 呆呆IIS管理组件)"
  OtT(3,8,0) = "Socket.TCP"
	OtT(3,8,1) = "(Dimac 公司的 Socket 组件)"
	
%>
<h2>ASP组件支持情况</h2><a name="G"></a>

<h3>■ 检查组件是否被支持</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <FORM action="?T=<%=T%>#G" method="post">
  <tr><td align="center" style="padding:10px 0px">
  在下面的文本框中输入您要检测的组件的 ProgId 或 ClassId
  <input class=input type=text value="" name="classname" size=50>
  <input type=submit value=" 检 查 " class=backc id=submit1 name=submit1>
<%
Dim strClass
strClass = Trim(Request.Form("classname"))
If "" <> strClass then
Response.Write "<p style=""margin:9px 0px 0px 0px"">"
Dim Verobj1
ObjTest(strClass)
  If Not IsObj then 
	Response.Write "<font color=red>很遗憾，该服务器不支持 " & strclass & " 组件！</font>"
  Else
	if VerObj="" or isnull(VerObj) then 
	  Verobj1="无法取得该组件版本"
	Else
	  Verobj1="该组件版本是：" & VerObj
	End If
	Response.Write "<font class=fonts>恭喜！该服务器支持 " & strclass & " 组件。" & verobj1 & "</font>"
  End If
end if
%>
  </td></tr>
  </FORM>
</table>

<h3>■ 操作系统自带的组件</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="380">组件名称及简介</td><td width="120">支持/版本</td></tr>
  <%
  k=0
  for i=0 to 13
    call ObjTest(OtT(k,i,0))
  %>
  <tr><td width="380"><%=OtT(k,i,0) & " <font color='#888888'>" & OtT(k,i,1) & "</font>"%></td><td width="120" title="<%=VerObj%>"><%=cIsReady(isobj) & " " & left(VerObj,10)%></td></tr>
  <%next%>
</table>

<h3>■ 常见文件上传和管理组件</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="380">组件名称及简介</td><td width="120">支持/版本</td></tr>
  <%
  k=1
  for i=0 to 5
    call ObjTest(OtT(k,i,0))
  %>
  <tr><td width="380"><%=OtT(k,i,0) & " <font color='#888888'>" & OtT(k,i,1) & "</font>"%></td><td width="120" title="<%=VerObj%>"><%=cIsReady(isobj) & " " & left(VerObj,10)%></td></tr>
  <%next%>
</table>

<h3>■ 常见邮件处理组件</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="380">组件名称及简介</td><td width="120">支持/版本</td></tr>
  <%
  k=2
  for i=0 to 6
    call ObjTest(OtT(k,i,0))
  %>
  <tr><td width="380"><%=OtT(k,i,0) & " <font color='#888888'>" & OtT(k,i,1) & "</font>"%></td><td width="120" title="<%=VerObj%>"><%=cIsReady(isobj) & " " & left(VerObj,10)%></td></tr>
  <%next%>
</table>

<h3>■ 其它常见组件</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="380">组件名称及简介</td><td width="120">支持/版本</td></tr>
  <%
  k=3
  for i=0 to 8
    call ObjTest(OtT(k,i,0))
  %>
  <tr><td width="380"><%=OtT(k,i,0) & " <font color='#888888'>" & OtT(k,i,1) & "</font>"%></td><td width="120" title="<%=VerObj%>"><%=cIsReady(isobj) & " " & left(VerObj,10)%></td></tr>
  <%next%>
</table>

<p>[<a href="http://www.ajiang.net/products/aspcheck/coms.asp">查看上述组件的详细介绍和下载地址</a>]
<%
	
end sub






' *******************************************************************************
' 　　[ H ] 磁盘信息
' *******************************************************************************
sub disklist()
  on error resume next

  ObjTest("Scripting.FileSystemObject")
  if isobj then
	set fsoobj=server.CreateObject("Scripting.FileSystemObject")

%>

<h2>磁盘和文件夹</h2>

<h3>■ 服务器磁盘信息</h3>

<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr align=center class="backs">
	<td width="100">盘符和磁盘类型</td>
	<td width="50">就绪</td>
	<td width="110">卷标</td>
	<td width="80">文件系统</td>
	<td width="80">可用空间</td>
	<td width="80">总空间</td>
  </tr>
<%

	' 测试磁盘信息的想法来自“COCOON ASP 探针”
	
	set drvObj=fsoobj.Drives
	for each d in drvObj
%>
  <tr align="center" class="backq">
	<td align="right"><%=cdrivetype(d.DriveType) & " " & d.DriveLetter%>:</td>
<%
	if d.DriveLetter = "A" then	'为防止影响服务器，不检查软驱
		Response.Write "<td></td><td></td><td></td><td></td><td></td>"
	else
%>
	<td><%=cIsReady(d.isReady)%></td>
	<td><%=d.VolumeName%></td>
	<td><%=d.FileSystem%></td>
	<td align="right"><%=cSize(d.FreeSpace)%></td>
	<td align="right"><%=cSize(d.TotalSize)%></td>
<%
	end if
%>
  </tr>
<%
	next
%>
</td></tr>
</table>
<p>“<font color=red><b>×</b></font>”表示磁盘没有就绪或者当前IIS站点没有对该磁盘的操作权限。

<h3>■ 当前文件夹信息</h3>
<%

Response.Flush


	dPath = server.MapPath("./")
	set dDir = fsoObj.GetFolder(dPath)
	set dDrive = fsoObj.GetDrive(dDir.Drive)
%>
<p>文件夹: <%=dPath%></p>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr height="18" align="center" class="backs">
	<td width="75">已用空间</td>
	<td width="75">可用空间</td>
	<td width="75">文件夹数</td>
	<td width="75">文件数</td>
	<td width="200">创建时间</td>
  </tr>
  <tr height="18" align="center" class="backq">
	<td><%=cSize(dDir.Size)%></td>
	<td><%=cSize(dDrive.AvailableSpace)%></td>
	<td><%=dDir.SubFolders.Count%></td>
	<td><%=dDir.Files.Count%></td>
	<td><%=dDir.DateCreated%></td>
  </tr>
</td></tr>
</table>

<%
Response.Flush

end if
end sub





' *******************************************************************************
' 　　[ I ] 磁盘速度
' *******************************************************************************
sub diskspeed()
  on error resume next

  %>
  <h2>磁盘文件操作速度测试</h2>
  <%
  ObjTest("Scripting.FileSystemObject")
  if isobj then
	set fsoobj=server.CreateObject("Scripting.FileSystemObject")
	' 测试文件读写的想法来自“迷城浪子”
	
	Response.Write "<p>正在重复创建、写入和删除文本文件50次..."

	dim thetime3,tempfile,iserr

    iserr=false
	t1=timer
	tempfile=server.MapPath("./") & "\aspchecktest.txt"
	for i=1 to 50
		Err.Clear

		set tempfileOBJ = FsoObj.CreateTextFile(tempfile,true)
		if Err <> 0 then
			Response.Write "创建文件错误！<br><br>"
			iserr=true
			Err.Clear
			exit for
		end if
		tempfileOBJ.WriteLine "Only for test. Ajiang ASPcheck"
		if Err <> 0 then
			Response.Write "写入文件错误！<br><br>"
			iserr=true
			Err.Clear
			exit for
		end if
		tempfileOBJ.close
		Set tempfileOBJ = FsoObj.GetFile(tempfile)
		tempfileOBJ.Delete 
		if Err <> 0 then
			Response.Write "删除文件错误！<br><br>"
			iserr=true
			Err.Clear
			exit for
		end if
		set tempfileOBJ=nothing
	next
	t2=timer
    if iserr <> true then
	thetime3=cstr(int(( (t2-t1)*10000 )+0.5)/10)
	Response.Write "...已完成！<font color=red>" & thetime3 & "毫秒</font>。<br>"
	Response.Flush

%>
</p>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr align=center class="backs">
	<td width=350>供 对 照 的 服 务 器</td>
	<td width=150>完成时间(毫秒)</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.zitian.cn/">紫田网络梦幻II型虚拟主机</a></td><td>&nbsp;31～78</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.gdxf.net/wzkj/index.htm">新丰信息港付费ASP+CGI空间</a></td><td>&nbsp;46～62</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.68l.com/">68互联</a></td><td>&nbsp;78</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.diy5.com">第5空间diy5.com四至强主机<font color=#888888>(P42.4,2GddrEcc,SCSI72.8G)</font></a></td><td>&nbsp;46～78</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.100u.com/?come=aspcheck&keyword=虚拟主机">百优科技 100u 主机</a></td><td>&nbsp;31～62</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.3366.com.cn">点击网络主机</a></td><td>&nbsp;31～62</td>
  </tr>
  <tr>
	<td><font color=red>这台服务器: <%=Request.ServerVariables("SERVER_NAME")%></font>&nbsp;</td><td>&nbsp;<font color=red><%=thetime3%></font></td>
  </tr>
</table>
<p>[<a href="http://www.ajiang.net/products/aspcheck/serverlist.asp" target="_blank">更多空间商的即时实测数据</a>]</p>
<%
end if

Response.Flush
	
	set fsoobj=nothing

end if
end sub






' *******************************************************************************
' 　　[ J ] 脚本运算速度
' *******************************************************************************
sub tspeed()
%>
<h2>ASP脚本解释和运算速度测试</h2><p>
<%
Response.Flush

	'感谢网际同学录 http://www.5719.net 推荐使用timer函数
	'因为只进行50万次计算，所以去掉了是否检测的选项而直接检测
	
	Response.Write "整数运算测试，正在进行50万次加法运算..."
	dim t1,t2,lsabc,thetime,thetime2
	t1=timer
	for i=1 to 500000
		lsabc= 1 + 1
	next
	t2=timer
	thetime=cstr(int(( (t2-t1)*10000 )+0.5)/10)
	Response.Write "...已完成！<font color=red>" & thetime & "毫秒</font>。<br>"


	Response.Write "浮点运算测试，正在进行20万次开方运算..."
	t1=timer
	for i=1 to 200000
		lsabc= 2^0.5
	next
	t2=timer
	thetime2=cstr(int(( (t2-t1)*10000 )+0.5)/10)
	Response.Write "...已完成！<font color=red>" & thetime2 & "毫秒</font>。<br>"
%></p>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr align=center class="backs">
	<td width=350>供对照的服务器及完成时间(毫秒)</td>
    <td width=75>整数运算</td><td width=75>浮点运算</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.100u.com?come=aspcheck&keyword=虚拟主机"
	>百优科技 100u 主机, <font color=#888888>2003-11-1</font></a></td><td>&nbsp;181～233</td><td>&nbsp;156～218</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.west263.net/index.asp?ads=ajiang"
	>西部数码 west263 主机, <font color=#888888>2003-11-1</font></a></td><td>&nbsp;171～233</td><td>&nbsp;156～171</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.163n.com "
	>数码城市 163n 主机,  <font color=#888888>2006-1-4</font></a></td><td>&nbsp;156～171</td><td>&nbsp;140～156</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.9s5.com/"
	>就是我www.9s5.com全功能(ASP+PHP+JSP)主机,<font color=#888888>2003-11-1</font></a></td><td>&nbsp;171～187</td><td>&nbsp;156～171</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.dnsmy.com/"
	>永讯网络 Dnsmy 主机, <font color=#888888>2003-11-1</font></a></td><td>&nbsp;155～180</td><td>&nbsp;122～172</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.senye.com"
	>胜易网 senye.com 主机, <font color=#888888>2004-3-28</font></a></td><td>&nbsp;156～171</td><td>&nbsp;140～156</td>
  </tr>
  <tr>
	<td><font color=red>这台服务器: <%=Request.ServerVariables("SERVER_NAME")%></font>&nbsp;</td><td>&nbsp;<font color=red><%=thetime%></font></td><td>&nbsp;<font color=red><%=thetime2%></font></td>
  </tr>
</table>
<p>[<a href="http://www.ajiang.net/products/aspcheck/serverlist.asp" target="_blank">更多空间商的即时实测数据</a>]</p>
<%
end sub






' *******************************************************************************
' 　　[ K ] 网络连接速度测试
' *******************************************************************************
sub tnet()
%>
<h2>连接带宽测试</h2><a name="K"></a>
<%
 if T<>"K" then
%>
<p>[<a href="?T=K">开始测试</a>]</p>
<%
 else
  haveok=false

  if Request("ok") <> "" then haveok=true
  if Request("tm") = "" then haveok=false

  if haveok=false then
%>
<p>正在测试您与当前服务器之间的连接速度，请稍等...<span id="baifen">.</span></p>
<script language="javascript" type="text/javascript">
var acd1;
acd1 = new Date();
acd1ok=acd1.getTime();
</script>
<%
Response.Flush
for i=1 to 1000
  Response.Write "<!--567890#########0#########0#########0#########0#########0#########0#########0#########012345-->" & vbcrlf
  if i mod 100=0 then
%>
<script language="javascript" type="text/javascript">
document.getElementById('baifen').innerHTML = '<%=i/10%>%';
</script>
<%
  end if
next
%>
<script language="javascript" type="text/javascript">
var acd2;
acd2 = new Date();
acd2ok=acd2.getTime();
window.location = '?T=K&ok=ok&tm=' + (acd2ok-acd1ok)
</script>
<%
Response.Flush :Response.end

  else

ttime=clng(Request("tm")) + 1

tnetspeed=100000/(ttime)
tnetspeed2=tnetspeed * 8
twidth=int(tnetspeed * 0.16)+5

if twidth> 300 then twidth=300
tnetspeed=formatnumber(tnetspeed,2,,,0)
tnetspeed2=formatnumber(tnetspeed2,2,,,0)

%><p>测试完成，向客户端传送 100k 字节数据共使用时间 <%=formatnumber(ttime,2)%> 毫秒。[<a href="?T=K">重测</a>]
</p>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr><td align="center" style="padding:10px 0px">
  <table style="margin:0px;border:none" align="center" width="400" border="0" cellspacing=0 cellpadding=0>
    <tr><td width="45">| 56k猫</td><td width="160">| 2M ADSL</td><td width=200>| 10M LAN</td></tr>
  </table>
  <table style="margin:0px" class="input" align="center" width="400" border="0" cellspacing=0 cellpadding=0>
    <tr class="input"><td width="<%=twidth%>" class="backs"></td><td width="<%=400-twidth%>">&nbsp;<%=tnetspeed%> kB/s</td></tr>
  </table>
  <p style="margin:10px 0px 0px 0px">您与此服务器的连接速度是 <%=tnetspeed%> kB/s (相当于<%=tnetspeed2%>kbps)
  <br><font color="#888888">换算关系:  1 Byte(字节) = 8 bit(位)</font></p>
  </td></tr>
</table>
<%

  end if
 end if
end sub






' *******************************************************************************
' 　　[ L ] 不安全组件检测
' *******************************************************************************
sub tsafe()
%>


<h2>不安全组件检测</h2>
<p>WScript.Shell <%=okObj("ws")%>, Shell.application <%=okObj("app")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>Shell 组件允许ASP运行.exe等可执行文件，存在严重的安全隐患。即使在文件系统进行过严格的权限设置的服务器上，此组件也会被用来运行提升权限的程序。</td>
    </tr>
  </table>
<p>WScript.Network <%=okObj("net")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>WScript.Network 为ASP程序罗列和创建系统用户(組)提供了可能，如果上面提示“√ 危险”则可能存在此安全隐患。</td>
    </tr>
  </table>
<p>Adodb.Stream <%=okObj("ado")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>Adodb.Stream 常常被用来上传木马等不安全程序，从而扩大攻击者的破坏能力。通过必要的权限设置，Adodb.Stream不会对系统安全造成威胁，它常常被使用在无组件上传工具中。</td>
    </tr>
  </table>
<p>FSO <%=okObj("fso")%>, XML V1.0 <%=okObj("x1")%>, V2.6 <%=okObj("x2")%>, V3.0 <%=okObj("x3")%>, V4.0 <%=okObj("x4")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>FSO(Scripting.FileSystemObject) 和 XML 具备罗列和管理服务器中文件和文件夹的能力，如果权限设置不当，将导致木马程序移动、修改甚至删除服务器上的文件。FSO 组件是常用的组件之一，禁用该组件并不是最理想的安全措施。</td>
    </tr>
  </table>
<p>HappyTime <%=okObj("hap")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>HappyTime(欢乐时光)是流行的网络蠕虫病毒之一，它的繁殖占用大量网络带宽，病毒发作时它有可能删除服务器上有用的可执行文件导致系统瘫痪。如果此项检测结果为危险，则您的服务器将存在感染和传播欢乐时光病毒的可能。</td>
    </tr>
  </table>
<p>[<a href="http://www.ajiang.net/products/aspcheck/safe.asp">点击这里参考阿江的安全配置方法</a>]
<%
end sub






' *******************************************************************************
' 　　[ M ] 系统用户和进程检测
' *******************************************************************************
sub userlist()
%>
<h2>系统用户(組)和进程检测</h2>
<p>如果下面列出了系统用户和进程，则说明系统存在安全隐患。</p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr class="backs">
      <td width="100">类 型</td><td width="400">名称及详情</td>
    </tr>
<%
  on error resume next
    for each obj in getObject("WinNT://.")
	err.clear
%>
    <tr>
      <td align=center><!--<%=obj.path%>-->
<%
    if err then
      Response.Write "系统用户(組)"
    else
      Response.Write "系统进程"
    end if
%>
      </td>
	  <td><%=obj.Name%><%if err=0 then Response.Write " (" & obj.displayname & ")"%><br><%=obj.path%>
	  </td>
	</tr>
<%
	next 
%>
  </table>
<p>[<a href="http://www.ajiang.net/products/aspcheck/safe.asp">点击这里参考阿江的安全配置方法</a>]
<%
end sub




' *******************************************************************************
' 　　[ N ] 主菜单
' *******************************************************************************
sub mmenu()
%>
<h2>主菜单</h2>
  <p>快速查看: <a href="?T=BG">精简模式</a> | <a href="?T=BGHIJ">典型模式</a> | <a href="?T=ABGHIJKLMCDEF">完整模式</a></p>
  <p>功能直达: <a href="?T=B">概况</a>
  | <a href="?T=G">组件</a>
  | <a href="?T=F">环境</a>
  | <a href="?T=HI">磁盘</a>
  | <a href="?T=J">运算速度</a>
  | <a href="?T=K">带宽检测</a>
  | <a href="?T=LHM">安全状况</a></p>
<%
end sub




' *******************************************************************************
' 　　其他函数和子程序
' *******************************************************************************

' 展示栏目
sub BodyGo(gCon)
  select case gCon
  case "A"
    call aspyes()
  case "B"
    call servinfo()
  case "C"
    call applist()
  case "D"
    call seslist()
  case "E"
    call sevalist()
  case "F"
    call wsslist()
  case "G"
    call comlist()
  case "H"
    call disklist()
  case "I"
    call diskspeed()
  case "J"
    call tspeed()
  case "K"
    call tnet()
  case "L"
    call tsafe()
  case "M"
    call userlist()
  case "N"
    call mmenu()
  end select
end sub


' 检测不安全组件
Function okObj(runstr)
  On Error Resume Next
  Response.Write "<span style=""display:none"">"
  okObj = true
  Err = 0
  Execute runstr & ".exec()"
  If 429 = Err Then
    okObj = false
  end if
  Err = 0
  Response.Write "</span>"
  if okObj then
    okObj="<font color=""red"">√ 危险</font>"
  else
    okObj="<font color=""green"">× 安全</font>"
  end if
End Function

' 转换字串为HTML代码
function cHtml(iText)
  cHtml = iText
  cHtml = server.HTMLEncode(cHtml)
  cHtml = replace(cHtml,chr(10),"<br>")
end function

' 转换磁盘类型为中文
function cdrivetype(tnum)
  Select Case tnum
    Case 0: cdrivetype = "未知"
    Case 1: cdrivetype = "可移动磁盘"
    Case 2: cdrivetype = "本地硬盘"
    Case 3: cdrivetype = "网络磁盘"
    Case 4: cdrivetype = "CD-ROM"
    Case 5: cdrivetype = "RAM 磁盘"
  End Select
end function

' 将是否可用转换为对号和错号
function cIsReady(trd)
  Select Case trd
    case true: cIsReady="<font class=fonts><b>√</b></font>"
    case false: cIsReady="<font color='red'><b>×</b></font>"
  End Select
end function

' 转换字节数为简写形式
function cSize(tSize)
  if tSize>=1073741824 then
    cSize=int((tSize/1073741824)*1000)/1000 & " GB"
  elseif tSize>=1048576 then
    cSize=int((tSize/1048576)*1000)/1000 & " MB"
  elseif tSize>=1024 then
    cSize=int((tSize/1024)*1000)/1000 & " KB"
  else
    cSize=tSize & "B"
  end if
end function

'检查组件是否被支持及组件版本的子程序
sub ObjTest(strObj)
  'on error resume next
  IsObj=false
  VerObj=""
 
  set TestObj=server.CreateObject (strObj)
  
  If -2147221005 <> Err then		'感谢网友iAmFisher的宝贵建议
    IsObj = True
    VerObj = TestObj.version
    if VerObj="" or isnull(VerObj) then VerObj=TestObj.about
  end if
  set TestObj=nothing
End sub

%>