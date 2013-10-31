<?php
include('./handle.php');
$h = new db_handle();
if($h->is_logged_in() === false) die('You are not logged in');
$h->log_out();
header('Location: ./');
?>