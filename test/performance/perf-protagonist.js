/*
 *  Protagonist async threading performance test
 *
 *  Run wiht fixtures:
 *
 *    node ../perf-protagonist.js fixture-1.apib fixture-1-ast.json fixture-2.apib fixture-2-ast.json 
 */ 

var fs = require('fs');
var path = require('path');
var protagonist = require('../../build/Release/protagonist');
var async = require('async');

// Total number of runs
var totalRuns = 10000;

// String buffers for blueprint, its ast and its alternatives
var blueprint, ast, altBlueprint, altAst;

// Process arguments
var args = process.argv.slice(2);
if (typeof args == 'undefined' || args.length !== 4) {
  var scriptName = path.basename(process.argv[1]);
  console.log('usage: ' + scriptName + ' <input> <input ast> <alt input> <alt input ast>\n');
  process.exit(0);
}

// Create a parse function alternating blueprints based on index
// \return Parse function taking a callback as its argument
var createParseFunction = function createParseFunction(index) {
    
    return function (callback) { 
      //console.log("> calling #" + index);

      protagonist.parse((index % 2) ? blueprint : altBlueprint, function (err, result) {

        //console.log("< finished #" + index);
        if (err) {
          console.error(JSON.stringify(err, null, 2));
          return callback(err);
        }

        if (JSON.stringify(result) !== ((index % 2) ? ast : altAst)) {
          return callback(new Error('parsing result does not match its input AST (' + index + ')'));
        }

        process.stdout.write('.');
        callback();
      });
    };
}

// Normalize input JSON string
// \param jsonString   A JSON string buffer as read from an input file
// \return normalized JSON string
var normalizeJSONString = function normalizeJSONString(jsonString) {
  
  try {
    return JSON.stringify(JSON.parse(jsonString));
  } 
  catch (e) {
    console.error('input is not a valid JSON');
    return;
  }  
}

// Run %totalRuns paprotagonist.parse() in async parallel
var runAsyncTest = function runAsyncTest() {

  var pool = [];

  for (var i = 0; i < totalRuns; ++i) {
    pool[i] = createParseFunction(i);
  }

  var startTime = new Date().getTime();

  async.parallel(pool, function (err) {
    if (err) {
      console.error(err);
      process.exit(err.code);
    }

    var endTime = new Date().getTime();
    var time = endTime - startTime;
    console.log('\ndone (' + time + 'ms)');
  });
}

// Read input files & run test
var inputData  = [];
async.forEachSeries(args, function (path, callback) {

  fs.readFile(path, 'utf8', function (err, data) {
    if (err)
      return callback(err);

    inputData.push(data);
    callback();
  });
}, function (err) {

  // Set up test buffers
  blueprint = inputData[0];
  ast = normalizeJSONString(inputData[1]);
  altBlueprint = inputData[2];
  altAst = normalizeJSONString(inputData[3]);

  // Run the test
  runAsyncTest();
});
