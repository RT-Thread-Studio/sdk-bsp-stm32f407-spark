<?php
/*清除session*/
session_start();
session_destroy();
header("location:admin_index.php");
//返回admin_index.php是判断session是否清除成功，成功则返回index.php
?>