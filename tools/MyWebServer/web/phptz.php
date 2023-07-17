<?php
	//header("content-Type: text/html; charset=utf-8");
    error_reporting(E_ERROR | E_WARNING | E_PARSE);
	ob_start();
     
    $valInt = (false == empty($_POST['pInt']))?$_POST['pInt']:"未测试";
    $valFloat = (false == empty($_POST['pFloat']))?$_POST['pFloat']:"未测试";
    $valIo = (false == empty($_POST['pIo']))?$_POST['pIo']:"未测试";
    $mysqlReShow = "none";
    $mailReShow = "none";
    $funReShow = "none";
    $opReShow = "none";
    $sysReShow = "none";
    $ioncube = extension_loaded('ionCube Loader');
    $ffmpeg = extension_loaded("ffmpeg");
    $imagick = extension_loaded("imagick");	 
    define("YES", "<span class='resYes'>YES</span>");
    define("NO", "<span class='resNo'>NO</span>");
    define("ICON", "<span class='icon'>2</span>&nbsp;");
    $phpSelf = $_SERVER[PHP_SELF] ? $_SERVER[PHP_SELF] : $_SERVER[SCRIPT_NAME];
    define("PHPSELF", preg_replace("/(.{0,}?\/+)/", "", $phpSelf));
     
    if ($_GET['act'] == "phpinfo")
    {
        phpinfo();
        exit();
    }
    elseif($_POST['act'] == "测试1")
    {
        $valInt = 测试int();
    }
    elseif($_POST['act'] == "测试2")
    {
        $valFloat = 测试float();
    }
    elseif($_POST['act'] == "测试3")
    {
        $valIo = 测试io();
    }
    elseif($_POST['act'] == "CONNECT")
    {
        $mysqlReShow = "show";
        $mysqlRe = "MYSQL连接测试结果：";
        $mysqlRe .= (false !== @mysql_connect($_POST['mysqlHost'], $_POST['mysqlUser'], $_POST['mysqlPassword']))?"MYSQL服务器连接正常, ":
        "MYSQL服务器连接失败, ";
        $mysqlRe .= "数据库 <b>".$_POST['mysqlDb']."</b> ";
        $mysqlRe .= (false != @mysql_select_db($_POST['mysqlDb']))?"连接正常":
        "连接失败";
    }
    elseif($_POST['act'] == "SENDMAIL")
    {
        $mailReShow = "show";
        $mailRe = "MAIL邮件发送测试结果：发送";
        $mailRe .= (false !== @mail($_POST["mailReceiver"], "MAIL SERVER TEST", "This email is sent by iProber.\r\n\r\ndEpoch Studio\r\nhttp://depoch.net"))?"完成":"失败";
    }
    elseif($_POST['act'] == "FUNCTION_CHECK")
    {
        $funReShow = "show";
        $funRe = "函数 <b>".$_POST['funName']."</b> 支持状况检测结果：".isfun($_POST['funName']);
    }
    elseif($_POST['act'] == "CONFIGURATION_CHECK")
    {
        $opReShow = "show";
        $opRe = "配置参数 <b>".$_POST['opName']."</b> 检测结果：".getcon($_POST['opName']);
    }
     
     
    // 系统参数
     
     
    switch (PHP_OS)
    {
        case "Linux":
        $sysReShow = (false !== ($sysInfo = sys_linux()))?"show":"none";
        break;
        case "FreeBSD":
        $sysReShow = (false !== ($sysInfo = sys_freebsd()))?"show":"none";
        break;
        default:
        break;
    }
     
