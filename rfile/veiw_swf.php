<?php
$_GET['path'] = preg_replace('%/\z%m','',$_GET['path']);
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="main.css" />
<title>RFile Flash Player - <?php echo $_GET['file'] ?></title>
</head>
<body>
<h3 align='center'>RFile Flash Player</h3>
<center><?php echo $_GET['path'].'/'.$_GET['file'] ?></center>
<?php
$name_array = explode('.',$_GET['file']);
$chunks = count($name_array)-1;
if(!file_exists($_GET['path'].'/'.$_GET['file']))
{
echo 'SWF does not exist';
}
else
{
if(strtolower($name_array[$chunks]) == 'swf')
{
echo '<object><param name=movie value="display.php?path='.$_GET['path'].'&file='.$_GET['file'].'"><embed src="display.php?path='.$_GET['path'].'&file='.$_GET['file'].'" type="application/x-shockwave-flash" width="100%" height="100%"></embed></object>';
}
else
{
echo 'This file does not apear to be a valid flash document';
}
}
?>
</body>
</html>