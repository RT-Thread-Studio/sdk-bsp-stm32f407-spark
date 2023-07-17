<%
Function Get_SafeStr(str)
Get_SafeStr = Replace(Replace(Replace(Replace(Replace(Trim(str), "'", ""), Chr(34), ""), ";", ""),"=",""),">","")
End Function

Function IsSafeStr(str)
	Dim s_BadStr, n, i
	s_BadStr = "' 　&<>?%,;:()`~!@#$^*{}[]|+-=" & Chr(34) & Chr(9) & Chr(32)
	n = Len(s_BadStr)
	IsSafeStr = True
	For i = 1 To n
		If Instr(str, Mid(s_BadStr, i, 1)) > 0 Then
			IsSafeStr = False
			Exit Function
		End If
	Next
End Function

db="db/sywl.asp"       
     '    On Error Resume Next
	dim ConnStr
	dim conn
		ConnStr = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & Server.MapPath(db)
		Set conn = Server.CreateObject("ADODB.Connection")
	conn.Open connstr
	If Err Then
		err.Clear
		Set Conn = Nothing
		Response.Write "数据库连接出错，请检查Conn.asp文件中的数据库参数设置。"
		Response.End
	End If



sub CloseConn()
	On Error Resume Next
	If IsObject(Conn) Then
		conn.close
		set conn=nothing
	end if
end sub

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
Case 7
' yyyy-mm-dd
Format_Time =m & "-" & d
End Select
End Function

'*************************************************
'函数名：gotTopic
'作  用：截字符串，汉字一个算两个字符，英文算一个字符
'参  数：str   ----原字符串
'       strlen ----截取长度
'返回值：截取后的字符串
'*************************************************
function gotTopic(str,strlen)
	if str="" then
		gotTopic=".."
		exit function
	end if
	dim l,t,c, i
	str=replace(replace(replace(replace(str,"&nbsp;"," "),"&quot;",chr(34)),"&gt;",">"),"&lt;","<")
	l=len(str)
	t=0
	for i=1 to l
		c=Abs(Asc(Mid(str,i,1)))
		if c>255 then
			t=t+2
		else
			t=t+1
		end if
		if t>=strlen then
			gotTopic=left(str,i)&""
			exit for
		else
			gotTopic=str
		end if
	next
	gotTopic=replace(replace(replace(replace(gotTopic," ","&nbsp;"),chr(34),"&quot;"),">","&gt;"),"<","&lt;")
end function


'去除html标签
  function nohtml(str) 
dim re 
Set re=new RegExp 
re.IgnoreCase =true 
re.Global=True 
re.Pattern="(\<.[^\<]*\>)" 
str=re.replace(str," ") 
re.Pattern="(\<\/[^\<]*\>)" 
str=re.replace(str," ") 
str=replace(str," "," ") 
str=replace(str," "," ") 
str=replace(str,"&nbsp;"," ")
nohtml=str 
set re=nothing 
end function 
%>


