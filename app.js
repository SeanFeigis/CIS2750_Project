'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');
const { json } = require('express');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************

let sharedLib = ffi.Library('./libgpxparser', {

    'GPXFiletoJSON' : [ 'string', ['string', 'string' ] ],		//return type first, argument list second
    'GPXFiletoJSONTrackList': [ 'string', ['string', 'string'] ],	//int return, int argument
    'GPXFiletoJSONRouteList': [ 'string', ['string', 'string'] ],
    'JSONtoGPXtoFile': [ 'void', ['string', 'string'] ],
    'JSONListToWaypointList': [ 'void', ['string', 'string', 'string', 'string'] ],
    'RouteNameToJson': [ 'string', ['string', 'string' ] ],
    'renameRoute': [ 'string', ['string', 'string', 'string' ] ],
    'routeToWaypointList': [ 'string', ['string', 'string' ] ]
    
    //'getDesc' : [ 'string', [] ] */
});


app.get('/uploadFiles', function(req , res){
    
  var arr = [];
  var nameJSON = [];

  const path = require("path");
  fs.readdir('uploads/', (err, files) => {
    if (err)
      console.log(err);
    else {
      //console.log("\Filenames with the .txt extension:");
      files.forEach(file => {
        if (path.extname(file) == ".gpx") {

          let file2 = 'uploads/' + file;
          //console.log(file);
          let obj = JSON.parse(sharedLib.GPXFiletoJSON(file2, "gpx.xsd"));
          //console.log(obj);
          if (obj != null) {
              if (Object.entries(obj).length != 0) {
                obj['filename'] = file;
                nameJSON.push(obj);
              }
          }
          
          //nameJSON.push(file);
          //console.log(nameJSON);
          }
       })
      }
      res.send({somethingElse:nameJSON});
  })

  
  });

  app.get('/FileSelect', function(req , res){
    let name = req.query.data1;
    let obj = JSON.parse(sharedLib.GPXFiletoJSONRouteList('uploads/' + name, "gpx.xsd"));
    let obj2 = JSON.parse(sharedLib.GPXFiletoJSONTrackList('uploads/' + name, "gpx.xsd"));
    //console.log(obj);
    //console.log(obj2);
    res.send({

        RouteList: obj,
        TrackList: obj2,

    });
  });

  app.get('/createGPX', function(req , res){ 
    let obj = req.query;
    let oldObj = req.query; 
    obj['version'] = parseFloat(obj.version);
    let obj2 = req.query.filename;
    delete obj['filename'];
    //console.log(JSON.stringify(obj));

    sharedLib.JSONtoGPXtoFile(JSON.stringify(obj), 'uploads/' + obj2);

    res.send({
        theData: oldObj 
    });

  })

  app.get('/showGPX', function(req , res){ 
    let name = req.query.name;
    let file = req.query.file;
    let string = "No data was found";
    /*
    let string = sharedLib.RouteNameToJson('uploads/'+file, name);
    console.log(string);

    
    
    res.send(error);

   
    */
    res.send({
        theData: string 
    });

  })

  app.get('/rename', function(req , res){ 
    let newName = req.query.value1;
    let oldName = req.query.value2;
    let filename = req.query.value3;
    
    console.log(newName);
    console.log(oldName);
    console.log(filename);

    let returnval = sharedLib.renameRoute('uploads/' + filename, newName,oldName);



    res.send({
        theData: returnval 
    });

  })

  app.get('/addRoute', function(req , res){ 
    let name = req.query.Name;
    let filename = req.query.filename;
    let latArr = req.query.latVals;
    let longArr = req.query.longVals;
    let numWpts = req.query.numWpts;
    let objArr = [];

    for (let i = 0; i < numWpts; i++) {
        let obj = { "lat": latArr[i], "lon": longArr[i]};
        objArr.push(obj);
    }

    //console.log(filename);

    sharedLib.JSONListToWaypointList(JSON.stringify(objArr), name ,'uploads/' + filename ,numWpts);

    res.send({

    });

  })

  app.get('/storeFile', async function(req , res, next){ 
    let dbConf = req.query.loginInfo;
    let filenames = req.query.filenameData;
    let creators = req.query.creatorData;
    let versions = req.query.versionData;

    let rows;
    const mysql = require('mysql2/promise');
    let connection;
    let gpxCount = 0;
    let routeCount = 0;
    let pointCount = 0;
    //console.log(filenames);

    try{
        connection = await mysql.createConnection(dbConf);
        
        //connection.execute("drop table file");
        await connection.execute("create table if not exists FILE (gpx_id int auto_increment,  file_name VARCHAR(60) not null, ver decimal(2,1) not null, creator VARCHAR(256) not null, primary key(gpx_id))");
        await connection.execute("CREATE TABLE IF NOT EXISTS ROUTE (route_id INT AUTO_INCREMENT, route_name VARCHAR(256), route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, PRIMARY KEY(route_id), FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)");
        await connection.execute("CREATE TABLE IF NOT EXISTS POINT (point_id int auto_increment, point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT NOT NULL, FOREIGN KEY (route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE, primary key(point_id))");

            for (let i = 0; i < filenames.length; i++) {

                let unique = 0;
                const [rows10, fields10] = await connection.execute('SELECT file_name from FILE');
                for (let row of rows10) {
                    if (row.file_name === filenames[i]) {
                        unique = 1;
                        break;
                    }
                }
                if (unique == 1) {
                    continue;
                }


                await connection.execute("insert into FILE (file_name, ver, creator) values('"+filenames[i]+"', '"+versions[i]+"', '"+creators[i]+"');");
    
                const [rows1, fields1] = await connection.execute("SELECT gpx_id FROM FILE WHERE file_name = '"+filenames[i]+"';");
                let gpx_id = (rows1[0]);
                let obj = JSON.parse(sharedLib.GPXFiletoJSONRouteList('uploads/' + filenames[i], "gpx.xsd"));

                for (let j = 0; j<obj.length; j++) {
                    
                    let route = obj[j]; 

                    await connection.execute("insert into ROUTE (route_name, route_len, gpx_id) values('"+route.name+"','"+route.len+"', '"+gpx_id.gpx_id+"');");
                    const [rows2, fields2] = await connection.execute("SELECT LAST_INSERT_ID();");
                    let route_id = Object.values((rows2[0]));
                    //console.log(route_id[0]);

                    let ptIndex = 0;

                    let wptArray = JSON.parse(sharedLib.routeToWaypointList('uploads/' + filenames[i], j.toString()));
                    //console.log(wptArray);

                    for (let k = 0; k <wptArray.length; k++) {
                        let wpt = wptArray[k];   
                        await connection.execute("insert into POINT (point_name, latitude, longitude, point_index, route_id) values('"+wpt.name+"','"+wpt.lat+"','"+wpt.lon+"', '"+ptIndex+"','"+route_id+"');");
                        ptIndex++;
                    } 
                }
            }

            const [rows1, fields1] = await connection.execute('SELECT * from FILE');
            for (let row of rows1){
                console.log("ID: "+row.gpx_id+" file name: "+row.file_name+" version: "+row.ver+" creator: "+row.creator);
            }

            gpxCount = rows1.length;

            const [rows5, fields5] = await connection.execute('SELECT * from ROUTE');
            for (let row of rows5){
                console.log("ID: "+row.route_id+"  route_name: "+row.route_name+" route_len: "+row.route_len+" file_id: "+row.gpx_id);
            }

            routeCount = rows5.length;

            const [rows6, fields6] = await connection.execute('SELECT * from POINT');
            for (let row of rows6){
                console.log("ID: "+row.point_id+"  point_index: "+row.point_index+" latitude: "+row.latitude+" longitude: "+row.longitude+" point_name: "+row.point_name+" route_id: "+row.route_id);
            }

            pointCount = rows6.length;

            const [rows8, fields8] = await connection.execute('SELECT file_name from FILE');
            rows = rows8

        }catch(e){
            console.log("Queryerror:"+e);
        }finally {
            if (connection && connection.end) connection.end();
        }
  
    res.send({
         
        fCount : gpxCount,
        rCount : routeCount,
        pCount : pointCount,
        files  : rows
    });
  })



  app.get('/Login', async function(req , res, next){ 
    let dbConf = req.query.loginInfo;
    const mysql = require('mysql2/promise');
    let connection;
    let success = 0;

    let rows;
    

    try{
        connection = await mysql.createConnection(dbConf);

        const [rows1, fields1] = await connection.execute('SELECT file_name from FILE');
        rows = rows1;


        success = 1;
        }catch(e){
            console.log("Queryerror:"+e);
            success = 0;
        }finally {
            if (connection && connection.end) connection.end();
        }
    
    res.send({
        connection: success,
        files : rows
    });
  })



  app.get('/displayDB', async function(req , res, next){ 
    let dbConf = req.query.loginInfo;
    const mysql = require('mysql2/promise');
    
    let gpxCount = 0;
    let routeCount = 0;
    let pointCount = 0;


    let connection;
    try{
        connection = await mysql.createConnection(dbConf);

        const [rows1, fields1] = await connection.execute('SELECT * from FILE');
        const [rows2, fields2] = await connection.execute('SELECT * from ROUTE');
        const [rows3, fields3] = await connection.execute('SELECT * from POINT');

        gpxCount = rows1.length;
        routeCount = rows2.length;
        pointCount = rows3.length;

    }catch(e){
        console.log("Queryerror:"+e);
    }finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        fCount : gpxCount,
        rCount : routeCount,
        pCount : pointCount
    });

  })

  app.get('/cleardata', async function(req , res, next){ 
    let dbConf = req.query.loginInfo;
    const mysql = require('mysql2/promise');
    


    let connection;
    try{
        connection = await mysql.createConnection(dbConf);

        await connection.execute("DELETE FROM POINT");
        await connection.execute("DELETE FROM ROUTE");
        await connection.execute("DELETE FROM FILE");
        await connection.execute("ALTER TABLE POINT AUTO_INCREMENT = 1");
        await connection.execute("ALTER TABLE ROUTE AUTO_INCREMENT = 1");
        await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 1");


    }catch(e){
        console.log("Queryerror:"+e);
    }finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
    });

  })
  
  app.get('/getRoutesFromFile', async function(req , res, next){ 
    let dbConf = req.query.loginInfo;
    let filename = req.query.filename;
    const mysql = require('mysql2/promise');
    let rows;

    let connection;
    try{
        connection = await mysql.createConnection(dbConf);

        const [rows1, fields1] = await connection.execute("SELECT gpx_id from FILE where file_name='"+filename+"'");
        let gpx_id = (rows1[0].gpx_id); 

        //console.log("gpx_id: "+gpx_id);

        const [rows2, fields2] = await connection.execute("SELECT route_name from ROUTE where gpx_id='"+gpx_id+"'");
        rows = rows2;

        //console.log(rows);

    }catch(e){
        console.log("Queryerror:"+e);
    }finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        rows : rows
    });

  })

    app.get('/Query1', async function(req , res, next){ 
        let dbConf = req.query.loginInfo;
        const mysql = require('mysql2/promise');
        let sortType = req.query.sortType;
        let rows;
        let string;

        //console.log(sortType);

        if (sortType == 1) {
            string = "route_name";
        } else {
            string = "route_len";
        }

        

        let connection;
    try{
        connection = await mysql.createConnection(dbConf);

        const [rows1, fields1] = await connection.execute("SELECT * from ROUTE ORDER BY "+string+" DESC");
        rows = rows1;
        

    }catch(e){
        console.log("Queryerror:"+e);
    }finally {
        if (connection && connection.end) connection.end();
    }

        res.send({
            theRows : rows
        });
    
    })  
    
    app.get('/Query2', async function(req , res, next){ 
        let dbConf = req.query.loginInfo;
        const mysql = require('mysql2/promise');
        let sortType = req.query.sortType;
        let filename = req.query.filename;
        let rows;
        let string;

        if (sortType == 1) {
            string = "route_name";
        } else {
            string = "route_len";
        }

        let connection;
    try{
        connection = await mysql.createConnection(dbConf);

        const [rows2, fields2] = await connection.execute("SELECT * from ROUTE, FILE WHERE (FILE.gpx_id=ROUTE.gpx_id AND file_name='"+filename+"') ORDER BY "+string+" DESC");
        rows = rows2;

    }catch(e){
        console.log("Queryerror:"+e);
    }finally {
        if (connection && connection.end) connection.end();
    }

        res.send({
            theRows : rows
        });
    
    })  

    app.get('/Query3', async function(req , res, next){ 
        let dbConf = req.query.loginInfo;
        const mysql = require('mysql2/promise');
        let routeName = req.query.routeName;
        let filename = req.query.filename;
        let rows;
        
        let connection;
    try{
        connection = await mysql.createConnection(dbConf);

        const [rows2, fields2] = await connection.execute("SELECT * from POINT, ROUTE, FILE WHERE (POINT.route_id=ROUTE.route_id AND ROUTE.gpx_id=FILE.gpx_id AND file_name='"+filename+"' AND route_name='"+routeName+"') ORDER BY point_index DESC");
        rows = rows2;

    }catch(e){
        console.log("Queryerror:"+e);
    }finally {
        if (connection && connection.end) connection.end();
    }

        res.send({
            theRows : rows
        });
    
    })  

    app.get('/Query4', async function(req , res, next){ 
        let dbConf = req.query.loginInfo;
        const mysql = require('mysql2/promise');
        let sortType = req.query.sortType;
        let filename = req.query.filename;
        let rows;
        let string;

        if (sortType == 1) {
            string = "route_name";
        } else {
            string = "route_len";
        }

        let connection;
    try{
        connection = await mysql.createConnection(dbConf);

        const [rows1, fields1] = await connection.execute("SELECT * from FILE, ROUTE, POINT WHERE (FILE.file_name='"+filename+"' AND POINT.route_id=ROUTE.route_id AND ROUTE.gpx_id=FILE.gpx_id) ORDER BY "+string+" DESC, point_index DESC");
        rows = rows1;

    }catch(e){
        console.log("Queryerror:"+e);
    }finally {
        if (connection && connection.end) connection.end();
    }

        res.send({
            theRows : rows
        });
    
    })  

    app.get('/Query5', async function(req , res, next){ 
        let dbConf = req.query.loginInfo;
        const mysql = require('mysql2/promise');
        let sortType = req.query.sortType;
        let filename = req.query.filename;
        let length = req.query.length;
        let numPoints = req.query.numPoints;
        let rows;
        let string;
        let string2;

        if (length == 1) {
            string2 = "DESC";
        } else {
            string2 = "ASC";
        }

        if (sortType == 1) {
            string = "route_name";
        } else {
            string = "route_len";
        }


        let connection;
    try{
        connection = await mysql.createConnection(dbConf);
        let query = ("SELECT * FROM (SELECT route_name, route_len from (FILE INNER JOIN ROUTE on ROUTE.gpx_id=FILE.gpx_id) WHERE FILE.file_name='"+filename+"' ORDER BY route_len "+string2+" LIMIT "+numPoints+") sub ORDER BY "+string);
        console.log(query);
        const [rows1, fields1] = await connection.execute(query);
        rows = rows1;


    }catch(e){
        console.log("Queryerror:"+e);
    }finally {
        if (connection && connection.end) connection.end();
    }

        res.send({
            theRows : rows
        });
    
    })  



app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
