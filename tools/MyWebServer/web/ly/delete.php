<?
 session_start();
 if($_SESSION['admin']=="OK")
 {
  include_once("include/conn.php");
  $sql="delete  from contents where id=".$_GET['id'];
  $rs = $conn->Execute($sql);
  header("location:admin_index.php");
 }
?>
