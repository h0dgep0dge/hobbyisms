<?php
if(file_exists($_GET['path'].'/'.$_GET['file']))
{
$_GET['path'].'/'.$_GET['file']
}
mkdir($_GET['path'].'/'.$_GET['file']);
header('Location: ./?path='.$_GET['path']);
?>