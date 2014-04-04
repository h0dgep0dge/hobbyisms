<?php
function si($num) {
	$prefixes = Array('B','KB','MB','GB','TB');
	$divs = 0;
	while(bccomp($num,'1024',5) == 1) {
		$num = bcdiv($num,'1024',5);
		$divs++;
	}
	return $num.' '.$prefixes[$divs];
}

$dbconn = pg_connect("host=localhost dbname=bandwidth_monitor user=postgres") or die('Could not connect: '.pg_last_error());

$throughput = Array();

// $query = 'SELECT ip,sum(size) as size FROM traffic_log_inbound WHERE time > current_timestamp - interval \'24 hours\' GROUP BY 1;';
$query = 'SELECT * FROM traffic_log_inbound_daily_cache;';
@$result = pg_query($query) or die('Query failed: '.pg_last_error());
while(($row = pg_fetch_array($result,NULL,PGSQL_NUM)) !== FALSE) $throughput[$row[0]] = Array(intval($row[1]),0);
pg_free_result($result);

// $query = 'SELECT ip,sum(size) as size FROM traffic_log_outbound WHERE time > current_timestamp - interval \'24 hours\' GROUP BY 1;';
$query = 'SELECT * FROM traffic_log_outbound_daily_cache;';
@$result = pg_query($query) or die('Query failed: '.pg_last_error());
while(($row = pg_fetch_array($result,NULL,PGSQL_NUM)) !== FALSE) {
	if(!isset($throughput[$row[0]])) $throughput[$row[0]] = Array(0,intval($row[1]));
	$throughput[$row[0]][1] = intval($row[1]);
}
pg_free_result($result);
pg_close($dbconn);

echo '<table border=\'1\'>';
echo '	<tr><th>IP</th><th>In (SI)</th><th>Out (SI)</th><th>In</th><th>Out</th></tr>',PHP_EOL;
foreach($throughput as $ip => $data) {
	echo '	<tr><td>',$ip,'</td><td>',si($data[0]),'</td><td>',si($data[1]),'</td><td>',$data[0],'</td><td>',$data[1],'</td></tr>',PHP_EOL;
}
echo '</table>';
?>