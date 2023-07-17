<? require_once('inc/config.php'); ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title>最简易PHP+Access文章管理，By xinyuefei.com</title>
<link rel="shortcut icon" href="images/favicon.ico" />
<link href="style/css.css" rel="stylesheet" type="text/css" />
</head>
<body><div id="box">
	<div id="header"></div>
	<div id="content">
		<div id="logo">最简易PHP+Access文章管理</div>
		<div id="navi">您的位置：<a href="index.php">首页</a><img src="images/arrow2.gif" />文章列表</div>
		<div id="list">
			<div id="listcontent">
				<h1>文章列表</h1>
				<ul class="indexlist">
				<? $rs=new COM("adodb.recordset");
				$rs->open("select * from [article] order by [articleid]",$conn,1,3);
				$rs->pagesize=5;
				$page=trim($_GET['page']);
				if($page=="" || is_numeric(intval($page))<=0){$page=1;}else if(intval($page)>$rs->pagecount){$page=$rs->pagecount;}
				$page=intval($page); 
				if(!$rs->eof||$rs->bof){
					$rs->absolutepage=$page; $mypagesize=$rs->pagesize; $i=$rs->recordCount()-($page-1)*$rs->pagesize;  
					while(!$rs->eof && $mypagesize>0 && $i>=$rs->recordCount()-1-$rs->pagesize*$page){ 
						$articleid = $rs->Fields(0)->value;
						$articletitle = $rs->Fields(1)->value; ?>
						<li><a href="detail.php?id=<? echo $articleid; ?>"><? echo $articletitle; ?></a><span><? echo $articlei; ?></span></li>
						<? $rs->movenext; $mypagesize--; $i--;  } } ?>
				</ul>
			</div>
		</div>
		<div id="page">
			<a href="index.php?page=1">头页</a>&nbsp;<a href="index.php?page=<? if($page>1){echo $page-1;}else{echo 1;}?>">上页</a>&nbsp;<a href="index.php?page=<? if($page<$rs->pagecount){echo $page+1;}else{echo $rs->pagecount;}?>">下页</a>&nbsp;<a href="index.php?page=<? echo $rs->pagecount;?>">尾页</a>&nbsp;&nbsp;共<? echo $rs->recordCount(); ?>篇文章，分<? echo $rs->pagecount;?>页显示，现在是第<? echo $page;?>页
		</div>
		<div id="links">
			<ul>
			<li><a href="http://xinyuefei.com/">首页</a></li>
			<li><a href="http://xinyuefei.com/">关于</a></li>
			<li class="last"><a href="http://xinyuefei.com/contact.htm">联系</a></li>
			</ul>
		</div>
	</div>
	<div id="footer"></div>
</div></body>
</html>
<? $rs->Close(); $rs=null;
$conn->Close(); $conn=null; ?>