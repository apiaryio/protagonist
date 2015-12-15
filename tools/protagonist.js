/*
 * Naive commandline parser
 */

var fs = require('fs');
var path = require('path');
var protagonist = require('../build/Release/protagonist');
//var protagonist = require('protagonist');

// Add Segfault handler
//
// Note: this is possible only for OS X & *nix
//
// Install (or add to package.json) segfault-handler:
// "segfault-handler": "git://github.com/ddopson/node-segfault-handler.git#master"
//
//var segfaultHandler = require('segfault-handler');
//segfaultHandler.registerHandler();

// Process arguments
var args = process.argv.slice(2);
if (typeof args == 'undefined' || args.length !== 1) {
  var scriptName = path.basename(process.argv[1]);
  console.log('usage: ' + scriptName + ' <input file>\n');
  process.exit(0);
}

// Read & parse
fs.readFile(args[0], 'utf8', function (err, data) {
  if (err) throw err;

  protagonist.parse(data, {generateSourceMap: true}, function (err, result) {
    if (err) {
        console.log(JSON.stringify(err, null, 2));
        process.exit(err.code);
    }

    console.log(JSON.stringify(result, null, 2));
  });
});
