<!--#include file="guke_up.asp"-->
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<style type="text/css">
<!--
BODY{
BACKGROUND-COLOR: #E1F4EE;
font-size:9pt
}
.tx1 { height: 20px;font-size: 9pt; border: 1px solid; border-color: #000000; color: #0000FF}
-->
</style>
<SCRIPT language=javascript>
function check() 
{
	var strFileName=document.form1.FileName.value;
	if (strFileName=="")
	{
    	alert("请选择要上传的文件");
		document.form1.FileName.focus();
    	return false;
  	}
}
</SCRIPT>
</head>
<body leftmargin="0" topmargin="0" style="background-color: #D6DFF7">
<form action="guke_up2.asp" method="post" name="form1" onSubmit="return check()" enctype="multipart/form-data">
  <input name="FileName" type="FILE" class="tx1" size="23">
  <input type="submit" name="Submit" value="上传" style="border:1px double rgb(88,88,88);font:9pt">
</form>
</body>
</html>