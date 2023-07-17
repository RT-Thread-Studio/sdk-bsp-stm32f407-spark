<?php
session_start();
if($_SESSION['admin']=="OK")
 {
  include_once("include/conn.php");
  $rs = $conn->Execute('SELECT * FROM contents');
  while(!$rs->EOF)
     {
      echo "<table><tr><td>ÁôÑÔÕßÐÕÃû:".$rs->Fields['name']->Value ."</td></tr>"; 
      echo "<tr><td>ÁôÑÔ:".$rs->Fields['content']->Value ."</td></tr></table><br/>";
      echo  "<a href=modify.php?id=".$rs->Fields['id']->Value ." >ÐÞ¸Ä</a>      <a href=delete.php?id=".$rs->Fields['id']->Value ." >É¾³ý</a>";
	  $rs->MoveNext();
     }
 echo "<br><br><br><br><br><a href=login_out.php>ÍË³ö</a>";
 
 $rs->Close();
 $conn->Close();
 $rs = null;
 $conn = null;
 }
 else
 {
	 header("location:index.php");
 }
?>

