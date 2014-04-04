<?php
if(!isset($_GET['target'])) die('This should give a graphical error. Fix that.');
if(!isset($_GET['bound'])) die('This should give a graphical error. Fix that.');

$target = $_GET['target'];

if(strtoupper($_GET['bound']) == 'IN') $table = 'inbound';
else if(strtoupper($_GET['bound']) == 'OUT') $table = 'outbound';
else die('This should give a graphical error. Fix that.');

$rates = Array();
for($i = 0;$i < 720;$i++) $rates[$i] = 0;

$dbconn = pg_connect("host=localhost dbname=bandwidth_monitor user=postgres") or die('Could not connect: '.pg_last_error());

$query = 
'select
    (extract(epoch from time) / 120)::integer,
	(extract(epoch from TIMESTAMP \'now\') / 120)::integer,
    sum(size) as size
from traffic_log_'.$table.'
where
    ip=inet \''.$target.'\'
    and
    time > current_timestamp - interval \'24 hours\'
group by 1
order by 1 DESC;';

@$result = pg_query($query) or die('Query failed: '.pg_last_error());
while(($row = pg_fetch_array($result,NULL,PGSQL_NUM)) !== FALSE) {
	$rates[intval($row[1])-intval($row[0])] = $row[2]/120;
}
pg_free_result($result);
pg_close($dbconn);

$graphx = 720;
$graphy = 200;
$im = imagecreatetruecolor($graphx,$graphy);
imagefill($im,0,0,imagecolorallocate($im,255,255,255));
for($i = 0;$i < 720;$i++){
	if(max($rates) > 0) imageline($im,$graphx-1-$i,$graphy-1,$graphx-1-$i,round($graphy-1-($rates[$i]*(($graphy-10)/max($rates)))),imagecolorallocate($im,0,0,0));
	else imageline($im,$graphx-1-$i,$graphy-1,$graphx-1-$i,$graphy-1,imagecolorallocate($im,0,0,0));
	// echo ($inbound[$i]*($graphy/max($inbound))),'<br>';
	// echo $inbound[$i],'<br>';
}
header('Content-type: image/png');
imagepng($im);
imagedestroy($im);
?>