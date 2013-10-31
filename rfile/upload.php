<?php
if(move_uploaded_file($_FILES['file']['tmp_name'],$_POST['path'].'/'.$_FILES['file']['name']))
{
header('Location: ./?path='.$_POST['path']);
}
else
{
echo 'Unknowen Error: Please Try Again';
}
?>