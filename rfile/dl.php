<?php
if(!is_file($_GET['path'].'/'.$_GET['file']))
{
header('Location: ./?path='.$_GET['path']);
}


if(isset($_GET['method']) && $_GET['method'] == 'nh')
{
@set_time_limit(0);
$file_handle = fopen($_GET['path'].'/'.$_GET['file'],'r');
while($line = fgets($file_handle))
{
echo $line;
}
fclose($file_handle);
}
elseif(isset($_GET['method']) && $_GET['method'] == 'dh')
{
//header('Content-type: exe');
// header("Content-Type: media/mpeg");

// header('Content-Disposition: attachment; filename="'.$_GET['file'].'"');


header('Content-type: video/mpeg');
header('Content-Length: '.filesize($_GET['path'].'/'.$_GET['file']));
header("Expires: -1");
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);
header('Content-Disposition: inline; filename="'.$_GET['file'].'";');



@set_time_limit(0);
$file_handle = fopen($_GET['path'].'/'.$_GET['file'],'r');
while($line = fgets($file_handle))
{
echo $line;
}
fclose($file_handle);
//readfile($_GET['path'].'/'.$_GET['file']);
}
else
{
?>
<html>
<head>
<title>Download Menu | RFile Browser</title>
<link rel="stylesheet" type="text/css" href="main.css" />
</head>
<body>
<center><h3>RFile Download Menu</h3>
<h4><a href='dl.php?path=<?php echo $_GET['path']; ?>&file=<?php echo $_GET['file']; ?>&method=dh'>Download With Attachment Headers</a></h4>

<h4><a href='dl.php?path=<?php echo $_GET['path']; ?>&file=<?php echo $_GET['file']; ?>&method=nh'>Download Without Any Headers</a></h4>

<a href='./?path=<?php echo $_GET['path']; ?>'>Back</a>
</center>
</body>
</html>
<?php
}
?>