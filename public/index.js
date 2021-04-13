// Put all onload AJAX calls here, and event listeners

jQuery(document).ready(function() {
    // On page-load AJAX Example
    
    var creatorData;
    var filenameData;
    var versionData;
    var serverFiles;
    var dbConf = {};

    $("#queryFileDropDownSelect").hide();
    $("#queryRouteDropDownSelect").hide();
    $("#longestDropDownSelect").hide();
    $("#NameOrLengthDropDownSelect").show();
    $("#numPointsBox").hide();

    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/uploadFiles',   //The server endpoint we are connecting to
        data: {

        },
        success: function (data) {

          //console.log("testing");
          //console.log(data.somethingElse);
          document.getElementById("cleardataButton").disabled = false;
          document.getElementById("storeFilesButton").disabled = false;
          

        if ($.isEmptyObject(dbConf)) {
            document.getElementById("cleardataButton").disabled = true;
            document.getElementById("storeFilesButton").disabled = true;
            document.getElementById("displaystatusButton").disabled = true;
            document.getElementById("executeQuery").disabled = true;
            document.getElementById("queryFileDropDownSelect").disabled = true;
            document.getElementById("queryRouteDropDownSelect").disabled = true;
            document.getElementById("NameOrLengthDropDownSelect").disabled = true;
            document.getElementById("executequerySelect").disabled = true;
            document.getElementById("longestDropDownSelect").disabled = true;
            document.getElementById("numPointsBox").disabled = true;
        } else {
            document.getElementById("cleardataButton").disabled = false;
            document.getElementById("storeFilesButton").disabled = false;
            document.getElementById("displaystatusButton").disabled = false;
            document.getElementById("executeQuery").disabled = false;
            document.getElementById("queryFileDropDownSelect").disabled = false;
            document.getElementById("queryRouteDropDownSelect").disabled = false;
            document.getElementById("NameOrLengthDropDownSelect").disabled = false;
            document.getElementById("executequerySelect").disabled = false;
            document.getElementById("longestDropDownSelect").disabled = false;
            document.getElementById("numPointsBox").disabled = false;
        }


          serverFiles = data.somethingElse;
          let thedata = data.somethingElse;
          creatorData = new Array(thedata.length);
          filenameData = new Array(thedata.length);
          versionData = new Array(thedata.length);
          
          if (thedata.length == 0 ) {
            jQuery('#FileLog').html("Console: No Files Loaded");
            document.getElementById("storeFilesButton").disabled = true;
          }


          for (var i = 0; i < thedata.length; i++) {
            var obj = thedata[i];

            creatorData[i] = obj.creator;
            filenameData[i] = obj.filename;
            versionData[i] = obj.version;

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

        let newName = document.getElementById("renametext").value;
        let oldName =  document.getElementById("RenameSelect").value;
        let filename =  document.getElementById("FileSelect").value;
        console.log("Rename Button Pressed");
        //alert("Rename of "+ document.getElementById("renameID").val +"Failed!");
        jQuery('#GPXLog').html("Console: Rename Failed");
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/rename',   //The server endpoint we are connecting to
            data: {
                value1 : newName,
                value2 : oldName,
                value3 : filename
            },

            success: function (data) {

                if (data.theData == null) {
                    jQuery('#GPXLog').html("Console: Rename Failed");
                } else {
                    jQuery('#GPXLog').html("Console: File Renamed!");
                }

                if ($.isEmptyObject(dbConf) == false) {

                    $.ajax({
            
                        type: 'get',            //Request type
                        dataType: 'json',       //Data type - we will use JSON for almost everything
                        url: '/cleardata',   //The server endpoint we are connecting to
                        data: {
                            loginInfo : dbConf,
                        },
            
                        success: function (data) {
            
                            $.ajax({
                
                                type: 'get',            //Request type
                                dataType: 'json',       //Data type - we will use JSON for almost everything
                                url: '/storeFile',   //The server endpoint we are connecting to
                                data: {
                                    loginInfo : dbConf,
                                    creatorData : creatorData,
                                    filenameData : filenameData,
                                    versionData : versionData
                                },
                    
                                success: function (data) {
                                    alert("Database has "+data.fCount+" files, "+data.rCount+" routes, and "+ data.pCount +" points");
                                    let variable = document.getElementById("queryFileDropDownSelect").value;
                                    $.ajax({
                                            
                                        type: 'get',            //Request type
                                        dataType: 'json',       //Data type - we will use JSON for almost everything
                                        url: '/getRoutesFromFile',   //The server endpoint we are connecting to
                                        data: {
                                            loginInfo : dbConf,
                                            filename : variable
                                        },

                                        success: function (data) {
                                            let rows = data.rows;

                                            console.log(rows);

                                            $('#queryRouteDropDownSelect').empty();
                                            for (let row of rows) {
                                                var x = document.getElementById("queryRouteDropDownSelect");
                                                var option = document.createElement("option");
                                                option.text = row.route_name;
                                                option.value = row.route_name;
                                                x.add(option, x[0]);
                                            }

                                        },
                                        fail: function(error) {
                                            console.log(error);
                                        }
                                    });
                                },
                                fail: function(error) {
                                    console.log(error);
                                }
                            });
                            
                        },
                        fail: function(error) {
                            console.log(error);
                        }
                    });
                }

                


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
        let val = childrencount.childElementCount -2 ;
        //console.log(val);

        let NewInput = document.createElement('div');
        NewInput.innerHTML = "<label for='entryBox'>Lat"+val/2+"</label> <input type='text' value='12.34' class='form-control' id='Lat"+(val/2)+"'>";
        document.getElementById("entry").appendChild(NewInput);
        NewInput = document.createElement('div');
        NewInput.innerHTML = "<label for='entryBox'>Long"+val/2+"</label> <input type='text' value='12.34' class='form-control' id='Long"+(val/2)+"'>";
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
        let fileName = document.getElementById("AddRouteSelect").value
        var latarr = [];
        var longarr = [];
        var childrencount = document.getElementById("entry");
        let val = (childrencount.childElementCount -2)/2 -1;
        //console.log(val);
        let name = document.getElementById("AddRouteName").value
        for (var i = 1; i <= val; i++) {
            latarr.push(document.getElementById("Lat"+i+"").value);
            longarr.push(document.getElementById("Long"+i+"").value)
        }

       

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/addRoute',   //The server endpoint we are connecting to
            data: {
                
                Name : name,
                latVals : latarr,
                longVals : longarr,
                numWpts : val,
                filename: fileName
            },

            success: function (data) {

                if ($.isEmptyObject(dbConf) == false) {
                    $.ajax({
            
                        type: 'get',            //Request type
                        dataType: 'json',       //Data type - we will use JSON for almost everything
                        url: '/cleardata',   //The server endpoint we are connecting to
                        data: {
                            loginInfo : dbConf,
                        },
            
                        success: function (data) {
            
                            $.ajax({
                
                                type: 'get',            //Request type
                                dataType: 'json',       //Data type - we will use JSON for almost everything
                                url: '/storeFile',   //The server endpoint we are connecting to
                                data: {
                                    loginInfo : dbConf,
                                    creatorData : creatorData,
                                    filenameData : filenameData,
                                    versionData : versionData
                                },
                    
                                success: function (data) {
                                    alert("Database has "+data.fCount+" files, "+data.rCount+" routes, and "+ data.pCount +" points");
                                    let variable = document.getElementById("queryFileDropDownSelect").value;
                                    $.ajax({
                                            
                                        type: 'get',            //Request type
                                        dataType: 'json',       //Data type - we will use JSON for almost everything
                                        url: '/getRoutesFromFile',   //The server endpoint we are connecting to
                                        data: {
                                            loginInfo : dbConf,
                                            filename : variable
                                        },

                                        success: function (data) {
                                            let rows = data.rows;

                                            console.log(rows);

                                            $('#queryRouteDropDownSelect').empty();
                                            for (let row of rows) {
                                                var x = document.getElementById("queryRouteDropDownSelect");
                                                var option = document.createElement("option");
                                                option.text = row.route_name;
                                                option.value = row.route_name;
                                                x.add(option, x[0]);
                                            }

                                        },
                                        fail: function(error) {
                                            console.log(error);
                                        }
                                    });
                                },
                                fail: function(error) {
                                    console.log(error);
                                }
                            });
                            
                        },
                        fail: function(error) {
                            console.log(error);
                        }
                    });
                }
            },
            fail: function(error) {
                console.log(error);
            }
        
        });

    });

    $('#StoreFile').submit(function(e){
        e.preventDefault();
        console.log("Store Files button pressed");

        $.ajax({
            
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/storeFile',   //The server endpoint we are connecting to
            data: {
                loginInfo : dbConf,
                creatorData : creatorData,
                filenameData : filenameData,
                versionData : versionData
            },

            success: function (data) {
                alert("Database has "+data.fCount+" files, "+data.rCount+" routes, and "+ data.pCount +" points");

                let fileList = data.files;
                //console.log(fileList);
                $('#queryFileDropDownSelect').empty();
                for (let row of fileList) {
                    var x = document.getElementById("queryFileDropDownSelect");
                    var option = document.createElement("option");
                    option.text = row.file_name;
                    option.value = row.file_name;
                    x.add(option, x[0]);
                }


               
                document.getElementById("cleardataButton").disabled = false;
                document.getElementById("executeQuery").disabled = false;
                document.getElementById("queryFileDropDownSelect").disabled = false;
                document.getElementById("queryRouteDropDownSelect").disabled = false;
                document.getElementById("NameOrLengthDropDownSelect").disabled = false;
                document.getElementById("executequerySelect").disabled = false;
                document.getElementById("storeFilesButton").disabled = false;
                document.getElementById("displaystatusButton").disabled = false;
                document.getElementById("longestDropDownSelect").disabled = false;
                document.getElementById("numPointsBox").disabled = false;





            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    $('#cleardata').submit(function(e){
        e.preventDefault();
        console.log("Clear data button pressed");
        
        let dbConf = {
            host     : 'dursley.socs.uoguelph.ca',
            user     : 'sfeigis',
            password : '1096849',
            database : 'sfeigis'
        };

        $.ajax({
            
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/cleardata',   //The server endpoint we are connecting to
            data: {
                loginInfo : dbConf,
            },

            success: function (data) {
                alert("Database has 0 files, 0 routes, and 0 points");
                document.getElementById("cleardataButton").disabled = true;
                document.getElementById("executeQuery").disabled = true;
                document.getElementById("queryFileDropDownSelect").disabled = true;
                document.getElementById("queryRouteDropDownSelect").disabled = true;
                document.getElementById("NameOrLengthDropDownSelect").disabled = true;
                document.getElementById("executequerySelect").disabled = true;
                document.getElementById("longestDropDownSelect").disabled = true;
                document.getElementById("numPointsBox").disabled = true;

            },
            fail: function(error) {
                console.log(error);
            }
        });
    });

    $('#displaystatus').submit(function(e){
        e.preventDefault();
        console.log("Display Status button pressed");
        
        $.ajax({
            
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/displayDB',   //The server endpoint we are connecting to
            data: {
                loginInfo : dbConf,
            },

            success: function (data) {
                alert("Database has "+data.fCount+" files, "+data.rCount+" routes, and "+ data.pCount +" points");
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });


    $('#executequery').on('change', function(e){ 
        e.preventDefault();
        let variable = document.getElementById("executequerySelect").value;
        console.log(variable);

        if (variable == 1) {
            $("#queryFileDropDownSelect").hide();
            $("#queryRouteDropDownSelect").hide();
            $("#longestDropDownSelect").hide();
            $("#NameOrLengthDropDownSelect").show();
            $("#numPointsBox").hide();

        }

        if (variable == 2) {
            $("#queryFileDropDownSelect").show();
            $("#queryRouteDropDownSelect").hide();
            $("#longestDropDownSelect").hide();
            $("#NameOrLengthDropDownSelect").show();
            $("#numPointsBox").hide();
        }

        if (variable == 3) {
            $("#queryFileDropDownSelect").show();
            $("#queryRouteDropDownSelect").show();
            $("#longestDropDownSelect").hide();
            $("#NameOrLengthDropDownSelect").hide();
            $("#numPointsBox").hide();
        }

        if (variable == 4) {
            $("#queryFileDropDownSelect").show();
            $("#queryRouteDropDownSelect").hide();
            $("#longestDropDownSelect").hide();
            $("#NameOrLengthDropDownSelect").show();
            $("#numPointsBox").hide();
        }

        if (variable == 5) {
            $("#queryFileDropDownSelect").show();
            $("#queryRouteDropDownSelect").hide();
            $("#longestDropDownSelect").show();
            $("#NameOrLengthDropDownSelect").show();
            $("#numPointsBox").show();
        }


    });

    $('#executeQuery').click(function(e){ 

        e.preventDefault();
        let variable = document.getElementById("executequerySelect").value;
        

        

        if (variable == 1) {

            let sort = $("#NameOrLengthDropDownSelect").val();

            $.ajax({
            
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: '/Query1',   //The server endpoint we are connecting to
                data: {
                    loginInfo : dbConf,
                    sortType : sort
                },
    
                success: function (data) {

                    jQuery('#QueryTable tbody').empty();
                    jQuery('#QueryTable thead').empty();
                    let table = document.getElementById("QueryTable");
                    let header = table.tHead;
                    let row = header.insertRow(0);    
                    let cell = row.insertCell(0);
                    let cell2 = row.insertCell(1);
                    let cell3 = row.insertCell(2);
                    let cell4 = row.insertCell(3);
                    cell.innerHTML = "Route ID";
                    cell2.innerHTML = "Route Name";
                    cell3.innerHTML = "Route Length";
                    cell4.innerHTML = "GPX ID";
                    
                    rows = data.theRows;
                    //console.log(data.theRows);

                    for (let row of rows) {
                        console.log(row);

                        let table = document.getElementById("QueryTable").getElementsByTagName('tbody')[0];
                        let tablerow = table.insertRow(0);
                        let cell1 = tablerow.insertCell(0);
                        let cell2 = tablerow.insertCell(1);
                        let cell3 = tablerow.insertCell(2);
                        let cell4 = tablerow.insertCell(3);
                        cell1.innerHTML = row.route_id;
                        cell2.innerHTML = row.route_name;
                        cell3.innerHTML = row.route_len;
                        cell4.innerHTML = row.gpx_id;

                    }

                },
                fail: function(error) {
                    console.log(error);
                }
            });
        }

        if (variable == 2) {
            let sort = $("#NameOrLengthDropDownSelect").val();
            let filename = $("#queryFileDropDownSelect").val();
            $.ajax({
            
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: '/Query2',   //The server endpoint we are connecting to
                data: {
                    loginInfo : dbConf,
                    sortType : sort,
                    filename : filename
                },
    
                success: function (data) {
                    
                    jQuery('#QueryTable tbody').empty();
                    jQuery('#QueryTable thead').empty();
                    let table = document.getElementById("QueryTable");
                    let header = table.tHead;
                    let row = header.insertRow(0);    
                    let cell = row.insertCell(0);
                    let cell2 = row.insertCell(1);
                    let cell3 = row.insertCell(2);
                    cell.innerHTML = "File Name";
                    cell2.innerHTML = "Route Name";
                    cell3.innerHTML = "Route Length";

                    rows = data.theRows;

                    for (let row of rows) {
                        console.log(row);

                        let table = document.getElementById("QueryTable").getElementsByTagName('tbody')[0];
                        let tablerow = table.insertRow(0);
                        let cell1 = tablerow.insertCell(0);
                        let cell2 = tablerow.insertCell(1);
                        let cell3 = tablerow.insertCell(2);
                        cell1.innerHTML = filename;
                        cell2.innerHTML = row.route_name;
                        cell3.innerHTML = row.route_len;

                    }



                },
                fail: function(error) {
                    console.log(error);
                }
            });
        }

        if (variable == 3) {

            let routeName = $("#queryRouteDropDownSelect").val();
            let filename = $("#queryFileDropDownSelect").val();

            $.ajax({
            
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: '/Query3',   //The server endpoint we are connecting to
                data: {
                    loginInfo : dbConf,
                    routeName : routeName,
                    filename : filename
                },
    
                success: function (data) {

                    jQuery('#QueryTable tbody').empty();
                    jQuery('#QueryTable thead').empty();
                    var table = document.getElementById("QueryTable");
                    var header = table.tHead;
                    var row = header.insertRow(0);    
                    var cell = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    var cell3 = row.insertCell(2);
                    var cell4 = row.insertCell(3);
                    var cell5 = row.insertCell(4);
                    var cell6 = row.insertCell(5);
                    cell.innerHTML = "Point ID";
                    cell2.innerHTML = "Point Index";
                    cell3.innerHTML = "Latitude";
                    cell4.innerHTML = "Longitude";
                    cell5.innerHTML = "Point name";
                    cell6.innerHTML = "Route ID";

                    rows = data.theRows;

                    for (let row of rows) {
                        console.log(row);

                        let table = document.getElementById("QueryTable").getElementsByTagName('tbody')[0];
                        let tablerow = table.insertRow(0);
                        let cell1 = tablerow.insertCell(0);
                        let cell2 = tablerow.insertCell(1);
                        let cell3 = tablerow.insertCell(2);
                        let cell4 = tablerow.insertCell(3);
                        let cell5 = tablerow.insertCell(4);
                        let cell6 = tablerow.insertCell(5);
                        cell1.innerHTML = row.point_id;
                        cell2.innerHTML = row.point_index;
                        cell3.innerHTML = row.latitude;
                        cell4.innerHTML = row.longitude;
                        cell5.innerHTML = row.point_name;
                        cell6.innerHTML = row.route_id;

                    }

                },
                fail: function(error) {
                    console.log(error);
                }
            });
        }

        if (variable == 4) {

            let sort = $("#NameOrLengthDropDownSelect").val();
            let filename = $("#queryFileDropDownSelect").val();

            $.ajax({
            
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: '/Query4',   //The server endpoint we are connecting to
                data: {
                    loginInfo : dbConf,
                    sortType : sort,
                    filename : filename
                },
    
                success: function (data) {

                    jQuery('#QueryTable tbody').empty();
                    jQuery('#QueryTable thead').empty();
                    var table = document.getElementById("QueryTable");
                    var header = table.tHead;
                    var row = header.insertRow(0);    
                    var cell = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    var cell3 = row.insertCell(2);
                    var cell4 = row.insertCell(3);
                    var cell5 = row.insertCell(4);
                    var cell6 = row.insertCell(5);
                    cell.innerHTML = "Point ID";
                    cell2.innerHTML = "Point Index";
                    cell3.innerHTML = "Latitude";
                    cell4.innerHTML = "Longitude";
                    cell5.innerHTML = "Route name";
                    cell6.innerHTML = "Route ID";


                    rows = data.theRows;
                    let i = 1;
                    for (let row of rows) {
                        console.log(row);

                        let table = document.getElementById("QueryTable").getElementsByTagName('tbody')[0];
                        let tablerow = table.insertRow(0);
                        let cell1 = tablerow.insertCell(0);
                        let cell2 = tablerow.insertCell(1);
                        let cell3 = tablerow.insertCell(2);
                        let cell4 = tablerow.insertCell(3);
                        let cell5 = tablerow.insertCell(4);
                        let cell6 = tablerow.insertCell(5);
                        cell1.innerHTML = row.point_id;
                        cell2.innerHTML = row.point_index;
                        cell3.innerHTML = row.latitude;
                        cell4.innerHTML = row.longitude;
                        if (row.point_name === "") {
                            cell5.innerHTML = "Unnamed";
                        }
                        cell5.innerHTML = row.route_name;
                        cell6.innerHTML = row.route_id;
                        i++;
                    }



                },
                fail: function(error) {
                    console.log(error);
                }
            });
        }

        if (variable == 5) {

            let sort = $("#NameOrLengthDropDownSelect").val();
            let filename = $("#queryFileDropDownSelect").val();
            let length = $("#longestDropDownSelect").val();
            let numPoints = $("#numPointsBox").val();

            $.ajax({
            
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: '/Query5',   //The server endpoint we are connecting to
                data: {
                    loginInfo : dbConf,
                    sortType : sort,
                    filename : filename,
                    length : length,
                    numPoints : numPoints
                },
    
                success: function (data) {

                    jQuery('#QueryTable tbody').empty();
                    jQuery('#QueryTable thead').empty();
                    var table = document.getElementById("QueryTable");
                    var header = table.tHead;
                    var row = header.insertRow(0);    
                    var cell = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    var cell3 = row.insertCell(2);
                    cell.innerHTML = "File Name";
                    cell2.innerHTML = "Route Name";
                    cell3.innerHTML = "Route Length";



                    rows = data.theRows;

                    for (let row of rows) {
                        console.log(row);

                        let table = document.getElementById("QueryTable").getElementsByTagName('tbody')[0];
                        let tablerow = table.insertRow(0);
                        let cell1 = tablerow.insertCell(0);
                        let cell2 = tablerow.insertCell(1);
                        let cell3 = tablerow.insertCell(2);
                        cell1.innerHTML = filename;
                        cell2.innerHTML = row.route_name;
                        cell3.innerHTML = row.route_len;

                    }



                },
                fail: function(error) {
                    console.log(error);
                }
            });
        }

    });

    $('#queryFileDropDown').on('change', function(e){    
        e.preventDefault();
        let variable = document.getElementById("queryFileDropDownSelect").value;


        $.ajax({
                
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/getRoutesFromFile',   //The server endpoint we are connecting to
            data: {
                loginInfo : dbConf,
                filename : variable
            },

            success: function (data) {
                let rows = data.rows;

                console.log(rows);

                $('#queryRouteDropDownSelect').empty();
                for (let row of rows) {
                    var x = document.getElementById("queryRouteDropDownSelect");
                    var option = document.createElement("option");
                    option.text = row.route_name;
                    option.value = row.route_name;
                    x.add(option, x[0]);
                }

            },
            fail: function(error) {
                console.log(error);
            }
        });
    
    });

    $('#LoginForm').submit(function(e){
        e.preventDefault();
        console.log("Login button pressed");
        
        let newUsername = $("#userEntryBox").val();
        let newPassword = $("#passEntryBox").val();
        let newDatabase = $("#dataEntryBox").val();
    
        let newdbConf = {
            host     : 'dursley.socs.uoguelph.ca',
        }
    
        newdbConf['user'] = newUsername;
        newdbConf['password'] = newPassword;
        newdbConf['database'] = newDatabase;
    
        $.ajax({
            
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything
            url: '/Login',   //The server endpoint we are connecting to
            data: {
                loginInfo : newdbConf,
            },
    
            success: function (data) {
                
                if (data.connection) {
    
                    dbConf = newdbConf;
                    //console.log(dbConf);
    
                    let fileList = data.files;
                    $('#queryFileDropDownSelect').empty();
                    //console.log(fileList);
                    for (let row of fileList) {
                        var x = document.getElementById("queryFileDropDownSelect");
                        var option = document.createElement("option");
                        option.text = row.file_name;
                        option.value = row.file_name;
                        x.add(option, x[0]);
                    }

                    if ($.isEmptyObject(fileList) == false) {
                        document.getElementById("cleardataButton").disabled = false;
                        document.getElementById("executeQuery").disabled = false;
                        document.getElementById("queryFileDropDownSelect").disabled = false;
                        document.getElementById("queryRouteDropDownSelect").disabled = false;
                        document.getElementById("NameOrLengthDropDownSelect").disabled = false;
                        document.getElementById("executequerySelect").disabled = false;
                        document.getElementById("longestDropDownSelect").disabled = false;
                        document.getElementById("numPointsBox").disabled = false;
                    }
    
                    if (serverFiles.length != 0 ) {
                        document.getElementById("storeFilesButton").disabled = false;
                    }
                    
                    document.getElementById("displaystatusButton").disabled = false;
                   
    
    
                    alert("Connection Success!");
                } else {
                    alert("Connection Failed, New Login Not accepted.");
                }
            },
            fail: function(error) {
                console.log(error);
            }
        });
    });
});

    




