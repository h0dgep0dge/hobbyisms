<?php
if(!isset($_GET['path']) || !is_dir($_GET['path']))
{
$_GET['path'] = getcwd();
}
$_GET['path'] = preg_replace('%/\z%m','',$_GET['path']);
$directories = Array();
$files		= Array();
$_GET['path'] = str_replace('\\','/',$_GET['path']);
$image_filetypes = Array('png','jpeg','jpg','png','gif');
$html_filetypes = Array('htm','html','txt','java');
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="main.css" />
<title><?php echo $_GET['path'].'/' ?></title>
</head>
<body>
<h3 align='center'>RFile PhP Browser</h3>
<center>
<?php
$alphabet = Array('A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z');

for($i = 0;$i <= 25;$i++)
{
if($alphabet[$i] == 'E') continue; // ugly dirty hack
$drive = $alphabet[$i].':/';
if(is_dir($drive)) echo '<a href="./?path='.$drive.'">[ '.$drive.' ]</a>';
}
?>
</center>
<?php
if(!($directory = @opendir($_GET['path'].'/'))) die('<h3>Cannot Read Directory</h3>');
while($file = readdir($directory))
{
if($file != '.' && $file != '..')
{
if(is_file($_GET['path'].'/'.$file))
{
$files[] = $file;
}
else
{
$directories[] = $file;
}
}
}

$path_array = explode('/',$_GET['path']);
$levels = count($path_array)-1;
$path_so_far = '';
echo '<b>';
foreach($path_array as $level => $dir)
{
if($level != $levels)
{
$path_so_far = $path_so_far.$dir.'/';
echo '<a href="./?path='.$path_so_far.'">'.$dir.'/</a>';
}
else
{
echo $dir.'/';
}
}

echo '</b><table width="100%">';
sort($directories);
sort($files);
foreach($directories as $dir)
{
echo '<tr class="list">
<td colspan="3" width="100%"><a href="./?path='.$_GET['path'].'/'.$dir.'">'.$dir.'</a></td>

<td><a href="del.php?path='.$_GET['path'].'&file='.$dir.'"><img src="images/del.png" /></a></td>
</tr>';
}

foreach($files as $file)
{
echo '<tr class="list">';
$name_array = explode('.',$file);
$chunks = count($name_array)-1;
if(in_array(strtolower($name_array[$chunks]),$image_filetypes))
{
echo '<td width="100%"><a href="view_image_array.php?path='.$_GET['path'].'&file='.$file.'">'.$file.'</a></td>';
}
elseif(in_array(strtolower($name_array[$chunks]),$html_filetypes))
{
echo '<td width="100%"><a href="display.php?path='.$_GET['path'].'&file='.$file.'">'.$file.'</a></td>';
}
else
{
echo '<td width="100%">'.$file.'</td>';
}
echo '<td><a href="dl.php?path='.$_GET['path'].'&file='.$file.'"><img src="images/dl.png" /></a></td>
<td><a href="edit.php?path='.$_GET['path'].'&file='.$file.'"><img src="images/edit.png" /></a></td>
<td><a href="del.php?path='.$_GET['path'].'&file='.$file.'"><img src="images/del.png" /></a></td>
</tr>';
}
?>
</table>

<center>
<table>
<tr>
<td width='300'>
<b>Create a File</b><br>
<form action='edit.php' method='get'>
<input type='text' name='file'>
<input type='hidden' name='path' value='<?php echo $_GET['path'] ?>'>
<input type='submit' value='Create'>
</form>
</td>
<td width='300'>
<b>Create a Directory</b><br>
<form action='mkdir.php' method='get'>
<input type='text' name='file'>
<input type='hidden' name='path' value='<?php echo $_GET['path'] ?>'>
<input type='submit' value='Create'>
</form>
</td>
</tr>
<tr>
<td width='300'>
<b>Upload a File</b><br>
<form enctype="multipart/form-data" action='upload.php' method='post'>
<input type='file' name='file'><br>
<input type='hidden' name='path' value='<?php echo $_GET['path'] ?>'>
<input type='submit' value='Upload'>
</form>
</td>
<td width='300'>
<b>Execute a Short Command</b><br>
<form action='execute.php' method='get'>
<input type='input' name='command'>
<input type='hidden' name='path' value='<?php echo $_GET['path'] ?>'>
<input type='submit' value='Execute'>
</form>
</tr>
</table>
</center>
</body>
</html>