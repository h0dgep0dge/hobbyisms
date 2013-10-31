<?php
include('./handle.php');
$h = new db_handle();
if($h->is_logged_in() === false) die('You are not logged in');
if(!isset($_POST['id'])) die('Bad input');
$d = 0;
for($i = 0;$i < count($_POST['id']);$i++) if($h->del_message($_POST['id'][$i]) === true) $d += 1;
echo strval($d).' Messages Deleted. <a href=\'./\'>Back</a>';
?>