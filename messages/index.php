<?php
include('./handle.php');
$h = new db_handle();
if($h->is_logged_in() === false) {
?>
<html>
  <head>
    <title>Login</title>
  </head>
  <body>
    <form method='post' action='./login.php'>
	  <?php if(isset($_GET['fill'])) echo 'No fields may be empty<br>'; ?>
	  <?php if(isset($_GET['inc'])) echo 'Incorrect username or password<br>'; ?>
	  <table>
        <tr>
          <td>Username:</td>
          <td><input type='text' name='username' /></td>
        </tr>
        <tr>
		  <td>Password:</td>
		  <td><input type='password' name='password' /></td>
		</tr>
		<tr>
          <td></td>
          <td><input type='submit' value='Log In' /></td>
		</tr>
		<tr>
          <td></td>
          <td><a href='reg.php'>Register</a></td>
		</tr>
	  </table>
	</form>
  </body>
</html>
<?php
}
else {
	if(isset($_GET['sent'])) $m = $h->get_sent_messages();
	else $m = $h->get_messages();
	echo '<a href=\'send.php\'>Compose new message</a> | ';
	if(isset($_GET['sent'])) echo '<a href=\'./\'>See recieved messages</a><br>';
	else echo '<a href=\'./?sent\'>See sent messages</a><br>';
	if(count($m) == 0) echo 'No messages<br>';
	else {
	echo '<form action=\'del.php\' method=\'post\'>';
	echo '<table border=\'1\'><tr><td></td><td><b>Sender</b></td><td><b>Subject</b></td><td><b>Time</b></td></tr>';
	for($i = 0;$i < count($m);$i++) {
		echo '<tr><td>';
		echo '<input type=\'checkbox\' name=\'id[]\' value=\''.strval($m[$i][0]).'\' />';
		echo '</td><td>';
		echo $h->id_to_user($m[$i][2]);
		echo '</td><td>';
		echo '<a href=\'read.php?id='.strval($m[$i][0]).'\'>'.$m[$i][3].'</a>';
		echo '</td><td>';
		echo date("j/n g:i A",$m[$i][1]); // D/M H:Mi (A|P)M "j/n g:i A"
		echo '</td></tr>';
	}
	echo '</table>';
	echo '<input type=\'submit\' value=\'Delete selected messages\' />';
	echo '</form>';
	}
	echo '<a href=\'logout.php\'>Log Out</a> | <a href=\'passwd.php\'>Change Password</a>';
	$id = $h->is_logged_in();
}




?>