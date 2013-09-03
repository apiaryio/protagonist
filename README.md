# Protagonist [![Build Status](https://travis-ci.org/apiaryio/protagonist.png?branch=master)](https://travis-ci.org/apiaryio/protagonist)
### API Blueprint Parser for Node.js

Protagonist is a Node.js wrapper for the [Snow Crash](https://github.com/apiaryio/snowcrash) library.

API Blueprint is Web API documentation language. You can find API Blueprint documentation on the [API Blueprint site](http://apiblueprint.org).

## Install
The best way to install Protagonist is by using its [NPM package](https://npmjs.org/package/protagonist).

```sh
$ npm install protagonist
```

## Getting started

```js
var protagonist = require('protagonist');
protagonist.parse('# My API', function(error, result) {
	if (error) {
   		console.log(error);
	    return;
  	}
  
	console.log(result.ast);
}
```

Refer to [Snow Crash JSON serialization](https://github.com/apiaryio/snowcrash/wiki/API-Blueprint-AST-Media-Types#json-serialization) for details about the AST output structure.

## Hacking Protagonist
You are welcome to contribute. Use following steps to build & test Protagonist.

### Build
Protagonist uses [node-gyp](https://github.com/TooTallNate/node-gyp) build tool. 

1. If needed, install node-gyp:

	```sh
	$ npm install -g node-gyp
	```

2. Clone the repo + fetch the submodules:

	```sh
	$ git clone git://github.com/apiaryio/protagonist.git
	$ cd protagonist
	$ git submodule update --init --recursive
	```
    
3. Build:
	
	```sh
	$ node-gyp configure
	$ node-gyp build	
	```

### Test
Inside the protagonist repository run:

```sh
$ npm install
$ npm test
```
	
### Contribute
Fork & Pull Request. 

Make sure to follow the [API Blueprint Developers discussion group](https://groups.google.com/forum/#!forum/apiblueprint-dev). Also check the Protagonist [issues page](https://github.com/apiaryio/protagonist/issues?state=open).

## License
MIT License. See the [LICENSE](https://github.com/apiaryio/protagonist/blob/master/LICENSE) file.
