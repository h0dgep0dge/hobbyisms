<?php
include('./handle.php');
$h = new db_handle();
if($h->is_logged_in() !== false) die('You are already logged in');
if(!isset($_POST['username']) || !isset($_POST['password'])) die('Bad input');
if($_POST['username'] == '' || $_POST['password'] == '') header('Location: ./?fill');
if($h->log_in($_POST['username'],$_POST['password'])) header('Location: ./');
else header('Location: ./?inc');
?>