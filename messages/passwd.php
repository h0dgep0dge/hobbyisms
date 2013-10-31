<?php
include('./handle.php');
$h = new db_handle();
if($h->is_logged_in() === false) die('You are not logged in');
if(!isset($_POST['pass1']) && !isset($_POST['pass2'])) {
?>
<html>
  <head>
    <title>Change password</title>
  </head>
  <body>
    <form method='post'>
    <table>
	  <tr>
	    <td>Password:</td>
		<td><input type='password' name='pass1' /></td>
	  </tr>
	  <tr>
	    <td>Confirm Password:</td>
		<td><input type='password' name='pass2' /></td>
	  </tr>
	  <tr>
	    <td></td>
		<td><input type='submit' value='Change' /></td>
	  </tr>
	</table>
	</form>
  </body>
</html>
<?php
}
elseif(!isset($_POST['pass1']) || !isset($_POST['pass2'])) die('Input error');
elseif(empty($_POST['pass1']) || empty($_POST['pass2'])) header('Location: ./passwd.php?err');
else {
	if($_POST['pass1'] != $_POST['pass2']) die('Passwords do not match');
	if($h->change_password($_POST['pass1'])) echo 'Password changed. <a href=\'./\'>Back</a>';
	else echo 'Unknown error';
}
?>