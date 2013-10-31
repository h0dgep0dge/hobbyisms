<?php
$_GET['path'] = preg_replace('%/\z%m','',$_GET['path']);
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="main.css" />
<title>RFile PhP Browser - File Editor</title>
</head>
<body>
<h3 align='center'>RFile PhP Browser - File Editor</h3>
<center><b><h4><?php
if(file_exists($_GET['path'].'/'.$_GET['file']))
{
echo 'Editing File: '.$_GET['path'].'/'.$_GET['file'];
}
else
{
echo 'Creating File: '.$_GET['path'].'/'.$_GET['file'];
}
?></h4></b><br>
<form action='save.php' method='post'>
<textarea cols="100" rows="25" name="data" wrap='off'>
<?php
if(file_exists($_GET['path'].'/'.$_GET['file']))
{
echo htmlentities(file_get_contents($_GET['path'].'/'.$_GET['file']));
}
?>
</textarea>
<input type='hidden' name='file' value='<?php echo $_GET['file']; ?>'>
<input type='hidden' name='path' value='<?php echo $_GET['path']; ?>'>
<br><input type='submit' name='submit' value='Save'>
</center>
</form>
</body>
</html>