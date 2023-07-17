<? header("Content-type: text/html; charset=gb2312");
error_reporting(0);
ob_start();
$NowPathArray=explode("inc",str_replace("\\","/",dirname(__FILE__)));
@define("root_path", $NowPathArray[0]);
@define("confign_path", root_path . "inc/");
@define("db_path", root_path."inc/data.mdb");
require_once(confign_path.'conn.php'); ?>