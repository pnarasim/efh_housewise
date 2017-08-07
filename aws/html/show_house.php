<?php
//Step1
 $db = mysqli_connect('localhost','hw1','efh@2017','housewise')
 or die('Error connecting to MySQL server.');
?>

<html>
 <head>
 </head>
 <body>
 <h1>HouseWise : Live Wisely! Know how you live!</h1>
House ID <?php echo $_POST["house_id"]; ?><br>

<?php
//Step2
$house_id = $_POST["house_id"];
$query = "SELECT * FROM hw_data where house_id=$house_id";
mysqli_query($db, $query) or die('Error querying database.');
$result = mysqli_query($db, $query);
$row = mysqli_fetch_array($result);

while ($row = mysqli_fetch_array($result)) {
 echo $row['house_id'] . ' ' . $row['sensor_id'] . ': ' . $row['sensor_value'] .'<br />';
}

//Step 4
mysqli_close($db);
?>

</body>
</html>
