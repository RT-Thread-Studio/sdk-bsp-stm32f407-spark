<!-- #include file="connmdb.asp" -->
<%

' if instr(request.ServerVariables("REMOTE_ADDR"),"10.2.140.")=1 then
if request.Form("dw")="" or   request.Form("ip")=""  then response.Redirect("sip.asp")
edit=request.Form("edit")
set rs=server.createobject("adodb.recordset")
if edit<>"1" then
 sql="select * from pcinfo"
 rs.open sql,conn,1,3
 a=rs.addnew
 else
 sql="select * from pcinfo where id=" & request.Form("id") 
 rs.open sql,conn,1,3
 end if
 ipt=""
 tmp=request.Form("ip")
 tmp=ip(tmp)
 iptmp=split(tmp,".")
 if ubound(iptmp)<3 then response.write "<script>alert('IP地址错误,请重新输入!');location.href='showip.asp'</script>":response.end
 for i =0 to 3
 if iptmp(i)="" then iptmp(i)="000"
 if len(iptmp(i))=2 then iptmp(i)="0" & iptmp(i)
 if len(iptmp(i))=1 then iptmp(i)="00" & iptmp(i)
  if ipt<>""  then ipt=ipt & "."
  
  ipt=ipt & iptmp(i)
 next

rs(1)=request.Form("dw")
rs(2)=request.Form("r")
rs(3)=ipt
rs(4)=request.Form("pz")
rs(5)=request.Form("mac")
rs(6)=request.Form("bz")
rs.update

set rs=nothing
set conn=nothing
response.Redirect("showip.asp")
'end if

function ip(t)
ip=""
on error resume next
a=split(t,".")

for i=0 to 3
if ip<>"" then ip=ip & "."
 if a(i)="" then a(i)="0"
 ip=ip & cstr(cint(a(i)))
next
end function
 %>