/*========================================================================*/
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>PHP探针 iProber V0.024 修改版</title>
<meta name="keywords" content="php探针,探针程序,php探针程序,探针,iProber,dEpoch Studio" />
<style type="text/css">
<!--
body,div,p,ul,form,h1 { margin:0px; padding:0px; }
body { background:#252724; }
div,a,input { font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 12px; color:#bbb; }
div { margin-left:auto; margin-right:auto; width:720px; }
input { border: 1px solid #414340; background:#353734; }
a,#t3 a.arrow,#f1 a.arrow { text-decoration:none; color:#978F78; }
a:hover { text-decoration:underline; }
a.arrow { font-family:Webdings, sans-serif; color:#343525; font-size:10px; }
a.arrow:hover { color:#ff0000; text-decoration:none; }
.resYes { font-size: 9px; font-weight: bold; color: #33CC00; } 
.resNo { font-size: 9px; font-weight: bold; color: #CC3300; }
.myButton { font-size:10px; font-weight:bold;  background:#3D3F3E; border:1px solid #4A4C49; border-right-color:#2D2F2C; border-bottom-color:#2D2F2C; color:#978F78; }
.bar { border:1px solid #2D2F2C; background:#6C6754; height:8px; font-size:2px; }
.bar li { background:#979179; height:8px; font-size:2px; list-style-type:none; }
table { clear:both; background:#2D2F2C; border:3px solid #41433E; margin-bottom:10px; }
td,th { padding:4px; background:#363835; }
th { background:#7E7860; color:#343525; text-align:left; }
th span { font-family:Webdings, sans-serif; font-weight:normal; padding-right:4px; }
th p { float:right; line-height:10px; text-align:right; }
th a { color:#343525; }
h1 { color:#009900; font-size:35px; width:300px; float:left; }
h1 b { color:#cc3300; font-size:50px; font-family: Webdings, sans-serif; font-weight:normal; }
h1 span { font-size:10px; padding-left:10px; color:#7D795E;  }
#t12 { float:right; text-align:right; padding:15px 0px 30px 0px; }
#t12 a { line-height:18px; color:#7D795E; }
#t3 td{ line-height:30px; height:30px; text-align:center; background:#3D3F3E; border:1px solid #4A4C49; border-right:none; border-bottom:none; }
#t3 th,#t3 th a { font-weight:normal; }
#m4 td {text-align:center;}
.th2 th,.th3 { background:#232522; text-align:center; color:#7D795E; font-weight:normal;  }
.th3 { font-weight:bold; text-align:left; }
#footer table { clear:none; }
#footer td { text-align:center; padding:1px 3px; font-size:9px; }
#footer a { font-size:9px; }
#f1 { text-align:right; padding:15px; }
#f2 {float:left; border:1px solid #dddddd; }
#f2 td { background:#FF6600; }
#f2 a { color:#ffffff; }
#f3 { border: 1px solid #888888; float:right; }
#f3 a { color:#222222; }
#f31 { background:#2359B1; color:#FFFFFF; }
#f32 { background:#dddddd; }
-->
</style>
</head>
<body>
<form method="post" action="<?=PHPSELF."#bottom"?>">
	<div>
		<input type="hidden" name="pInt" value="<?=$valInt?>" />
		<input type="hidden" name="pFloat" value="<?=$valFloat?>" />
		<input type="hidden" name="pIo" value="<?=$valIo?>" />
<!-- =============================================================
页头 
============================================================= -->
		<h1><i><b>i</b>Prober</i><span>V0.024</span></h1>
		<a name="top"></a>
		<p id="t12">
			<a href="http://depoch.net/download.htm" target="_blank">点击下载 iProber 探针, 或查看最新版本 </a>
			■<br />
			<a href="mailto:depoch@gmail.com">报告BUGS, 或反馈意见给 dEpoch Studio </a>
			■
		</p>
		<table width="100%" border="0" cellspacing="1" cellpadding="0" id="t3">
			<tr>
				<td><a href="#sec1">服务器特征</a></td>
				<td><a href="#sec2">PHP基本特征</a></td>
				<td><a href="#sec3">PHP组件支持状况</a></td>
				<td><a href="#sec4">服务器性能检测</a></td>
				<td><a href="#sec5">自定义检测</a></td>
				<td><a href="<?=PHPSELF?>" class="t211">刷新</a></td>
				<td><a href="#bottom" class="arrow">66</a></td>
			</tr>
			<tr>
				<th colspan="7"><b>〖AD〗</b>
				<a href="http://www.egobiz.com" target="_blank">羿高域名商务网: </a>
				<a href="http://www.egobiz.com/domain/" target="_blank">域名注册</a>
				<a href="http://www.egobiz.com/domain-sell/" target="_blank">域名交易</a>
				<a href="http://www.egobiz.com/deleting-domain/" target="_blank">过期域名|删除域名查询</a>
				<a href="http://www.egobiz.com/domain-tool/" target="_blank">域名工具集</a></th>
			</tr>
		</table>
<!-- =============================================================
服务器特性 
============================================================= -->
		<table width="100%" border="0" cellspacing="1" cellpadding="0">
			<tr>
				<th colspan="2"><p>
						<a href="#top" class="arrow">5</a>
						<br />
						<a href="#bottom" class="arrow">6</a>
					</p>
					<span>8</span>服务器特性
					<a name="sec1" id="sec1"></a>
				</th>
			</tr>
			<?if("show"==$sysReShow){?>
			<tr>
				<td>服务器处理器 CPU</td>
				<td>CPU个数：
					<?=$sysInfo['cpu']['num']?>
					<br />
					<?=$sysInfo['cpu']['detail']?></td>
			</tr>
			<?}?>
			<tr>
				<td>服务器时间</td>
				<td><?=date("Y年n月j日 H:i:s")?>
					&nbsp;北京时间：
					<?=gmdate("Y年n月j日 H:i:s",time()+8*3600)?></td>
			</tr>
			<?if("show"==$sysReShow){?>
			<tr>
				<td>服务器运行时间</td>
				<td><?=$sysInfo['uptime']?></td>
			</tr>
			<?}?>
			<tr>
				<td>服务器域名/IP地址</td>
				<td><?=$_SERVER['SERVER_NAME']?>
					(
					<?=@gethostbyname($_SERVER['SERVER_NAME'])?>
					)</td>
			</tr>
			<tr>
				<td>服务器操作系统
					<?$os = explode(" ", php_uname());?></td>
				<td><?=$os[0];?>
					&nbsp;内核版本：<?= php_uname()?>
					//<?=$os[1]." ".$os[3]." ".$os[4]." ".$os[5]?></td>
			</tr>
			<tr>
				<td>主机名称</td>
				<td><?=$os[2];?></td>
			</tr>
			<tr>
				<td>服务器解译引擎</td>
				<td><?=$_SERVER['SERVER_SOFTWARE']?></td>
			</tr>
			<tr>
				<td>Web服务端口</td>
				<td><?=$_SERVER['SERVER_PORT']?></td>
			</tr>
			<tr>
				<td>服务器管理员</td>
				<td><a href="mailto:<?=$_SERVER['SERVER_ADMIN']?>">
					<?=$_SERVER['SERVER_ADMIN']?>
					</a></td>
			</tr>
			<tr>
				<td>本文件路径</td>
				<td><?=$_SERVER['PATH_TRANSLATED']?></td>
			</tr>
			<tr>
				<td>目前还有空余空间&nbsp;diskfreespace</td>
				<td><?=round((@disk_free_space(".")/(1024*1024)),2)?>
					M</td>
			</tr>
			<?if("show"==$sysReShow){?>
			<tr>
				<td>内存使用状况</td>
				<td> 物理内存：共
					<?=$sysInfo['memTotal']?>
					M, 已使用
					<?=$sysInfo['memUsed']?>
					M, 空闲
					<?=$sysInfo['memFree']?>
					M, 使用率
					<?=$sysInfo['memPercent']?>
					%
					<?=bar($sysInfo['memPercent'])?>
					SWAP区：共
					<?=$sysInfo['swapTotal']?>
					M, 已使用
					<?=$sysInfo['swapUsed']?>
					M, 空闲
					<?=$sysInfo['swapFree']?>
					M, 使用率
					<?=$sysInfo['swapPercent']?>
					%
					<?=bar($sysInfo['swapPercent'])?>
				</td>
			</tr>
			<tr>
				<td>系统平均负载</td>
				<td><?=$sysInfo['loadAvg']?></td>
			</tr>
			<?}?>
		</table>

<!-- ============================================================= 
PHP基本特性 
============================================================== -->
		<table width="100%" cellpadding="0" cellspacing="1" border="0">
			<tr>
				<th colspan="2"><p>
						<a href="#top" class="arrow">5</a>
						<br />
						<a href="#bottom" class="arrow">6</a>
					</p>
					<span>8</span>PHP基本特性
					<a name="sec2" id="sec2"></a>
				</th>
			</tr>
			<tr>
				<td width="49%">PHP运行方式</td>
				<td width="51%"><?=strtoupper(php_sapi_name())?></td>
			</tr>
			<tr>
				<td>PHP版本</td>
				<td><?=PHP_VERSION?></td>
			</tr>
			<tr>
				<td>运行于安全模式</td>
				<td><?=getcon("safe_mode")?></td>
			</tr>
			<tr>
				<td>支持ZEND编译运行</td>
				<td><?=(get_cfg_var("zend_optimizer.optimization_level")||get_cfg_var("zend_extension_manager.optimizer_ts")||get_cfg_var("zend_extension_ts")) ?YES:NO?></td>
			</tr>
			<tr>
				<td>支持ioncube编译运行</td>
				<td><? if ($ioncube) { echo("<span class='resYes'>YES</span>");} 
				       else { echo("<span class='resNo'>NO</span>"); } ?> 
			    </td>
			</tr>
			<tr>
				<td>支持Eaccelerator加速</td>
				<td><?=(get_cfg_var("eaccelerator.allowed_admin_path")||get_cfg_var("eaccelerator.enable")||get_cfg_var("eaccelerator.optimizer")) ?YES:NO?></td>
			</tr>
			<tr>
				<td>支援FFmpeg组件</td>
				<td><? if ($ffmpeg) { echo("<span class='resYes'>YES</span>");} 
				       else { echo("<span class='resNo'>NO</span>"); } ?> 
			    </td>
			</tr>
			<tr>
				<td>支援Imagick组件</td>
				<td><? if ($imagick) { echo("<span class='resYes'>YES</span>");} 
				       else { echo("<span class='resNo'>NO</span>"); } ?> 
			    </td>
			</tr>
			<tr>
				<td>允许使用URL打开档&nbsp;allow_url_fopen</td>
				<td><?=getcon("allow_url_fopen")?></td>
			</tr>
			<tr>
				<td>允许动态加载链接库&nbsp;enable_dl</td>
				<td><?=getcon("enable_dl")?></td>
			</tr>
			<tr>
				<td>显示错误信息&nbsp;display_errors</td>
				<td><?=getcon("display_errors")?></td>
			</tr>
			<tr>
				<td>自动定义全局变量&nbsp;register_globals</td>
				<td><?=getcon("register_globals")?></td>
			</tr>
			<tr>
				<td>程序最多允许使用内存量&nbsp;memory_limit</td>
				<td><?=getcon("memory_limit")?></td>
			</tr>
			<tr>
				<td>POST最大字节数&nbsp;post_max_size</td>
				<td><?=getcon("post_max_size")?></td>
			</tr>
			<tr>
				<td>允许最大上传档&nbsp;upload_max_filesize</td>
				<td><?=getcon("upload_max_filesize")?></td>
			</tr>
			<tr>
				<td>程序最长运行时间&nbsp;max_execution_time</td>
				<td><?=getcon("max_execution_time")?>
					秒</td>
			</tr>
			<tr>
				<td>magic_quotes_gpc</td>
				<td><?=(1===get_magic_quotes_gpc())?YES:NO?></td>
			</tr>
			<tr>
				<td>magic_quotes_runtime</td>
				<td><?=(1===get_magic_quotes_runtime())?YES:NO?></td>
			</tr>
			<tr>
				<td>被禁用的函数&nbsp;disable_functions</td>
				<td><?=(""==($disFuns=get_cfg_var("disable_functions")))?"无":str_replace(",","<br />",$disFuns)?></td>
			</tr>
			<tr>
				<td>PHP信息&nbsp;PHPINFO</td>
				<td><?=(false!==eregi("phpinfo",$disFuns))?NO:"<a href='$phpSelf?act=phpinfo' target='_blank' class='static'>PHPINFO</a>"?></td>
			</tr>
		</table>
<!-- ============================================================= 
PHP组件支持 
============================================================== -->
		<table width="100%" cellpadding="0" cellspacing="1" border="0">
			<tr>
				<th colspan="4"><p>
						<a href="#top" class="arrow">5</a>
						<br />
						<a href="#bottom" class="arrow">6</a>
					</p>
					<span>8</span>PHP组件支持
					<a name="sec3" id="sec3"></a>
				</th>
			</tr>
			<tr>
				<td width="38%">拼写检查 ASpell Library</td>
				<td width="12%"><?=isfun("aspell_check_raw")?></td>
				<td width="38%">高精度数学运算 BCMath</td>
				<td width="12%"><?=isfun("bcadd")?></td>
			</tr>
			<tr>
				<td>历法运算 Calendar</td>
				<td><?=isfun("cal_days_in_month")?></td>
				<td>DBA数据库</td>
				<td><?=isfun("dba_close")?></td>
			</tr>
			<tr>
				<td>dBase数据库</td>
				<td><?=isfun("dbase_close")?></td>
				<td>DBM数据库</td>
				<td><?=isfun("dbmclose")?></td>
			</tr>
			<tr>
				<td>FDF窗体数据格式</td>
				<td><?=isfun("fdf_get_ap")?></td>
				<td>FilePro数据库</td>
				<td><?=isfun("filepro_fieldcount")?></td>
			</tr>
			<tr>
				<td>Hyperwave数据库</td>
				<td><?=isfun("hw_close")?></td>
				<td>图形处理 GD Library</td>
				<td><?=isfun("gd_info")?></td>
			</tr>
			<tr>
				<td>IMAP电子邮件系统</td>
				<td><?=isfun("imap_close")?></td>
				<td>Informix数据库</td>
				<td><?=isfun("ifx_close")?></td>
			</tr>
			<tr>
				<td>LDAP目录协议</td>
				<td><?=isfun("ldap_close")?></td>
				<td>MCrypt加密处理</td>
				<td><?=isfun("mcrypt_cbc")?></td>
			</tr>
			<tr>
				<td>哈稀计算 MHash</td>
				<td><?=isfun("mhash_count")?></td>
				<td>mSQL数据库</td>
				<td><?=isfun("msql_close")?></td>
			</tr>
			<tr>
				<td>SQL Server数据库</td>
				<td><?=isfun("mssql_close")?></td>
				<td>MySQL数据库</td>
				<td><?=isfun("mysql_close")?></td>
			</tr>
			<tr>
				<td>SyBase数据库</td>
				<td><?=isfun("sybase_close")?></td>
				<td>Yellow Page系统</td>
				<td><?=isfun("yp_match")?></td>
			</tr>
			<tr>
				<td>Oracle数据库</td>
				<td><?=isfun("ora_close")?></td>
				<td>Oracle 8 数据库</td>
				<td><?=isfun("OCILogOff")?></td>
			</tr>
			<tr>
				<td>PREL兼容语法 PCRE</td>
				<td><?=isfun("preg_match")?></td>
				<td>PDF文档支持</td>
				<td><?=isfun("pdf_close")?></td>
			</tr>
			<tr>
				<td>Postgre SQL数据库</td>
				<td><?=isfun("pg_close")?></td>
				<td>SNMP网络管理协议</td>
				<td><?=isfun("snmpget")?></td>
			</tr>
			<tr>
				<td>VMailMgr邮件处理</td>
				<td><?=isfun("vm_adduser")?></td>
				<td>WDDX支持</td>
				<td><?=isfun("wddx_add_vars")?></td>
			</tr>
			<tr>
				<td>压缩档支持(Zlib)</td>
				<td><?=isfun("gzclose")?></td>
				<td>XML解析</td>
				<td><?=isfun("xml_set_object")?></td>
			</tr>
			<tr>
				<td>FTP</td>
				<td><?=isfun("ftp_login")?></td>
				<td>ODBC数据库连接</td>
				<td><?=isfun("odbc_close")?></td>
			</tr>
			<tr>
				<td>Session支持</td>
				<td><?=isfun("session_start")?></td>
				<td>Socket支持</td>
				<td><?=isfun("socket_accept")?></td>
			</tr>
		</table>
<!-- ============================================================= 
服务器性能检测 
============================================================== -->
		<table width="100%" cellpadding="0" cellspacing="1" border="0" id="m4">
			<tr>
				<th colspan="4"><p>
						<a href="#top" class="arrow">5</a>
						<br />
						<a href="#bottom" class="arrow">6</a>
					</p>
					<span>8</span>服务器性能检测
					<a name="sec4" id="sec4"></a>
				</th>
			</tr>
			<tr class="th2" >
				<th>检测对象</th>
				<th>整数运算能力测试<br />
					(1+1运算300万次)</th>
				<th>浮点运算能力测试<br />
					(开平方300万次)</th>
				<th>资料I/O能力测试<br />
					(读取10K文件10000次)</th>
			</tr>
			<tr>
				<td>Tahiti 的计算机(P4 1.7G 256M WinXP)</td>
				<td> 1.421秒</td>
				<td> 1.358秒</td>
				<td> 0.177秒</td>
			</tr>
			<tr>
				<td>PIPNI免费空间(2004/06/28 02:08)</td>
				<td> 2.545秒</td>
				<td> 2.545秒</td>
				<td>0.171秒 </td>
			</tr>
			<tr>
				<td>神话科技风CGI型(2004/06/28 02:03)</td>
				<td> 0.797秒</td>
				<td> 0.729秒</td>
				<td>0.156秒</td>
			</tr>
			<tr>
				<td>您正在使用的这台服务器</td>
				<td><b>
					<?=$valInt?>
					</b><br />
					<input type="submit" value="测试1" class="myButton"  name="act" /></td>
				<td><b>
					<?=$valFloat?>
					</b><br />
					<input type="submit" value="测试2" class="myButton"  name="act" /></td>
				<td><b>
					<?=$valIo?>
					</b><br />
					<input type="submit" value="测试3" class="myButton"  name="act" /></td>
			</tr>
		</table>
<!-- ============================================================= 
自定义检测 
============================================================== -->
<?php
    $isMysql = (false !== function_exists("mysql_query"))?"":" disabled";
    $isMail = (false !== function_exists("mail"))?"":" disabled";
?>
		<table width="100%" border="0" cellspacing="1" cellpadding="0">
			<tr>
				<th colspan="4"><p>
						<a href="#top" class="arrow">5</a>
						<br />
						<a href="#bottom" class="arrow">6</a>
					</p>
					<span>8</span>自定义检测
					<a name="sec5" id="sec5"></a>
				</th>
			</tr>
			<tr>
				<th colspan="4" class="th3">MYSQL连接测试</th>
			</tr>
			<tr>
				<td>MYSQL服务器</td>
				<td><input type="text" name="mysqlHost" value="localhost" <?=$isMysql?> /></td>
				<td> MYSQL用户名 </td>
				<td><input type="text" name="mysqlUser" <?=$isMysql?> /></td>
			</tr>
			<tr>
				<td> MYSQL用户密码 </td>
				<td><input type="text" name="mysqlPassword" <?=$isMysql?> /></td>
				<td> MYSQL数据库名称 </td>
				<td><input type="text" name="mysqlDb" />
					&nbsp;<input type="submit" class="myButton" value="CONNECT" <?=$isMysql?>  name="act" /></td>
			</tr>
			<?php if("show"==$mysqlReShow){?>
			<tr>
				<td colspan="4"><?=$mysqlRe?></td>
			</tr>
			<?}?>
			<tr>
				<th colspan="4" class="th3">MAIL邮件发送测试</th>
			</tr>
			<tr>
				<td>收信地址</td>
				<td colspan="3"><input type="text" name="mailReceiver" size="50" <?=$isMail?> />
					&nbsp;<input type="submit" class="myButton" value="SENDMAIL" <?=$isMail?>  name="act" /></td>
			</tr>
			<?php if("show"==$mailReShow){?>
			<tr>
				<td colspan="4"><?=$mailRe?></td>
			</tr>
			<?}?>
			<tr>
				<th colspan="4" class="th3">函数支持状况</th>
			</tr>
			<tr>
				<td>函数名称</td>
				<td colspan="3"><input type="text" name="funName" size="50" />
					&nbsp;<input type="submit" class="myButton" value="FUNCTION_CHECK" name="act" /></td>
				<?php if("show"==$funReShow){?>
			<tr>
				<td colspan="4"><?=$funRe?></td>
			</tr>
			<?}?>
			</tr>
			
			<tr>
				<th colspan="4" class="th3">PHP配置参数状况</th>
			</tr>
			<tr>
				<td>参数名称</td>
				<td colspan="3"><input type="text" name="opName" size="40" />
					&nbsp;<input type="submit" class="myButton" value="CONFIGURATION_CHECK" name="act" /></td>
			</tr>
			<?php if("show"==$opReShow){?>
			<tr>
				<td colspan="4"><?=$opRe?></td>
			</tr>
			<?}?>
		</table>
<!-- ============================================================= 
页脚  
============================================================== -->
		<div id="footer">
			<p id="f1">
				<a name="bottom"></a>
				<a href="#top" class="arrow">55</a>
			</p>
			<table  border="0" cellspacing="1" cellpadding="0" id="f2">
				<tr>
					<td><a href="http://validator.w3.org/check?uri=referer" target="_blank">XHTML</a></td>
					<td><a href="http://jigsaw.w3.org/css-validator/validator?uri=http://<?=$_SERVER['SERVER_NAME'].($_SERVER[PHP_SELF] ? $_SERVER[PHP_SELF] : $_SERVER[SCRIPT_NAME]);?>" target="_blank">CSS</a>
					</td>
				</tr>
			</table>
			<table  border="0" cellspacing="1" cellpadding="0" id="f3">
				<tr>
					<td id="f31">Powered by</td>
					<td id="f32"><a href="http://www.depoch.net"><b>dEpoch Studio</b></a>
					</td>
				</tr>
			</table>
		</div>
	</div>
</form>
</body>
</html>
<?php
/*=============================================================
    函数库
=============================================================*/
/*-------------------------------------------------------------------------------------------------------------
    检测函数支持
--------------------------------------------------------------------------------------------------------------*/
    function isfun($funName)
    {
        return (false !== function_exists($funName))?YES:NO;
    }
/*-------------------------------------------------------------------------------------------------------------
    检测PHP设置参数
--------------------------------------------------------------------------------------------------------------*/
    function getcon($varName)
    {
        switch($res = get_cfg_var($varName))
        {
            case 0:
            return NO;
            break;
            case 1:
            return YES;
            break;
            default:
            return $res;
            break;
        }
         
    }
/*-------------------------------------------------------------------------------------------------------------
    整数运算能力测试
--------------------------------------------------------------------------------------------------------------*/
    function 测试int()
    {
        $timeStart = gettimeofday();
        for($i = 0; $i < 3000000; $i++);
        {
            $t = 1+1;
        }
        $timeEnd = gettimeofday();
        $time = ($timeEnd["usec"]-$timeStart["usec"])/1000000+$timeEnd["sec"]-$timeStart["sec"];
        $time = round($time, 3)."秒";
        return $time;
    }
/*-------------------------------------------------------------------------------------------------------------
    浮点运算能力测试
--------------------------------------------------------------------------------------------------------------*/
    function 测试float()
    {
        $t = pi();
        $timeStart = gettimeofday();
        for($i = 0; $i < 3000000; $i++);
        {
            sqrt($t);
        }
        $timeEnd = gettimeofday();
        $time = ($timeEnd["usec"]-$timeStart["usec"])/1000000+$timeEnd["sec"]-$timeStart["sec"];
        $time = round($time, 3)."秒";
        return $time;
    }
/*-------------------------------------------------------------------------------------------------------------
    资料IO能力测试
--------------------------------------------------------------------------------------------------------------*/
    function 测试io()
    {
        $fp = fopen(PHPSELF, "r");
        $timeStart = gettimeofday();
        for($i = 0; $i < 10000; $i++)
        {
            fread($fp, 10240);
            rewind($fp);
        }
        $timeEnd = gettimeofday();
        fclose($fp);
        $time = ($timeEnd["usec"]-$timeStart["usec"])/1000000+$timeEnd["sec"]-$timeStart["sec"];
        $time = round($time, 3)."秒";
        return($time);
    }
/*-------------------------------------------------------------------------------------------------------------
    比例条
--------------------------------------------------------------------------------------------------------------*/
    function bar($percent)
    {
    ?>
<ul class="bar">
	<li style="width:<?=$percent?>%">&nbsp;</li>
</ul>
<?php
    }
/*-------------------------------------------------------------------------------------------------------------
    系统参数探测 LINUX
--------------------------------------------------------------------------------------------------------------*/
    function sys_linux()
    {
        // CPU
        if (false === ($str = @file("/proc/cpuinfo"))) return false;
        $str = implode("", $str);
        @preg_match_all("/model\s+name\s{0,}\:+\s{0,}([\w\s\)\(.]+)[\r\n]+/", $str, $model);
        //@preg_match_all("/cpu\s+MHz\s{0,}\:+\s{0,}([\d\.]+)[\r\n]+/", $str, $mhz);
        @preg_match_all("/cache\s+size\s{0,}\:+\s{0,}([\d\.]+\s{0,}[A-Z]+[\r\n]+)/", $str, $cache);
        if (false !== is_array($model[1]))
            {
            $res['cpu']['num'] = sizeof($model[1]);
            for($i = 0; $i < $res['cpu']['num']; $i++)
            {
                $res['cpu']['detail'][] = "类型：".$model[1][$i]." 缓存：".$cache[1][$i];
            }
            if (false !== is_array($res['cpu']['detail'])) $res['cpu']['detail'] = implode("<br />", $res['cpu']['detail']);
            }
         
         
        // UPTIME
        if (false === ($str = @file("/proc/uptime"))) return false;
        $str = explode(" ", implode("", $str));
        $str = trim($str[0]);
        $min = $str / 60;
        $hours = $min / 60;
        $days = floor($hours / 24);
        $hours = floor($hours - ($days * 24));
        $min = floor($min - ($days * 60 * 24) - ($hours * 60));
        if ($days !== 0) $res['uptime'] = $days."天";
        if ($hours !== 0) $res['uptime'] .= $hours."小时";
        $res['uptime'] .= $min."分钟";
         
        // MEMORY
        if (false === ($str = @file("/proc/meminfo"))) return false;
        $str = implode("", $str);
        preg_match_all("/MemTotal\s{0,}\:+\s{0,}([\d\.]+).+?MemFree\s{0,}\:+\s{0,}([\d\.]+).+?SwapTotal\s{0,}\:+\s{0,}([\d\.]+).+?SwapFree\s{0,}\:+\s{0,}([\d\.]+)/s", $str, $buf);
         
        $res['memTotal'] = round($buf[1][0]/1024, 2);
        $res['memFree'] = round($buf[2][0]/1024, 2);
        $res['memUsed'] = ($res['memTotal']-$res['memFree']);
        $res['memPercent'] = (floatval($res['memTotal'])!=0)?round($res['memUsed']/$res['memTotal']*100,2):0;
         
        $res['swapTotal'] = round($buf[3][0]/1024, 2);
        $res['swapFree'] = round($buf[4][0]/1024, 2);
        $res['swapUsed'] = ($res['swapTotal']-$res['swapFree']);
        $res['swapPercent'] = (floatval($res['swapTotal'])!=0)?round($res['swapUsed']/$res['swapTotal']*100,2):0;
         
        // LOAD AVG
        if (false === ($str = @file("/proc/loadavg"))) return false;
        $str = explode(" ", implode("", $str));
        $str = array_chunk($str, 3);
        $res['loadAvg'] = implode(" ", $str[0]);
         
        return $res;
    }
/*-------------------------------------------------------------------------------------------------------------
    系统参数探测 FreeBSD
--------------------------------------------------------------------------------------------------------------*/
    function sys_freebsd()
    {
        //CPU
        if (false === ($res['cpu']['num'] = get_key("hw.ncpu"))) return false;
        $res['cpu']['detail'] = get_key("hw.model");
         
        //LOAD AVG
        if (false === ($res['loadAvg'] = get_key("vm.loadavg"))) return false;
        $res['loadAvg'] = str_replace("{", "", $res['loadAvg']);
        $res['loadAvg'] = str_replace("}", "", $res['loadAvg']);
         
        //UPTIME
        if (false === ($buf = get_key("kern.boottime"))) return false;
        $buf = explode(' ', $buf);
        $sys_ticks = time() - intval($buf[3]);
        $min = $sys_ticks / 60;
        $hours = $min / 60;
        $days = floor($hours / 24);
        $hours = floor($hours - ($days * 24));
        $min = floor($min - ($days * 60 * 24) - ($hours * 60));
        if ($days !== 0) $res['uptime'] = $days."天";
        if ($hours !== 0) $res['uptime'] .= $hours."小时";
        $res['uptime'] .= $min."分钟";
         
        //MEMORY
        if (false === ($buf = get_key("hw.physmem"))) return false;
        $res['memTotal'] = round($buf/1024/1024, 2);
        $buf = explode("\n", do_command("vmstat", ""));
        $buf = explode(" ", trim($buf[2]));
         
        $res['memFree'] = round($buf[5]/1024, 2);
        $res['memUsed'] = ($res['memTotal']-$res['memFree']);
        $res['memPercent'] = (floatval($res['memTotal'])!=0)?round($res['memUsed']/$res['memTotal']*100,2):0;
		         
        $buf = explode("\n", do_command("swapinfo", "-k"));
        $buf = $buf[1];
        preg_match_all("/([0-9]+)\s+([0-9]+)\s+([0-9]+)/", $buf, $bufArr);
        $res['swapTotal'] = round($bufArr[1][0]/1024, 2);
        $res['swapUsed'] = round($bufArr[2][0]/1024, 2);
        $res['swapFree'] = round($bufArr[3][0]/1024, 2);
        $res['swapPercent'] = (floatval($res['swapTotal'])!=0)?round($res['swapUsed']/$res['swapTotal']*100,2):0;
         
        return $res;
    }
     
/*-------------------------------------------------------------------------------------------------------------
    取得参数值 FreeBSD
--------------------------------------------------------------------------------------------------------------*/
function get_key($keyName)
    {
        return do_command('sysctl', "-n $keyName");
    }
     
/*-------------------------------------------------------------------------------------------------------------
    确定执行文件位置 FreeBSD
--------------------------------------------------------------------------------------------------------------*/
    function find_command($commandName)
    {
        $path = array('/bin', '/sbin', '/usr/bin', '/usr/sbin', '/usr/local/bin', '/usr/local/sbin');
        foreach($path as $p)
        {
            if (@is_executable("$p/$commandName")) return "$p/$commandName";
        }
        return false;
    }
     
/*-------------------------------------------------------------------------------------------------------------
    执行系统命令 FreeBSD
--------------------------------------------------------------------------------------------------------------*/
    function do_command($commandName, $args)
    {
        $buffer = "";
        if (false === ($command = find_command($commandName))) return false;
        if ($fp = @popen("$command $args", 'r'))
            {
				while (!@feof($fp))
				{
					$buffer .= @fgets($fp, 4096);
				}
				return trim($buffer);
			}
        return false;
    }

?>
                                                                                              
                                                                                              
