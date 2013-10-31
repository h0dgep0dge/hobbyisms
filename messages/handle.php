<?php
class db_handle {
	private $db_host = 'localhost';
	private $db_user = 'root';
	private $db_pass = '';
	private $db_base = 'messages';
	private $link;
	
	public function __construct() {
		date_default_timezone_set('Pacific/Auckland');
		session_start();
		$this->link = @mysql_connect($this->db_host,$this->db_user,$this->db_pass) or die('MYSql Error on line '. __LINE__);
		mysql_select_db($this->db_base, $this->link) or die('MYSql Error on line '. __LINE__);
	}
	
	public function __deconstruct() {
		@mysql_close($this->link);
	}
	
	public function log_in($user,$pass) {
		$user = mysql_real_escape_string(htmlspecialchars($user),$this->link);
		$pass = sha1($pass);
		$query = 'SELECT * FROM `users` WHERE `username`=\''.$user.'\' AND `password`=\''.$pass.'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_num_rows($results);
		if($r != 1) return false;
		$row = mysql_fetch_row($results);
		$_SESSION['id'] = intval($row[0]);
		$this->last_access();
		#$this->hlog("Logged in");
		return true;
	}
	
	public function is_logged_in() {
		if(!isset($_SESSION['id'])) return false;
		return $_SESSION['id'];
	}
	
	public function log_out() {
		if(isset($_SESSION['id'])) return session_destroy();
		return false;
	}
	
	public function get_messages() {
		if(!isset($_SESSION['id'])) return false;
		$query = 'SELECT `id`,`time`,`from`,`subject` FROM `messages` WHERE `to`=\''.strval($_SESSION['id']).'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = array();
		$row = null;
		while(($row = mysql_fetch_row($results)) !== false) $r[] = $row;
		return $r;
	}
	
	public function id_to_user($id) {
		$query = 'SELECT `username` FROM `users` WHERE `id`=\''.strval($id).'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_num_rows($results);
		if($r != 1) return 'unknown user';
		$row = mysql_fetch_row($results);
		return $row[0];
	}
	
	public function get_message($id) {
		if(!isset($_SESSION['id'])) return false;
		$this->last_access();
		$user = mysql_real_escape_string(strval($id),$this->link);
		$query = 'SELECT * FROM `messages` WHERE `id`=\''.strval($id).'\' AND (`to`=\''.strval($_SESSION['id']).'\' OR `from`=\''.strval($_SESSION['id']).'\')';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_num_rows($results);
		if($r != 1) return false;
		$row = mysql_fetch_row($results);
		return $row;
	}
	
	public function send_message($to,$subject,$message,$attachment) {
		if(!isset($_SESSION['id'])) return false;
		$this->last_access();
		$time = strval(time());
		$to = mysql_real_escape_string(strval($to),$this->link);
		$from = mysql_real_escape_string(strval($_SESSION['id']),$this->link);
		$subject = mysql_real_escape_string($subject,$this->link);
		$message = mysql_real_escape_string($message,$this->link);
		$attachment = mysql_real_escape_string($attachment,$this->link);
		$query = 'INSERT INTO `messages` (`id`, `time`, `to`, `from`, `subject`, `message`,`attachment`) VALUES (NULL, \''.$time.'\', \''.$to.'\', \''.$from.'\', \''.$subject.'\', \''.$message.'\', \''.$attachment.'\')';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_affected_rows($this->link);
		#$this->hlog("Sent a message");
		return ($r == 1);
	}
	
	public function get_users() {
		if(!isset($_SESSION['id'])) return false;
		$query = 'SELECT `id`,`username` FROM `users` WHERE `id`<>\''.$_SESSION['id'].'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = array();
		$row = null;
		while(($row = mysql_fetch_row($results)) !== false) $r[] = $row;
		return $r;
	}
	
	public function del_message($id) {
		if(!isset($_SESSION['id'])) return false;
		$id = mysql_real_escape_string(strval($id),$this->link);
		$query = 'SELECT * FROM `messages` WHERE `id`=\''.$id.'\' AND `to`=\''.$_SESSION['id'].'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_num_rows($results);
		if($r != 1) return false;
		$row = mysql_fetch_row($results);
		$file = $row[6];
		$query = 'DELETE FROM `messages` WHERE `id`=\''.$id.'\' AND `to`=\''.$_SESSION['id'].'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_affected_rows($this->link);
		if($r != 1) return false;
		if($file != '') @unlink('./attachments/'.$file);
		#$this->hlog("Deleted a message");
		return true;
	}
	
