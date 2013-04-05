var protagonist = require('./../build/Release/protagonist');

// Check what we have there
// console.log(protagonist);

// Specify a blueprint
var blueprint = "# XXX Snowcrash API\n"

// Parse blueprint
protagonist.parse(blueprint, function(error, ast) {

  //console.log("parse callback:")

  if (error) {
    console.log(error);
    return;
  }
  
  console.log(ast);
});
