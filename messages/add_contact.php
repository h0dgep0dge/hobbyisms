<?php
include('./handle.php');
$h = new db_handle();
if($h->is_logged_in() === false) die('You are not logged in');
if(!isset($_GET['id']) || !isset($_GET['r'])) die('Bad input');
if($h->add_contact($_GET['id'])) header('Location: ./read.php?id='.$_GET['r']);
else echo 'ERROR';
?>