<%
'=================================================================== 
'使用说明 
'程序参数说明 
'PapgeSize 定义分页每一页的记录数 
'GetRS 返回经过分页的Recordset此属性只读 
'GetConn 得到数据库连接 
'GetSQL 得到查询语句 
'程序方法说明 
'ShowPage 显示分页导航条,唯一的公用方法 
' 
'=================================================================== 

Const Btn_First="首页" '定义第一页按钮显示样式 
Const Btn_Prev="上一页" '定义前一页按钮显示样式 
Const Btn_Next="下一页" '定义下一页按钮显示样式 

Const Btn_Last="尾页" '定义最后一页按钮显示样式 
Const XD_Align="Center" '定义分页信息对齐方式 
Const XD_Width="100%" '定义分页信息框大小 

Class Xdownpage 
Private XD_PageCount,XD_Conn,XD_Rs,XD_SQL,XD_PageSize,Str_errors,int_curpage,str_URL,int_totalPage,int_totalRecord,XD_sURL 


'================================================================= 
'PageSize 属性 
'设置每一页的分页大小 
'================================================================= 
Public Property Let PageSize(int_PageSize) 
If IsNumeric(Int_Pagesize) Then 
XD_PageSize=CLng(int_PageSize) 
Else 
str_error=str_error & "PageSize的参数不正确" 
ShowError() 
End If 
End Property 
Public Property Get PageSize 
If XD_PageSize="" or (not(IsNumeric(XD_PageSize))) Then 
PageSize=10 
Else 
PageSize=XD_PageSize 
End If 
End Property 

'================================================================= 
'GetRS 属性 
'返回分页后的记录集 
'================================================================= 
Public Property Get GetRs() 
Set XD_Rs=Server.createobject("adodb.recordset") 
XD_Rs.PageSize=PageSize 
XD_Rs.Open XD_SQL,XD_Conn,1,1 
If not(XD_Rs.eof and XD_RS.BOF) Then 
If int_curpage>XD_RS.PageCount Then 
int_curpage=XD_RS.PageCount 
End If 
XD_Rs.AbsolutePage=int_curpage 
End If 
Set GetRs=XD_RS 
End Property 

'================================================================ 
'GetConn 得到数据库连接 
' 
'================================================================ 
Public Property Let GetConn(obj_Conn) 
Set XD_Conn=obj_Conn 
End Property 

'================================================================ 
'GetSQL 得到查询语句 
' 
'================================================================ 

Public Property Let GetSQL(str_sql) 
XD_SQL=str_sql 
End Property 



'================================================================== 
'Class_Initialize 类的初始化 
'初始化当前页的值 
' 
'================================================================== 
Private Sub Class_Initialize 
'======================== 
'设定一些参数的黙认值 
'======================== 
XD_PageSize=10 '设定分页的默认值为10 
'======================== 
'获取当前面的值 
'======================== 
If request("page")="" Then 
int_curpage=1 
ElseIf not(IsNumeric(request("page"))) Then 
int_curpage=1 
ElseIf CInt(Trim(request("page")))<1 Then 
int_curpage=1 
Else 
Int_curpage=CInt(Trim(request("page"))) 
End If 

End Sub 

'==================================================================== 
'ShowPage 创建分页导航条 
'有首页、前一页、下一页、末页、还有数字导航 
' 
'==================================================================== 
Public Sub ShowPage() 
Dim str_tmp 
XD_sURL = GetUrl() 
int_totalRecord=XD_RS.RecordCount 
If int_totalRecord<=0 Then 
str_error=str_error & "总记录数为零，请输入数据" 
Call ShowError() 
End If 
If int_totalRecord="" then 
int_TotalPage=1 
Else 
If int_totalRecord mod PageSize =0 Then 
int_TotalPage = CLng(int_TotalRecord / XD_PageSize) 
Else 
int_TotalPage = int(int_TotalRecord / XD_PageSize)+1
End If 

End If 

If Int_curpage>int_Totalpage Then 
int_curpage=int_TotalPage 
End If 

'================================================================== 
'显示分页信息，各个模块根据自己要求更改显求位置 
'================================================================== 
response.write "" 
str_tmp=ShowFirstPrv 
response.write str_tmp 
str_tmp=ShowNextLast 
response.write str_tmp 
str_tmp=ShowPageInfo 
response.write str_tmp 

