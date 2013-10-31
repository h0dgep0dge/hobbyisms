<?php
$_GET['path'] = preg_replace('%/\z%m','',$_GET['path']);
$filetypes = Array('png','jpeg','jpg','png','gif');
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="main.css" />
<title>RFile Image Viewer - <?php echo $_GET['file'] ?></title>
</head>
<body>
<h3 align='center'>RFile Image Viewer</h3>
<center><?php echo $_GET['path'].'/'.$_GET['file'] ?></center>
<?php
$name_array = explode('.',$_GET['file']);
$chunks = count($name_array)-1;
if(!file_exists($_GET['path'].'/'.$_GET['file']))
{
echo 'Image does not exist';
}
else
{
if(in_array($name_array[$chunks],$filetypes))
{
echo '<center><img width=100%" src="display.php?path='.$_GET['path'].'&file='.$_GET['file'].'" /></center>';
}
else
{
echo 'This file does not apear to be a valid image';
}
}
?>
</body>
</html>