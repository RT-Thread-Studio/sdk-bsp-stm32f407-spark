<?php 
  $name=$_POST['user_name']; 
  $content=$_POST['user_post']; 
  if ($name<>"" and $content<>"")
  {
   include_once("include/conn.php");
   $rs = $conn->Execute("insert into contents (name,content) values ('$name','$content')"); 
   header("location:index.php");
   }
   else 
   echo "<script language='javascript'>alert('×Ö¶Î²»ÄÜÓÐ¿Õ£¡');location='index.php';</script>";
?>
