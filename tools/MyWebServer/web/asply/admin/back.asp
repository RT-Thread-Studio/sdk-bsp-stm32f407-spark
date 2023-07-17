<%
if not session("check")="checked" then
response.Redirect "login.asp"
end if
if  Request("action")="ys" then
Const JET_3X = 4
Function CompactDB(dbPath, boolIs97)
Dim fso, Engine, strDBPath
strDBPath = left(dbPath,instrrev(DBPath,""))
Set fso = CreateObject("Scripting.FileSystemObject")
If fso.FileExists(dbPath) Then
Set Engine = CreateObject("JRO.JetEngine")
If boolIs97 = "True" Then
Engine.CompactDatabase "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & dbpath, _
"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & strDBPath & "temp.mdb;" _
& "Jet OLEDB:Engine Type=" & JET_3X
Else
Engine.CompactDatabase "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & dbpath, _
"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & strDBPath & "temp.mdb"
End If
fso.CopyFile strDBPath & "temp.mdb",dbpath
fso.DeleteFile(strDBPath & "temp.mdb")
Set fso = nothing
Set Engine = nothing
CompactDB = "数据库压缩成功！"
Else
CompactDB = "数据库压缩失败，请重试"
End If
End Function

Dim dbpath,boolIs97
dbpath = "../db/sywl.asp"
'boolIs97 = request("boolIs97")
dbpath = server.mappath(dbpath)
'response.write(CompactDB(dbpath,boolIs97))

Response.Write("<SCRIPT language=JavaScript>alert('"&CompactDB(dbpath,boolIs97)&"');this.location.href='"&request.ServerVariables("HTTP_REFERER")&"';</SCRIPT>")
Response.End
end if

if Request("action")="back" then
currf="../db/sywl.asp"
currf=server.mappath(currf)
backf="../db/"
backf=server.mappath(backf)
backfy=Format_Time(now,2)&".bak"
on error resume next
Set objfso = Server.CreateObject("Scripting.FileSystemObject")
if err then 
err.clear
response.write "<script>alert(""不能建立fso对象，请确保你的空间支持fso:！"");history.back();</script>"
response.end
end if
if objfso.Folderexists(backf) then
else
Set fy=objfso.CreateFolder(backf)
end if
objfso.copyfile currf,backf& "\"& backfy
Response.Write("<SCRIPT language=JavaScript>alert('数据库备份成功');this.location.href='"&request.ServerVariables("HTTP_REFERER")&"';</SCRIPT>")
response.end
end if 

' ============================================
' 格式化时间(显示)
' 参数：n_Flag
' 1:"yyyy-mm-dd hh:mm:ss"
' 2:"yyyy-mm-dd"
' 3:"hh:mm:ss"
' 4:"yyyy年mm月dd日"
' 5:"yyyymmdd"
' 6:"yyyymmddhhmmss"
' ============================================
Function Format_Time(s_Time, n_Flag)
Dim y, m, d, h, mi, s
Format_Time = ""
If IsDate(s_Time) = False Then Exit Function
y = cstr(year(s_Time))
m = cstr(month(s_Time))
If len(m) = 1 Then m = "0" & m
d = cstr(day(s_Time))
If len(d) = 1 Then d = "0" & d
h = cstr(hour(s_Time))
If len(h) = 1 Then h = "0" & h
mi = cstr(minute(s_Time))
If len(mi) = 1 Then mi = "0" & mi
s = cstr(second(s_Time))
If len(s) = 1 Then s = "0" & s
Select Case n_Flag
Case 1
' yyyy-mm-dd hh:mm:ss
Format_Time = y & "-" & m & "-" & d & " " & h & ":" & mi & ":" & s
Case 2
' yyyy-mm-dd
Format_Time = y & "-" & m & "-" & d
Case 3
' hh:mm:ss
Format_Time = h & ":" & mi & ":" & s
Case 4
' yyyy年mm月dd日
Format_Time = y & "年" & m & "月" & d & "日"
Case 5
' yyyymmdd
Format_Time = y & m & d
case 6
'yyyymmddhhmmss
format_time= y & m & d & h & mi & s
End Select
End Function%>