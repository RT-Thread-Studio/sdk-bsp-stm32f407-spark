<? 
  session_start();
  $admin_name=$_POST['admin_name'];
  $admin_password=$_POST['admin_password'];
  include_once("include/conn.php");
  $sql="select * from admin where admin_name='".$admin_name."'"."and admin_password='".$admin_password."'";
  //echo $sql;
  //exit;
  $rs = $conn->Execute($sql);
 
  if (!$rs->EOF)
        {$_SESSION['admin']="OK";
		header("location:admin_index.php");


        }
  else  
	  {
	  $_SESSION['admin']="";
	  echo "<script language='javascript'>alert('用户名或者密码不正确！');location='index.php';</script>";
       }
  $rs->Close();
  $conn->Close();
  $rs = null;
  $conn = null;
?>
