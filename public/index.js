// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/uploads',   //The server endpoint we are connecting to
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
            var row = table.insertRow(1);
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
    $('#DropDown').submit(function(e){

        console.log("File Drop Down Button Pressed + ");
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    $('#CreateGPX').submit(function(e){
        e.preventDefault();
        console.log("CreateGPX Button Pressed");
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    $('#FindPathBetween').submit(function(e){
        e.preventDefault();
        console.log("FindPathBetween Button Pressed");
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    $('#FileUploadForm').submit(function(e){


        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({

          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/uploads',   //The server endpoint we are connecting to
          data: {

          },
          success: function (data) {
            let thedata = data.somethingElse;

            if (thedata.length == 0 ) {
               jQuery('#FileLog').html("Console: No Files Loaded");
            }

              $("#FileTable:not(:first)").remove();

              for (var i = 0; i < thedata.length; i++) {
              var obj = thedata[i];

              var table = document.getElementById("FileTable").getElementsByTagName('tbody')[0];
              var row = table.insertRow(1);
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
              x.add(option, x[0]);

              }




            console.log("GPX files read");




              //jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
              //We write the object to the console to show that the request was successful
              console.log(data);

          },
          fail: function(error) {
              // Non-200 return, do something with erro
              console.log(error);
          }

        });
    });

});
