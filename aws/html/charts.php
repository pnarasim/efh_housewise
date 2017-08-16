<html>

   <head>
    <title>HouseWise Charts</title>
    <!--
        Step 2:  Include the `fusioncharts.js` file. This file is needed to render the chart.
         Ensure that the path to this JS file is correct. Otherwise, it may lead to JavaScript errors.
    -->
    <script src="fusioncharts/js/fusioncharts.js"></script>
   </head>
   <body>
<?php
// This is a simple example on how to draw a chart using FusionCharts and PHP.
// We have included includes/fusioncharts.php, which contains functions
// to help us easily embed the charts.
/* Include the `fusioncharts.php` file that contains functions  to embed the charts. */

  include("fusioncharts/fusioncharts.php");

$numdatapoints=40;
/* The following 4 code lines contain the database connection information. Alternatively, you can move these code lines to a separate file and include the file here. You can also modify this code based on your database connection. */

 $hostdb = "localhost";  // MySQl host
 $userdb = "hw1";  // MySQL username
 $passdb = "efh@2017";  // MySQL password
 $namedb = "housewise";  // MySQL database name

 // Establish a connection to the database
 $dbhandle = new mysqli($hostdb, $userdb, $passdb, $namedb);

 // Render an error message, to avoid abrupt failure, if the database connection parameters are incorrect
 if ($dbhandle->connect_error) {
  exit("There was an error with your connection: ".$dbhandle->connect_error);
 }

  // Form the SQL query that returns the top 10 most populous countries
  $house_id= $_POST["house_id"];
  $strQuery_noise = "SELECT date_format(timeofpost, '%H:%i:%s'), sensor_data FROM hw_data where house_id=$house_id and sensor_id=1 order by timeofpost desc LIMIT $numdatapoints";
  $strQuery_co2 = "SELECT date_format(timeofpost, '%H:%i:%s'), sensor_data FROM hw_data where house_id=$house_id and sensor_id=2 order by timeofpost desc LIMIT $numdatapoints";
  $strQuery_water = "SELECT date_format(timeofpost, '%H:%i:%s'), sensor_data FROM hw_data where house_id=$house_id and sensor_id=3 order by timeofpost desc LIMIT $numdatapoints";
  $strQuery_elec = "SELECT date_format(timeofpost, '%H:%i:%s'), sensor_data FROM hw_data where house_id=$house_id and sensor_id=4 order by timeofpost desc LIMIT $numdatapoints";

  // Execute the query, or else return the error message.
  $result_noise = $dbhandle->query($strQuery_noise) or exit("Error code ({$dbhandle->errno}): {$dbhandle->error}");
  $result_co2 = $dbhandle->query($strQuery_co2) or exit("Error code ({$dbhandle->errno}): {$dbhandle->error}");
  $result_water= $dbhandle->query($strQuery_water) or exit("Error code ({$dbhandle->errno}): {$dbhandle->error}");
  $result_elec= $dbhandle->query($strQuery_elec) or exit("Error code ({$dbhandle->errno}): {$dbhandle->error}");

	// +++ NOISE charts ++++++
  // If the query returns a valid response, prepare the JSON string
  if ($result_noise) {
    // The `$arrData` array holds the chart attributes and data
    $arrData_noise = array(
      "chart" => array(
          "caption" => "House ID=$house_id Noise Data ",
          "paletteColors" => "#0075c2",
          "bgColor" => "#ffffff",
          "borderAlpha"=> "20",
          "canvasBorderAlpha"=> "0",
          "usePlotGradientColor"=> "0",
          "plotBorderAlpha"=> "10",
          "showXAxisLine"=> "1",
          "xAxisName"=>"Time (seconds)",
	  "yAxisName"=>"Noise level (analog reading)",
          "xAxisLineColor" => "#999999",
          "showValues" => "0",
          "divlineColor" => "#999999",
          "divLineIsDashed" => "1",
          "showAlternateHGridColor" => "0"
        )
    );

    $arrData_noise["data"] = array();

    // Push the data into the array
    while($row = mysqli_fetch_array($result_noise)) {
      array_push($arrData_noise["data"], array(
          "label" => $row["timeofpost"],
          "value" => $row["sensor_data"]
          )
      );
    }

    /*JSON Encode the data to retrieve the string containing the JSON representation of the data in the array. */

    $jsonEncodedData_noise = json_encode($arrData_noise);

    /*Create an object for the column chart using the FusionCharts PHP class constructor. Syntax for the constructor is ` FusionCharts("type of chart", "unique chart id", width of the chart, height of the chart, "div id to render the chart", "data format", "data source")`. Because we are using JSON data to render the chart, the data format will be `json`. The variable `$jsonEncodeData` holds all the JSON data for the chart, and will be passed as the value for the data source parameter of the constructor.*/

    $lineChart_noise = new FusionCharts("line", "Housewise_NoiseData" , 600, 300, "chart-1", "json", $jsonEncodedData_noise);

    // Render the chart
    $lineChart_noise->render();
} // for if $result_noise

	// +++ Co2 charts ++++++
  // If the query returns a valid response, prepare the JSON string
  if ($result_co2) {
    // The `$arrData` array holds the chart attributes and data
    $arrData_co2 = array(
      "chart" => array(
          "caption" => "House ID=$house_id Co2 Data ",
          "paletteColors" => "#0075c2",
          "bgColor" => "#ffffff",
          "borderAlpha"=> "20",
          "canvasBorderAlpha"=> "0",
          "usePlotGradientColor"=> "0",
          "plotBorderAlpha"=> "10",
          "showXAxisLine"=> "1",
          "xAxisName"=>"Time (seconds)",
          "yAxisName"=>"Co2 level (ppm)",
          "xAxisLineColor" => "#999999",
          "showValues" => "0",
          "divlineColor" => "#999999",
          "divLineIsDashed" => "1",
          "showAlternateHGridColor" => "0"
        )
    );

    $arrData_co2["data"] = array();

    // Push the data into the array
    while($row = mysqli_fetch_array($result_co2)) {
      array_push($arrData_co2["data"], array(
          "label" => $row["timeofpost"],
          "value" => $row["sensor_data"]
          )
      );
    }

    //JSON Encode the data to retrieve the string containing the JSON representation of the data in the array. 

    $jsonEncodedData_co2 = json_encode($arrData_co2);

    //Create an object for the column chart using the FusionCharts PHP class constructor. Syntax for the constructor is ` FusionCharts("type of chart", "unique chart id", width of the chart, height of the chart, "div id to render the chart", "data format", "data source")`. Because we are using JSON data to render the chart, the data format will be `json`. The variable `$jsonEncodeData` holds all the JSON data for the chart, and will be passed as the value for the data source parameter of the constructor.

    $lineChart_co2 = new FusionCharts("line", "Housewise_Co2Data" , 600, 300, "chart-2", "json", $jsonEncodedData_co2);

    // Render the chart
    $lineChart_co2->render();
  } // for the if $result_co2

	// +++ Water charts ++++++
  // If the query returns a valid response, prepare the JSON string
  if ($result_water) {
    // The `$arrData` array holds the chart attributes and data
    $arrData_water = array(
      "chart" => array(
          "caption" => "House ID=$house_id Water Consumption ",
          "paletteColors" => "#0075c2",
          "bgColor" => "#ffffff",
          "borderAlpha"=> "20",
          "canvasBorderAlpha"=> "0",
          "usePlotGradientColor"=> "0",
          "plotBorderAlpha"=> "10",
          "showXAxisLine"=> "1",
          "xAxisLineColor" => "#999999",
          "showValues" => "0",
          "divlineColor" => "#999999",
          "divLineIsDashed" => "1",
          "showAlternateHGridColor" => "0"
        )
    );

    $arrData_water["data"] = array();

    // Push the data into the array
    while($row = mysqli_fetch_array($result_water)) {
      array_push($arrData_water["data"], array(
          "label" => $row["timeofpost"],
          "value" => $row["sensor_data"]
          )
      );
    }

    //JSON Encode the data to retrieve the string containing the JSON representation of the data in the array. 

    $jsonEncodedData_water = json_encode($arrData_water);

    //Create an object for the column chart using the FusionCharts PHP class constructor. Syntax for the constructor is ` FusionCharts("type of chart", "unique chart id", width of the chart, height of the chart, "div id to render the chart", "data format", "data source")`. Because we are using JSON data to render the chart, the data format will be `json`. The variable `$jsonEncodeData` holds all the JSON data for the chart, and will be passed as the value for the data source parameter of the constructor.

    $lineChart_water = new FusionCharts("line", "Housewise_WaterUsage" , 600, 300, "chart-3", "json", $jsonEncodedData_water);

    // Render the chart
    $lineChart_water->render();
  } // for the if $result_water

	// +++ Elec charts ++++++
  // If the query returns a valid response, prepare the JSON string
  if ($result_elec) {
    // The `$arrData` array holds the chart attributes and data
    $arrData_elec = array(
      "chart" => array(
          "caption" => "House ID=$house_id Electricity Consumption ",
          "paletteColors" => "#0075c2",
          "bgColor" => "#ffffff",
          "borderAlpha"=> "20",
          "canvasBorderAlpha"=> "0",
          "usePlotGradientColor"=> "0",
          "plotBorderAlpha"=> "10",
          "showXAxisLine"=> "1",
          "xAxisLineColor" => "#999999",
          "showValues" => "0",
          "divlineColor" => "#999999",
          "divLineIsDashed" => "1",
          "showAlternateHGridColor" => "0"
        )
    );

    $arrData_elec["data"] = array();

    // Push the data into the array
    while($row = mysqli_fetch_array($result_elec)) {
      array_push($arrData_elec["data"], array(
          "label" => $row["timeofpost"],
          "value" => $row["sensor_data"]
          )
      );
    }

    //JSON Encode the data to retrieve the string containing the JSON representation of the data in the array. 

    $jsonEncodedData_elec = json_encode($arrData_elec);

    //Create an object for the column chart using the FusionCharts PHP class constructor. Syntax for the constructor is ` FusionCharts("type of chart", "unique chart id", width of the chart, height of the chart, "div id to render the chart", "data format", "data source")`. Because we are using JSON data to render the chart, the data format will be `json`. The variable `$jsonEncodeData` holds all the JSON data for the chart, and will be passed as the value for the data source parameter of the constructor.

    $lineChart_elec = new FusionCharts("line", "Housewise_ElecUsage" , 600, 300, "chart-4", "json", $jsonEncodedData_elec);

    // Render the chart
    $lineChart_elec->render();
  } // for the if $result_elec


    // Close the database connection
    $dbhandle->close();

?>
 <div id="chart-1"><!-- Fusion Charts will render here--></div>
 <div id="chart-2"><!-- Fusion Charts will render here--></div>
 <div id="chart-3"><!-- Fusion Charts will render here--></div>
 <div id="chart-4"><!-- Fusion Charts will render here--></div>
   </body>
</html>

