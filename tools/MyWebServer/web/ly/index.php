<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="gb2312">
<head>
<title>简单留言本(PHP+ACCESS)</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<meta http-equiv="Content-Language" content="gb2312" />
<meta name="author" content="學無止境，QQ:182407777" />
<meta name="Copyright" content="www.wesent.cn,自由版权,任意转载" />
<meta name="description" content="php,access,php初学" />
<meta content="php,access,php初学，學無止境" name="keywords" />
</head> 
<body>
<BR><BR>
<center><a href="admin_login.htm" target="_blank">留言管理</a> </center><BR><BR>
<font color="red">最新留言如下：<BR><BR></font>
<?php 
     include_once("include/conn.php");
     $rs = $conn->Execute('SELECT * FROM contents order by id desc');
     while(!$rs->EOF)  
            { 
              echo "<table><tr><td>留言人ID:<font color='red'>".$rs->Fields['id']->Value ."</font></td>";
              echo "<td>&nbsp;&nbsp;姓名:".$rs->Fields['name']->Value ."</td></tr>"; 
              echo "<tr><td colspan='2'>留言:".$rs->Fields['content']->Value ."</td></tr></table><br/>";
              echo "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "; 
              $rs->MoveNext();
			  }
	  /*释放资源*/
	  $rs->Close();
      $conn->Close();
      $rs = null;
      $conn = null;
 ?>
<br><br><br>
<form action="add_messages.php" method="post"> 
    姓名:<input type="text" name="user_name" size="60"><br> 
    留言:<textarea name="user_post" rows="20" cols="59"></textarea>
	<input type="submit" value="提交留言"> 
   </form>
</body>
</html>