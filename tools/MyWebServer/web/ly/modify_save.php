<?
 session_start();
 if($_SESSION['admin']=="OK")
 {
  include_once("include/conn.php");
  $sql="update contents set content='".$_POST['post_contents']."' where id=".$_POST['id'];
  $rs = $conn->Execute($sql);
  }
 header("location:admin_index.php");
?>
