<?php
$_GET['path'] = preg_replace('%/\z%m','',$_GET['path']);
@set_time_limit(0);
$file_handle = fopen($_GET['path'].'/'.$_GET['file'],'r');
while($line = fgets($file_handle))
{
echo $line;
}
fclose($file_handle);
?>