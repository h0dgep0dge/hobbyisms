<?php
include('./handle.php');
$h = new db_handle();
if(!isset($_POST['username']) && !isset($_POST['pass1']) && !isset($_POST['pass2'])) {
?>
<html>
  <head>
    <title>Register</title>
  </head>
  <body>
    <form method='post'>
	  <?php if(isset($_GET['e']) && $_GET['e'] == 0) echo 'No fields may be empty'; ?>
	  <?php if(isset($_GET['e']) && $_GET['e'] == 1) echo 'Password must match'; ?>
	  <?php if(isset($_GET['e']) && $_GET['e'] == 2) echo 'Usernames taken'; ?>
	  <table>
	    <tr>
		  <td>
		    Username:
		  </td>
		  <td>
		    <input type='text' name='username' />
		  </td>
		</tr>
		<tr>
		  <td>
		    Password:
		  </td>
		  <td>
		    <input type='password' name='pass1' />
		  </td>
		</tr>
		<tr>
		  <td>
		    Confirm Password:
		  </td>
		  <td>
		    <input type='password' name='pass2' />
		  </td>
		</tr>
		<tr>
		  <td></td>
		  <td><input type='submit' value='Register' /></td>
		</tr>
	  </table>
	</form>
  </body>
</html>
<?php
}
elseif(!isset($_POST['username']) || !isset($_POST['pass1']) || !isset($_POST['pass2'])) die('Input error');
elseif(!!empty($_POST['username']) || !!empty($_POST['pass1']) || !!empty($_POST['pass2'])) header('Location: ./reg.php?e=0');
else {
	if($_POST['pass1'] != $_POST['pass2']) header('Location: ./reg.php?e=1');
	if($h->username_to_id($_POST['username']) !== false) header('Location: ./reg.php?e=2');
	if($h->register_account($_POST['username'],$_POST['pass1']) === true) echo 'Account created. Go <a href=\'./\'>here</a> to log in.';
	else echo 'Failed to create account';
}
?>