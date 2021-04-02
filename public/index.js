// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/uploadFiles',   //The server endpoint we are connecting to
        data: {

        },
        success: function (data) {

          //console.log("testing");
          //console.log(data.somethingElse);
          let thedata = data.somethingElse;

          if (thedata.length == 0 ) {
             jQuery('#FileLog').html("Console: No Files Loaded");
          }


          for (var i = 0; i < thedata.length; i++) {
            var obj = thedata[i];

            var table = document.getElementById("FileTable").getElementsByTagName('tbody')[0];
            var row = table.insertRow(0);
            var cell1 = row.insertCell(0);
            var cell2 = row.insertCell(1);
            var cell3 = row.insertCell(2);
            var cell4 = row.insertCell(3);
            var cell5 = row.insertCell(4);
            var cell6 = row.insertCell(5);
            cell1.innerHTML = '<a href="'+obj.filename+'">'+obj.filename+'</a>';
            cell2.innerHTML = obj.creator;
            cell3.innerHTML = obj.version;
            cell4.innerHTML = obj.numWaypoints;
            cell5.innerHTML = obj.numTracks;
            cell6.innerHTML = obj.numRoutes;

            var x = document.getElementById("FileSelect");
            var option = document.createElement("option");
            option.text = obj.filename;
            option.value = obj.filename;
            x.add(option, x[0]);
            

            var x = document.getElementById("AddRouteSelect");
            var option = document.createElement("option");
            option.text = obj.filename;
            option.value = obj.filename;
            x.add(option, x[0]);
            

            }




          console.log("GPX files read");

        },
        fail: function(error) {
            // Non-200 return, do something with erro
            console.log(error);
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#DropDown').on('change', function(e){
        let variable = document.getElementById("FileSelect").value;
        console.log("File Drop Down Button Pressed, File: "+ variable);
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({

            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/FileSelect',   //The server endpoint we are connecting to
            data: {
                data1 : variable
            },
            success: function (data) {

                jQuery('#GPXTable tbody').empty();
                $('#DataSelect').empty();
                $('#RenameSelect').empty();

                let routeList = data.RouteList; 
                let trackList = data.TrackList;

                console.log(routeList);
                console.log(trackList);

                if (routeList.length >0) {
                    for (var i = 0; i < routeList.length; i++) {
                        var temp = routeList[i];

                        var table = document.getElementById("GPXTable").getElementsByTagName('tbody')[0];
                        var row = table.insertRow(0);
                        var cell1 = row.insertCell(0);
                        var cell2 = row.insertCell(1);
                        var cell3 = row.insertCell(2);
                        var cell4 = row.insertCell(3);
                        var cell5 = row.insertCell(4);

                        cell1.innerHTML = "Route " + i;
                        cell2.innerHTML = temp.name;
                        cell3.innerHTML = temp.numPoints;
                        cell4.innerHTML = temp.len;
                        cell5.innerHTML = temp.loop;

                        var x = document.getElementById("DataSelect");
                        var option = document.createElement("option");
                        option.text = temp.name;
                        option.value = temp.name;
                        x.add(option, x[0]);

                        
                        var x = document.getElementById("RenameSelect");
                        var option = document.createElement("option");
                        option.text = temp.name;
                        option.value = temp.name;
                        x.add(option, x[0]);
                
                    }    
                }

                if (trackList.length >0) {
                for (var i = 0; i < trackList.length; i++) {
                    var temp = trackList[i];

                    var table = document.getElementById("GPXTable").getElementsByTagName('tbody')[0];
                    var row = table.insertRow(0);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    var cell3 = row.insertCell(2);
                    var cell4 = row.insertCell(3);
                    var cell5 = row.insertCell(4);

                    cell1.innerHTML = "Track " + i;
                    cell2.innerHTML = temp.name;
                    cell3.innerHTML = temp.numPoints;
                    cell4.innerHTML = temp.len;
                    cell5.innerHTML = temp.loop;

                    var x = document.getElementById("DataSelect");
                    var option = document.createElement("option");
                    option.text = temp.name;
                    option.value = temp.name;
                    x.add(option, x[0]);

                    
                    var x = document.getElementById("RenameSelect");
                    var option = document.createElement("option");
                    option.text = temp.name;
                    option.value = temp.name;
                    x.add(option, x[0]);

                }
            }
            },
            fail: function(error) {
                // Non-200 return, do something with erro
                console.log(error);
            }
        });
    });

    $('#CreateGPX').submit(function(e){
        e.preventDefault();
        console.log("CreateGPX Button Pressed");
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/createGPX',   //The server endpoint we are connecting to
            data: {
                filename : $('#FileNameEntryBox').val(),
                version : $('#VersionEntryBox').val(),
                creator :  $('#CreatorEntryBox').val(),
            },

            success: function (data) {

                location.reload();


            },
            fail: function(error) {
                console.log(error);
            }
        });



    });

    $('#ShowData').submit(function(e){
        e.preventDefault();
        let elementname = document.getElementById("DataSelect").value  
        let fileName = document.getElementById("FileSelect").value
        console.log("Show gpx data button pressed");
        jQuery('#GPXLog').html("Console: Data Not found");
        //console.log(fileName);
        //alert("Show GPX Data Button Pressed");
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({

            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/showGPX',   //The server endpoint we are connecting to
            data: {
                
                name: elementname,
                file: fileName

            },

            success: function (data) {
                alert("No data was found");
            },

            fail: function(error) {
                console.log(error);
            }
            //Create an object for connecting to another waypoint
        });
    });

    $('#RenameData').submit(function(e){ 
        e.preventDefault();
        console.log("Rename Button Pressed");
        //alert("Rename of "+ document.getElementById("renameID").val +"Failed!");
        jQuery('#GPXLog').html("Console: Rename Failed");
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/rename',   //The server endpoint we are connecting to
            data: {
                
            },

            success: function (data) {

            },
            fail: function(error) {
                console.log(error);
            }
        });
    });



    $('#FindPathBetween').submit(function(e){
        e.preventDefault();
        console.log("FindPathBetween Button Pressed");
        var table = document.getElementById("OutputTable").getElementsByTagName('tbody')[0];
        var row = table.insertRow(0);
        var cell1 = row.insertCell(0);
        var cell2 = row.insertCell(1);
        var cell3 = row.insertCell(2);
        var cell4 = row.insertCell(3);
        var cell5 = row.insertCell(4);
        cell1.innerHTML = "BLah";
        cell2.innerHTML = "Blah";
        cell3.innerHTML = "Blah";
        cell4.innerHTML = "Blah";
        cell5.innerHTML = "Blah";
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            
            //Create an object for connecting to another waypoint
        });
    });

    $('#Addwpt').submit(function(e){
        e.preventDefault();
        console.log("Add Waypoint Button pressed");
        var childrencount = document.getElementById("entry");
        let val = childrencount.childElementCount;
        //console.log(val);

        let NewInput = document.createElement('div');
        NewInput.innerHTML = "<label for='entryBox'>Lat"+val/2+"</label> <input type='text' value='123.456' class='form-control' id='newInputBox"+eval(val)+1+"'>";
        document.getElementById("entry").appendChild(NewInput);
        NewInput = document.createElement('div');
        NewInput.innerHTML = "<label for='entryBox'>Long"+val/2+"</label> <input type='text' value='123.456' class='form-control' id='newInputBox"+eval(val)+2+"'>";
        document.getElementById("entry").appendChild(NewInput);

        $('#entry').attr("value", val+2);
       
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });
    
    
    $('#AddRouteDropDown').submit(function(e){
        e.preventDefault();
        console.log("Add Route Button Pressed");

        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    

    $('#submitpls').submit(function(e){


        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({

          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/uploadFiles',   //The server endpoint we are connecting to
          data: {

          },
          success: function (data) {
              
            let thedata = data.somethingElse;

            if (thedata.length == 0 ) {
               jQuery('#FileLog').html("Console: No Files Loaded");
            }

            jQuery('#FileTable tbody').empty();
            $('#FileSelect').empty();
            //console.log("Emptying table");

              for (var i = 0; i < thedata.length; i++) {
              var obj = thedata[i];

              var table = document.getElementById("FileTable").getElementsByTagName('tbody')[0];
              var row = table.insertRow(0);
              var cell1 = row.insertCell(0);
              var cell2 = row.insertCell(1);
              var cell3 = row.insertCell(2);
              var cell4 = row.insertCell(3);
              var cell5 = row.insertCell(4);
              var cell6 = row.insertCell(5);
              cell1.innerHTML = '<a href="'+obj.filename+'">'+obj.filename+'</a>';
              cell2.innerHTML = obj.creator;
              cell3.innerHTML = obj.version;
              cell4.innerHTML = obj.numWaypoints;
              cell5.innerHTML = obj.numTracks;
              cell6.innerHTML = obj.numRoutes;

              var x = document.getElementById("FileSelect");
              var option = document.createElement("option");
              option.text = obj.filename;
              option.value = obj.filename;
              x.add(option, x[0]);


              }




            console.log("GPX files read");


              

              //jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
              //We write the object to the console to show that the request was successful
              //console.log(data);

          },
          fail: function(error) {
              // Non-200 return, do something with erro
              console.log(error);
          }

        });
    });
    
});
