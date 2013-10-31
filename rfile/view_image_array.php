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
<a href='./?path=<?php echo $_GET['path'] ?>'><h4>Back to folder</h4></a>
<center><?php echo $_GET['path'].'/'.$_GET['file'] ?></center>
<?php
$name_array = explode('.',$_GET['file']);
$chunks = count($name_array)-1;

if(!in_array(strtolower($name_array[$chunks]),$filetypes))
{
die('This does not apear to be a valid image');
}
if(!file_exists($_GET['path'].'/'.$_GET['file']))
{
echo 'Image does not exist';
}
else
{
if(!($directory = @opendir($_GET['path'].'/'))) die('<h3>Cannot Read Directory</h3>');
while($file = readdir($directory))
{
if($file != '.' && $file != '..')
{
if(is_file($_GET['path'].'/'.$file))
{
$name_array = explode('.',$file);
$chunks = count($name_array)-1;
if(in_array(strtolower($name_array[$chunks]),$filetypes))
{
$files[] = $file;
}
}
}
}
sort($files);
$picture_key = array_search($_GET['file'],$files);
$file_count = count($files)-1;
echo '<center>';
if(strval($picture_key) == '0')
{
echo 'Prev / <a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key+1].'">Next</a>';
}
elseif(strval($picture_key) == strval($file_count))
{
echo '<a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key-1].'">Prev</a> / Next';
}
else
{
echo '<a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key-1].'">Prev</a> / <a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key+1].'">Next</a>';
}

if(intval($picture_key) >= 0 && intval($picture_key) < intval($file_count))
{
echo '<a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key+1].'"><center><img width=100%" src="display.php?path='.$_GET['path'].'&file='.$_GET['file'].'" /></center></a>';
}
else
{
echo '<center><img width=100%" src="display.php?path='.$_GET['path'].'&file='.$_GET['file'].'" /></center>';
}
if(strval($picture_key) == '0')
{
echo 'Prev / <a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key+1].'">Next</a>';
}
elseif(strval($picture_key) == strval($file_count))
{
echo '<a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key-1].'">Prev</a> / Next';
}
else
{
echo '<a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key-1].'">Prev</a> / <a href="view_image_array.php?path='.$_GET['path'].'&file='.$files[$picture_key+1].'">Next</a>';
}
echo '</center>';
}
?>
</body>
</html>