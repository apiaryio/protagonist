![logo](https://raw.github.com/apiaryio/api-blueprint/master/assets/logo_apiblueprint.png)

# Protagonist

[![Circle CI](https://circleci.com/gh/apiaryio/protagonist.svg?style=shield)](https://circleci.com/gh/apiaryio/protagonist)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/uaa6ivk97urmoucr/branch/master?svg=true)](https://ci.appveyor.com/project/Apiary/protagonist)

### API Blueprint Parser for Node.js
Protagonist is a Node.js wrapper for the [Drafter](https://github.com/apiaryio/drafter) library.

API Blueprint is Web API documentation language. You can find API Blueprint documentation on the [API Blueprint site](https://apiblueprint.org).

## Install

**NOTE:** *For general use we recommend that you use the [Drafter
NPM](https://github.com/apiaryio/drafter-npm) package instead of Protagonist
directly as Protagonist needs to be compiled which may not be possible in every
situation*

The best way to install Protagonist is by using its [NPM package](https://npmjs.org/package/protagonist).

```sh
$ npm install protagonist
```

**NOTE:** *Installing Protagonist depends on having Python 2 installed along with a compiler such as GCC, Clang or MSVC.*

Tested with node.js ~0.8.15, 0.10.x, 0.12.x, 4, 6

## Getting started

```js
var protagonist = require('protagonist');

protagonist.parse('# My API', function(error, result) {
    if (error) {
        console.log(error);
        return;
    }

    console.log(result);
});
```

### Synchronous usage

If you for some reason need to use the API synchronously, that is also possible.
Please note that the recommended way is to use the asynchronous API as to not
block the event loop.

```js
var result = protagonist.parseSync('# My API');
```

### Parsing Options

Options can be passed to the parser as an optional second argument to both the asynchronous and synchronous interfaces:

```js
var options = {
    generateSourceMap: true
}
protagonist.parse('# My API', options, callback);
```

The available options are:

Name                   | Description
---------------------- | ----------------------------------------------------------
`requireBlueprintName` | Require parsed blueprints have a title (default: false)
`generateSourceMap`    | Enable sourcemap generation (default: false)
`type`                 | Set the output structure type as either `ast` or `refract` (default: `refract`)

**NOTE**: *The `ast` option is deprecated in favour of `refract`.*

### Parsing Result

Parsing this blueprint:

```
# GET /1
+ response
```

will produce the following object (`result` variable):

```json
{
  "element": "parseResult",
  "content": [
    {
      "element": "category",
      "meta": {
        "classes": [
          "api"
        ],
        "title": ""
      },
      "content": [
        {
          "element": "category",
          "meta": {
            "classes": [
              "resourceGroup"
            ],
            "title": ""
          },
          "content": [
            {
              "element": "resource",
              "meta": {
                "title": ""
              },
              "attributes": {
                "href": "/1"
              },
              "content": [
                {
                  "element": "transition",
                  "meta": {
                    "title": ""
                  },
                  "content": [
                    {
                      "element": "httpTransaction",
                      "content": [
                        {
                          "element": "httpRequest",
                          "attributes": {
                            "method": "GET"
                          },
                          "content": []
                        },
                        {
                          "element": "httpResponse",
                          "attributes": {
                            "statusCode": "200"
                          },
                          "content": []
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
    }
  ]
}
```

## Hacking Protagonist
You are welcome to contribute. Use following steps to build & test Protagonist.

### Build
Protagonist uses [node-gyp](https://github.com/TooTallNate/node-gyp) build tool.

1. If needed, install node-gyp:

    ```sh
    $ npm install -g node-gyp
    ```

2. Clone the repository, and fetch the submodules:

    ```sh
    $ git clone --recursive git://github.com/apiaryio/protagonist.git
    $ cd protagonist
    $ npm install
    ```

3. Build:

    ```sh
    $ node-gyp configure
    $ node-gyp build
    ```

### Test
Inside the protagonist repository run:

```sh
$ npm test
```

### Contribute
Fork & Pull Request.

## License
MIT License. See the [LICENSE](https://github.com/apiaryio/protagonist/blob/master/LICENSE) file.