str_tmp=showNumBtn 
response.write str_tmp 

response.write "" 
End Sub 

'==================================================================== 
'ShowFirstPrv 显示首页、前一页 
' 
' 
'==================================================================== 
Private Function ShowFirstPrv() 
Dim Str_tmp,int_prvpage 
If int_curpage=1 Then 
str_tmp=Btn_First&" "&Btn_Prev & " "
Else 
int_prvpage=int_curpage-1 
str_tmp="<a href="""&XD_sURL & "1" & """>" & Btn_First&"</a> <a href=""" & XD_sURL & CStr(int_prvpage) & """>" & Btn_Prev&"</a> " 
End If 
ShowFirstPrv=str_tmp 
End Function 

'==================================================================== 


'ShowNextLast 下一页、末页 
' 
' 
'==================================================================== 
Private Function ShowNextLast() 
Dim str_tmp,int_Nextpage 
If Int_curpage>=int_totalpage Then 
str_tmp=Btn_Next & " " & Btn_Last & " "
Else 
Int_NextPage=int_curpage+1 
str_tmp="<a href=""" & XD_sURL & CStr(int_nextpage) & """>" & Btn_Next&"</a> <a href="""& XD_sURL & CStr(int_totalpage) & """>" & Btn_Last&"</a> " 
End If 
ShowNextLast=str_tmp 
End Function 


'==================================================================== 
'ShowNumBtn 数字导航 
' 
' 
'==================================================================== 
Private Function showNumBtn() 
Dim i,str_tmp 

str_tmp=str_tmp &"转第 <select name=""select"" onchange=""javascript:window.location.href=this.options[this.selectedIndex].value"">"
For i=1 to int_totalpage 
'str_tmp=str_tmp & "[<a href=""" & XD_sURL & CStr(i) & """>"&i&"</a>] " 
if i=int_curpage then
str_tmp=str_tmp &"<option selected value='"&XD_sURL & CStr(i) &"'>"&i&"</option>"
else
str_tmp=str_tmp &"<option value='"&XD_sURL & CStr(i) &"'>"&i&"</option>"

end if
Next 
str_tmp=str_tmp &"</select> 页"
showNumBtn=str_tmp 
End Function 


'==================================================================== 
'ShowPageInfo 分页信息 
'更据要求自行修改 
' 
'==================================================================== 
Private Function ShowPageInfo() 
Dim str_tmp 
str_tmp="页次："&int_curpage&"/"&int_totalpage&"页 共"&int_totalrecord&"条记录 "&XD_PageSize&"条/页 " 
ShowPageInfo=str_tmp 
End Function 
'================================================================== 
'GetURL 得到当前的URL 
'更据URL参数不同，获取不同的结果 
' 
'================================================================== 
Private Function GetURL() 
Dim strurl,str_url,i,j,search_str,result_url 
search_str="page=" 

strurl=Request.ServerVariables("URL") 
Strurl=split(strurl,"/") 
i=UBound(strurl,1) 

str_url=strurl(i)'得到当前页文件名 

str_params=Trim(Request.ServerVariables("QUERY_STRING")) 
If str_params="" Then 
result_url=str_url & "?page=" 
Else 
If InstrRev(str_params,search_str)=0 Then 
result_url=str_url & "?" & str_params &"&page=" 
Else 
j=InstrRev(str_params,search_str)-2 
If j=-1 Then 
result_url=str_url & "?page=" 
Else 
str_params=Left(str_params,j) 
result_url=str_url & "?" & str_params &"&page=" 
End If 
End If 
End If 
GetURL=result_url 
End Function 

'==================================================================== 
' 设置 Terminate 事件。 
' 
'==================================================================== 
Private Sub Class_Terminate 
XD_RS.close 
Set XD_RS=nothing 
End Sub 
'==================================================================== 


'ShowError 错误提示 
' 
' 
'==================================================================== 
Private Sub ShowError() 
If str_Error <> "" Then 
Response.Write("" & str_Error & "") 
Response.End 
End If 
End Sub 
End class 
%>