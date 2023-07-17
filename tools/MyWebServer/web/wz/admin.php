<? require_once('inc/config.php'); ?>
<? $do = $_GET["do"]; ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<meta name="Robots" content="noindex,nofollow" />
<title>后台管理</title>
<link href="style/style.css" rel="stylesheet" type="text/css" />
<script type="text/JavaScript">
<!--
function confirmdelete() { var msg = "确定要删除？"; if (confirm(msg)==true){ return true; }else{ return false; } }
//-->
</script>
</head>
<body>
<div id="main">
	<div id="middle">
		<div id="column1">
		<? if (isset($_COOKIE["admincookie"])){ ?>
			<div class="topborder"><div class="rightborder"><div class="bottomborder"><div class="leftborder"><div class="topleft"><div class="topright"><div class="bottomright"><div class="bottomleft">
				<div class="title">操作</div>
				<ul class="newul">
                                        <li><a href="index.php">首页</a></li>
					<li><a href="admin.php?do=new">添加</a></li>
					<li><a href="admin.php?do=manage">文章列表</a></li>
				</ul>
				<ul class="newul">
					<li><img src="style/logout.gif" style=" padding: 0px 5px 0px 0px;" /><a href="admin.php?do=logout">注销</a></li>
				</ul>
			</div></div></div></div></div></div></div></div>
		<? ;} ?>
		</div>
		<div id="column2">
		<? if (!isset($_COOKIE["admincookie"])){ ?>
			<div class="summary"><img src="style/login.png" />登录</div>
			<form name="myForm" method="post" action="admin.php?do=login">
			<div id="message">
				<p><label for="adminname">name</label><input name="adminname" id="adminname" type="text" /></p>
				<p><label for="adminpassword">password</label><input name="adminpassword" id="adminpassword" type="password" /></p>
				<p><input name="mySubmit" id="mySubmit" type="submit" value="登录" class="input1" /></p>
			</div>
			</form>
		<? ;}
		if($do=="login"){
			$adminname1 = $_POST["adminname"]; $adminpassword1 = $_POST["adminpassword"];
			$rs=new COM("adodb.recordset");
			$rs->open("select * from [admin]",$conn,1,1);
			$adminname = $rs->Fields(1)->value;
			$adminpassword = $rs->Fields(2)->value;
			$rs->Close(); $rs=null;
			$conn->Close(); $conn=null;
			$admincookie = $adminname;
			if ($adminname==$adminname1 && $adminpassword==$adminpassword1){
				setcookie("admincookie", $admincookie, time()+3600);
				echo "<script>window.location.href='admin.php?do=new';</script>";
			}else{echo "<script>alert('用户名或密码错误！');window.history.back(-1);</script>";}
		}
		if($do=="logout"){
			$conn->Close(); $conn=null;
			setcookie("admincookie", "", time()-3600);
			echo "<script>window.location.href='admin.php';</script>";
		}

		if (isset($_COOKIE["admincookie"])){
			if($do=="new"){
				$conn->Close(); $conn=null; ?>
				<form name="myForm" method="post" action="admin.php?do=add">
				<div id="message">
					<p><label for="mytitle">标题</label><input name="mytitle" id="mytitle" type="text" /></p>
					<p><label for="mycontent">内容</label><textarea name="mycontent" id="mycontent" rows="15"><p></p></textarea>
					</p>
					<p><input name="mySubmit" id="mySubmit" type="submit" value="提交表单" class="input1" /></p>
				</div>
				</form>	
			<? ;}
			if($do=="add"){
				$articletitle=$_POST["mytitle"]; $articlecontent=$_POST["mycontent"];
				if (!$articletitle||!$articlecontent){
					echo "<script>alert('内容不能为空！');window.history.back(-1);</script>";
				}else{
					$conn->execute("INSERT INTO [article] (articletitle,articlecontent) VALUES ('$articletitle','$articlecontent')");
					$conn->Close(); $conn=null;
					echo "<script>alert('添加成功！');window.location.href='admin.php?do=new';</script>";
				}
			}
			if($do=="" || $do=="manage"){
				$rs=new COM("adodb.recordset");
				$rs->open("select * from [article] order by [articleid] desc",$conn,1,3); ?>
				<div id="message">
				<table width="80%" style="background:#ffc;boader:#0 1px" border="1" cellpadding="0" cellspacing="1" bgcolor="#CCC">
				<tr>
				<td  align="center" >文章ID</td>
				<td align="center" >文章标题</td>
				<td  align="center" >操作</td>
				</tr>
				<? while(!$rs->eof){
					$articleid = $rs->Fields(0)->value;
					$articletitle = $rs->Fields(1)->value; ?>
					<tr>
					<td align="center" ><? echo $articleid; ?></td>
					<td ><a href="detail.php?id=<? echo $articleid; ?>" target="_blank"><? echo $articletitle; ?></a></td>
					<td align="center" ><a href="admin.php?do=edit&id=<? echo $articleid; ?>">修改</a>&nbsp;/&nbsp;<a href="admin.php?do=delete&id=<? echo $articleid; ?>" onclick="javascript:return confirmdelete()">删除</a></td>
					</tr>
					<? $rs->movenext;
				} ?>
				</table>
				</div>
				<? $rs->Close(); $rs=null;
				$conn->Close(); $conn=null;
			}
			if($do=="edit"){
				$articleid=$_GET['id'];
				$rs=new COM("adodb.recordset");
				$rs->open("select * from [article] where [articleid]=".$articleid,$conn,1,1);
				$articletitle = $rs->Fields(1)->value;
				$articlecontent = $rs->Fields(2)->value; ?>
				<form name="mainForm" method="post" action="admin.php?do=modify">
				<div id="message">
					<p><label for="mytitle">标题</label><input name="mytitle" id="mytitle" type="text" value="<? echo $articletitle; ?>" /></p>
					<p><label for="mycontent">内容</label><textarea name="mycontent" id="mycontent" rows="15"><? echo stripslashes($articlecontent); ?></textarea></p>
					<p><input name="mainSubmit" id="mainSubmit" type="submit" value="提交表单" class="input1" /></p>
				</div><input name="myid" id="myid" type="hidden" value="<? echo $articleid ?>" />
				</form>	
				<? $rs->Close(); $rs=null;
				$conn->Close(); $conn=null;
			}
			if($do=="modify"){
				$articleid=$_POST["myid"]; $articletitle=$_POST["mytitle"]; $articlecontent=$_POST["mycontent"]; 
				$conn->execute("UPDATE [article] set [articletitle] = '$articletitle' , [articlecontent] = '$articlecontent' where [articleid]=".$articleid);
				$conn->Close(); $conn=null;
				echo "<script>alert('修改成功！');window.location.href='admin.php?do=manage';</script>";
			}
			if($do=="delete"){
				$articleid=$_GET['id'];
				$conn->execute("delete from [article] where [articleid]=".$articleid);
				$conn->Close(); $conn=null;
				echo "<script>alert('删除成功！');window.location.href='admin.php?do=manage';</script>";
			}
		} ?>
		</div>
</div></div>
</body>
</html>