<!-- #include file="connmdb.asp" -->
<htm>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=GB2312" >
</head>
<body>
<center>
<%
response.write "你的IP:" & request.ServerVariables("REMOTE_ADDR") & "<BR>"
d=request.querystring("d")
k=request.querystring("k")
if d="" then d="id"
if d="1" then d="id" 
if d="2" then d="使用科室" 
if d="3" then d="IP地址" 

set rs=server.createobject("adodb.recordset")
sql="select id,使用科室,基本配置,IP地址,MAC地址 from pcinfo order by " & d & " "  & k
rs.open sql,conn,1,1
if k="" then k="DESC" else k=""
%>

<A style="font-size:20px"><b>办公用电脑IP地址登记表</b></a><br /><br />
共计：<%=rs.recordcount%>台电脑。
<br /><br />

</div>

<table border="1" bordercolor="#bbbbFF"  bordercolorlight="#335566" bgcolor="#4084aE">
<thead>
<tr  style="text-align:center;"><td onclick="window.location='?d=1&k=<%=k%>'" style="cursor: default;">ID</td><td style="cursor: default;" onclick="window.location='?d=2&k=<%=k%>'">使用单位(科室)</td><td style="cursor: default;" onclick="window.location='?d=3&k=<%=k%>'">配置情况</td><td style="cursor: default;" onclick="window.location='?d=4&k=<%=k%>'">IP地址</td><td style="cursor: default;" onclick="window.location='?d=5&k=<%=k%>'">MAC地址</td></tr></thead><tbody><%
do while not rs.eof %>


<tr  STYLE="COLOR='#ffbb22';"><td ><%=rs(0)%></td><td><%=rs(1)%></td><td><%=rs(2)%></td><td><%=ip(rs(3))%></td><td><%=rs(4)%></td></tr>
<%
rs.movenext
loop
set rs=nothing
set conn=nothing
 

function ip(t)
ip=""
'on error resume next
a=split(t,".")

for i=0 to 3
if ip<>"" then ip=ip & "."
if a(i)="" then a(i)="0"
 ip=ip & cstr(cint(a(i)))
next
end function
%>
</tbody>
</table>
</center>
</body>
</html>