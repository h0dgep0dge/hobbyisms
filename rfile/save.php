<?php
file_put_contents($_POST['path'].'/'.$_POST['file'],$_POST['data']);
header('Location: ./?path='.$_POST['path']);
?>