	public function add_contact($id) {
		if(!isset($_SESSION['id'])) return false;
		$this->last_access();
		if($this->contact_exists($_SESSION['id'],$id)) return false;
		$id = mysql_real_escape_string(strval($id),$this->link);
		$query = 'INSERT INTO `contacts` (`id`, `o_id`, `c_id`) VALUES (NULL, \''.strval($_SESSION['id']).'\', \''.$id.'\')';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_affected_rows($this->link);
		return ($r == 1);
	}
	
	public function contact_exists($c_id) {
		$c_id = mysql_real_escape_string(strval($c_id),$this->link);
		$query = 'SELECT * FROM `contacts` WHERE `o_id`=\''.$_SESSION['id'].'\' AND `c_id`=\''.$c_id.'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_affected_rows($this->link);
		return ($r == 1);
	}
	
	public function get_contacts() {
		if(!isset($_SESSION['id'])) return false;
		$query = 'SELECT * FROM `contacts` WHERE `o_id`=\''.$_SESSION['id'].'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = array();
		$row = null;
		while(($row = mysql_fetch_row($results)) !== false) $r[] = $this->id_to_user($row[2]);
		return $r;
	}
	
	public function username_to_id($username) {
		$username = mysql_real_escape_string(strval($username),$this->link);
		$query = 'SELECT * FROM `users` WHERE `username`=\''.$username.'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_num_rows($results);
		if($r != 1) return false;
		$row = mysql_fetch_row($results);
		return $row[0];
	}
	
	public function change_password($password) {
		if(!isset($_SESSION['id'])) return false;
		$this->last_access();
		$password = sha1($password);
		$query = 'SELECT * FROM `users` WHERE `password`=\''.$password.'\' AND `id`=\''.$_SESSION['id'].'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_num_rows($results);
		if($r == 1) return true;
		$query = 'UPDATE `users` SET `password`=\''.$password.'\' WHERE `id`=\''.$_SESSION['id'].'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_affected_rows($this->link);
		#$this->hlog("Changed password");
		return ($r == 1);
	}
	
	public function handle_upload($file) {
		if(!isset($_SESSION['id'])) return false;
		if($file['error'] == 4) return false;
		$i = 2;
		$name = $file['name'];
		$name = explode('.',$name);
		if($name[count($name)-1] == 'php') $name[count($name)-1] = 'txt';
		$name = implode('.',$name);
		while(file_exists('./attachments/'.$file['name'])) while(file_exists('./attachments/'.strval($i++).' - '.$file['name'])) $name = strval($i).' - '.$file['name'];
		$r = @move_uploaded_file($file['tmp_name'], './attachments/'.$name);
		return ($r?$name:'');
	}
	
	public function get_sent_messages() {
		if(!isset($_SESSION['id'])) return false;
		$this->last_access();
		$query = 'SELECT `id`,`time`,`from`,`subject` FROM `messages` WHERE `from`=\''.strval($_SESSION['id']).'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = array();
		$row = null;
		while(($row = mysql_fetch_row($results)) !== false) $r[] = $row;
		return $r;
	}
	
	public function register_account($username,$password) {
		$username = mysql_real_escape_string($username,$this->link);
		$password = sha1($password);
		$query = 'INSERT INTO `messages`.`users` (`id`, `username`, `password`) VALUES (NULL, \''.$username.'\', \''.$password.'\')';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
		$r = mysql_affected_rows($this->link);
		return ($r == 1);
	}
	
	public function last_access() {
		if(!isset($_SESSION['id'])) return;
		$query = 'UPDATE `users` SET `last_access` = \''.strval(time()).'\' WHERE `id`=\''.strval($_SESSION['id']).'\'';
		$results = @mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
	}
	
	public function hlog($message) {
		if(!isset($_SESSION['id'])) return;
		$message = mysql_real_escape_string($message,$this->link);
		$query = 'INSERT INTO `messages`.`logs` (`id`, `user_id`, `message`, `ip_addr`, `time`) VALUES (NULL, \''.strval($_SESSION['id']).'\', \''.$message.'\', \''.$_SERVER['REMOTE_ADDR'].'\', \''.strval(time()).'\')';
		@mysql_query($query,$this->link) or die('MYSql Error on line '. __LINE__ .' : '.mysql_error($this->link));
	}
}
?>