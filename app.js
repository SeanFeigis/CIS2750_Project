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
    'RouteNameToJson': [ 'string', ['string', 'string' ] ]
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
          obj['filename'] = file;
          nameJSON.push(obj);
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
  

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
