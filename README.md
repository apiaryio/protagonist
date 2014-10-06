![logo](https://raw.github.com/apiaryio/api-blueprint/master/assets/logo_apiblueprint.png)

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
});
```

### Parsing Result

Parsing this blueprint: 

```
# GET /1
+ response
```

will produce the following object (`result` variable):

```json
{
  "ast": {
    "_version": "2.1",
    "metadata": [],
    "name": "",
    "description": "",
    "resourceGroups": [
      {
        "name": "",
        "description": "",
        "resources": [
          {
            "name": "",
            "description": "",
            "uriTemplate": "/1",
            "model": {},
            "parameters": [],
            "actions": [
              {
                "name": "",
                "description": "",
                "method": "GET",
                "parameters": [],
                "examples": [
                  {
                    "name": "",
                    "description": "",
                    "requests": [],
                    "responses": [
                      {
                        "name": "200",
                        "description": "",
                        "headers": [],
                        "body": "",
                        "schema": ""
                      }
                    ]
                  }
                ]
              }
            ]
          }
        ]
      }
    ]
  },
  "warnings": [
    {
      "code": 6,
      "message": "missing response HTTP status code, assuming 'Response 200'",
      "location": [
        {
          "index": 12,
          "length": 9
        }
      ]
    }
  ]
}
```

#### Keys Description

+ `ast` ... This is the abstract syntax tree (AST) of the parsed blueprint. 

    The structure under this key is **1:1** with the [AST Blueprint serialization JSON media type v2.1](https://github.com/apiaryio/api-blueprint-ast#json-serialization) – `vnd.apiblueprint.ast.raw+json; version=2.1`.

+ `warnings` ... Array of the parser warnings as occurred during the parsing
    + `code` ...  Warning [group](https://github.com/apiaryio/snowcrash/blob/master/src/SourceAnnotation.h#L128) code
    + `message` ... Warning message
    + `location` ... Array of (possibly non-continuous) locations in the source blueprint
        + `index` ... Zero-based index of the character where the warning occurs
        + `lenght` ... Number of the characters from index where the warning occurs

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

## License
MIT License. See the [LICENSE](https://github.com/apiaryio/protagonist/blob/master/LICENSE) file.
