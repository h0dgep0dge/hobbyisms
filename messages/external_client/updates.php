<?php
include('../handle.php');
$h = new db_handle();
if(!isset($_GET['u']) || !isset($_GET['p'])) die('ERR0');
$u = $_GET['u'];
$p = $_GET['p'];
$r = $h->log_in($u,$p);
if($r == false) die('ERR1');
$m = $h->get_messages();
echo count($m);
$h->log_out();
?>