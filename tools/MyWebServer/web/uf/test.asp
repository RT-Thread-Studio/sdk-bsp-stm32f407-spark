<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title>上传文件</title>
</head>
<body>
<script type="text/javascript">
function test(str){
    var picz = document.getElementById('pic').innerHTML
    var tc = document.getElementById("content");
    var tclen = tc.value.length;
    tc.focus();
    if(typeof document.selection != "undefined")
    {
        document.selection.createRange().text = str;  
    }
    else
    {
        tc.value = tc.value.substr(0,tc.selectionStart)+str+tc.value.substring(tc.selectionStart,tclen);
    }
}
</script>
<form method="post" action="" name="form1">
<table width="96%" border="0" cellspacing="0" cellpadding="0" align="center">
                <tr> 
                <td width="18%" align="right" height="40">图 片：</td>
                <td width="82%"> 
          <table>
            <tr>
<td><input type="text" name="pic" size="30" /> 
<input type="button" onclick="test('<img src=' + document.form1.pic.value + '></img>')" value="插入图片" />&nbsp;&nbsp;</td>
<td><iframe style="top:2px" ID="UploadFiles" src="guke_up1.asp" frameborder=0 scrolling=no width="92%" height="22" name="I1" marginwidth="1" marginheight="1"></iframe></td>
           </td>
          </tr>
            </table>
                </td>
              </tr>
              <tr> 
                <td width=18% align=right valign=top>内 容：</td>
                <td width=82% >
	<textarea id="content" name="content" rows="7" cols="81">这里是编辑框...</textarea>
               </td>
              </tr>
</table>
<table width=90% cellpadding=0 cellspacing=0 border=0 align=center>
<tr>
<td align="center" height="60">
<input type="button" value=" 提 交 " onclick="document.all['www'].innerText=document.form1.content.value;" />
</td>
</tr>
<tr>
<td align="center">
<h4 id="www"></h4>
</td>
</tr>
</table>
</form>
</body>
</html>