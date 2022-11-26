<?php
include_once 'database.php';
$result = mysqli_query($conn,"SELECT * FROM sensor");
?>

<!DOCTYPE html>
<html>
<head>
<title> Temperature and Humidity in room</title>
<style>
*{
	box-sizing: border-box;
}

.column {
	float: left;
	width: 50%;
	padding: 10px;
}

.row:after {
	content:"";
	display:table;
	clear:both;
}

table {
    border-collapse: collapse;
    font-family: "Lucida Console", "Courier New", monospace;
    width: auto;
    border: 1px solid black;
}

td, th {
    border: 1px solid #dddddd;
    text-align: center;
    font-weight:bold;
    padding: 8px;
}

tr:nth-child(even) {
    background-color: white;
}

tr:nth-child(odd) {
    background-color: #D6EEEE;
}

.button {
    border: none;
    color: black;
    padding: 15px 32px;
    text-align: center;
    font-family: "Lucida Console", "Courier New", monospace;
    text-decoration: none;
    font-weight:bold;
    display: inline-block;
    font-size: 16px;
    position: relative;
    margin: 4px 2px;
    transition-duration: 0.4s;
    cursor: pointer;
}

.button1 {
    background-color: white;
    position: absolute;
    left: 55%;
}
</style>
</head>

<body>
<h1 style="color:mediumstateblue;">DHT11 Temperature and Humdity Sensor</h1>
<p>The table displays measurements geathered by DHT11 sensor connected to the Raspberry Pi.</p>
<p>System is set up in the laboratory in Science and Technology Park Novi Sad. </p>

<div class="row">
    <div class="column">

    <table> 
      <tr>
	<td>Date and time</td>
	<td>Temperature [°C]</td>
	<td>Humidity [%]</td>
      </tr>
    <?php
    $i = 0;
    $sum_temp = 0;
    $sum_hum = 0;
    $avg_temp = 0;
    $avg_hum = 0;
    $max_temp = -20;
    
    while($row = mysqli_fetch_array($result)) {
		    
	    $temp = $row["temperature"];
	    $hum = $row["humidity"];
	    
		//store maximum value in variable
	    if ($temp > $max_temp){
		    $max_temp = $temp;
	    }
		
		//calculate average values
	    $sum_temp = $sum_temp + $temp;
	    $sum_hum = $sum_hum + $hum;

	    $avg_temp = $sum_temp/($i+1);
	    $avg_hum = $sum_hum/($i+1);
	    
		//depending on the temperature, the background changes
	    if ($temp < 0) {
		    $image = 'cold_winter.jpg';
	    } elseif ($temp < 10) {
		    $image = 'cold_autumn.jpg';
	    } elseif ($temp < 20 & $hum >= 70) {
		    $image = 'rain2.jpg';
	    } elseif ($temp < 20 & $hum < 70) {
		    $image = 'nice_day.jpg'; 
	    } elseif ($temp < 30) {
		    $image = 'hot_day.jpg'; 
	    } elseif ($hum < 40) {
		    $image = 'very_hot.jpg'; 
	    }
	    else {
		    $image = 'beach2.jpg'; 
	    }
    ?>
    <tr>
	<td><?php echo $row["datetime"]; ?></td>
	<td><?php echo $row["temperature"]; ?></td>
	<td><?php echo $row["humidity"]; ?></td>
    </tr>
    <?php
    $i++;
    }
    ?>
    </table>
    </div>
    
    <div class="column">

    <?php
    if (mysqli_num_rows($result) > 0) {
    ?>
    <table>
    <tr>
		<td>Current Temperature [°C]</td>
		<td>Current Humidity [%]</td>
    </tr>
    
    <tr>
		<td><?php echo $temp; ?></td>
		<td><?php echo $hum; ?></td>
    </tr>

    </table>
   
    <p> </p>
	
    <table>
    <tr>
		<td>Average Temperature [°C]</td>
		<td>Average Humidity [%]</td>
    </tr>
    
    <tr>
		<td><?php echo round($avg_temp, 2); ?></td>
		<td><?php echo round($avg_hum, 2); ?></td>
    </tr>

    </table>
	
	<p> </p>
	<p> </p>
    <p> </p>
    <p> </p>
	<p> </p>
	
    <button class="button button1" onclick="button1_info()">Check the highest temperature</button>
    <p id = p1 style="opacity:0.0"> . </p>
    <p id = p2 style="opacity:0.0"> . </p>
    <p id = "info" style="opacity:0.0"> In the entire database, the highest value of temperature is: </p>
    <p id = "temp_info"> </p>

    <script>
    	function button1_info() {
	    const elem = document.getElementById("info");
	    elem.style.opacity = 100.0;
	    document.getElementById("temp_info").innerHTML = <?php echo $max_temp; ?>;
        }
    </script>
    </div>
</div>
<?php
}
else{ echo "No result found"; }
?>
