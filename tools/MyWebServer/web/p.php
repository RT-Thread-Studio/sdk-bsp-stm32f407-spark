<form action="" method="POST"  >
<input type="submit" value="提交">
<input type="text" name="tgy" width=20  value="sdf4r4r4r4r">
</form>
提交的数据:<?php echo $_POST['tgy'] ?>
<br>原始数据:<?php echo file_get_contents("php://input");?>