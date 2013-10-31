<?php
function del_folder($folder)
{
$directory = scandir($folder);
foreach($directory as $file)
{
if($file != '.' && $file != '..')
{
if(is_file($folder.'/'.$file))
{
unlink($folder.'/'.$file);
}
else
{
del_folder($folder.'/'.$file);
}
}
}
rmdir($folder);
}


if(is_file($_GET['path'].'/'.$_GET['file']))
{
unlink($_GET['path'].'/'.$_GET['file']);
}
else
{
del_folder($_GET['path'].'/'.$_GET['file']);
}


header('Location: ./?path='.$_GET['path']);
?>