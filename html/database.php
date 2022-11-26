<?php

$url='localhost';
$username='raspberry';
$password='pi';
$conn=mysqli_connect($url,$username,$password);

if(!$conn){
	die('Could not Connect My Sql:' .mysql_error());
}
$retval = mysqli_select_db($conn, 'DHT11_SENSOR');
	if (!$retval) {
		die('Could not select database:' . mysqli_errno($conn));
	}
	else { 
		echo(" "); 
	}

$result = mysqli_query($conn,"SELECT * FROM sensor");
$dateTempHum = array();
$i = 0;
while ($row = mysqli_fetch_array($result, MYSQLI_NUM))
{
	$dateTempHum[$i]=$row;
	$i++;
}

//echo json_encode($dateTempHum, JSON_NUMERIC_CHECK);

mysqli_close();

?>

