<?php
include('./handle.php');
$h = new db_handle();
function make_list($users) {
	echo '<select size=\'15\' MULTIPLE>';
	for($i = 0;$i < count($users);$i++) {
		echo '<option onDblClick=\'set_to("'.$users[$i].'")\'>'.$users[$i].'</option>';
	}
	echo '</select>';
}
function add_re($sub) {
	if(preg_match('/RE: .+/sm',$sub)) return $sub;
	return 'RE: '.$sub;
}
if($h->is_logged_in() === false) die('You are not logged in');
if(!isset($_POST['to']) && !isset($_POST['subject']) && !isset($_POST['body']) && !isset($_FILES['attachment'])) {
	if(isset($_GET['r'])) {
		$m = $h->get_message($_GET['r']);
		if($m === false) unset($m);
	}
?>
<html>
  <head>
    <title>Send</title>
	<script language='javascript'>
	  function set_to(name) {
	    document.getElementById('to').value = name;
	  }
	</script>
  </head>
  <body>
    <form action='./send.php' method='post' enctype='multipart/form-data'>
	  <?php if(isset($_GET['err'])) echo 'No fields may be empty'; ?>
      <table border='1'>
	    <tr>
	      <td>
		    To
		  </td>
		  <td>
		    <input type='text' name='to' id='to' <?php if(isset($m)) echo 'value=\''.$h->id_to_user($m[3]).'\''; ?> size='66' />
		  </td>
		  <td>
		    Contacts
		  </td>
	    </tr>
	    <tr>
	      <td>
		    Subject
		  </td>
		  <td>
		    <input type='text' name='subject' <?php if(isset($m)) echo 'value=\''.str_replace('\'','&#39;',add_re($m[4])).'\''; ?> size='66' />
		  </td>
		  <td rowspan='3'>
		    <?php make_list($h->get_contacts()); ?>
		  </td>
	    </tr>
		<tr>
	      <td>
		    Body
		  </td>
		  <td>
		    <textarea name='body' cols='50' rows='10' wrap='hard'></textarea>
		  </td>
	    </tr>
		<tr>
	      <td>
		    Attachment
		  </td>
		  <td>
		    <input type='file' name='attachment' size='54' />
		  </td>
	    </tr>
	  </table>
	  <a href='./'>Back</a>
	  <input type='submit' value='Send' />
	</form>
  </body>
</html>
<?php
}
elseif(!isset($_POST['to']) || !isset($_POST['subject']) || !isset($_POST['body'])) die('Input Error');
elseif(empty($_POST['to']) || empty($_POST['body'])) header('Location: ./send.php?err');
else {
	$_POST['body'] = str_replace(PHP_EOL,'<br>',$_POST['body']);
	if(isset($_FILES['attachment'])) {
		$f = $h->handle_upload($_FILES['attachment']);
	}
	else {
		$f = '';
	}
	$u = $h->username_to_id($_POST['to']);
	if($u === false) die('User does not exist');
	$r = $h->send_message($u,$_POST['subject'],$_POST['body'],$f);
	if($r === false) die('There has been an error');
	else die('Message Sent. <a href=\'./\'>Back</a>');
}
?>