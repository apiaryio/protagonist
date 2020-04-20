![logo](https://raw.github.com/apiaryio/api-blueprint/master/assets/logo_apiblueprint.png)

# Protagonist - API Blueprint Parser for Node.js

[![Circle CI](https://circleci.com/gh/apiaryio/protagonist.svg?style=shield)](https://circleci.com/gh/apiaryio/protagonist)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/uaa6ivk97urmoucr/branch/master?svg=true)](https://ci.appveyor.com/project/Apiary/protagonist)

Protagonist is a Node.js wrapper for the
[Drafter](https://github.com/apiaryio/drafter), an API Blueprint parser. [API
Blueprint](https://apiblueprint.org) is a language for describing web APIs.

## Install

**NOTE:** *For general use we recommend that you use the [Drafter
NPM](https://github.com/apiaryio/drafter-npm) package instead of Protagonist
directly as Protagonist needs to be compiled which may not be possible in every
situation.*

Protagonist can be installed via the [Protagonist npm
package](https://npmjs.org/package/protagonist) by
[npm](https://docs.npmjs.com/cli/npm) or [yarn](https://yarnpkg.com/en/).

```sh
$ npm install protagonist
# or
$ yarn install protagonist
```

Protagonist uses the
[node-gyp](https://github.com/nodejs/node-gyp#installation) build tool which
requires Python 2.7 (3.x is not supported) along with a compiler and other
build tools. Take a look at their installation steps for
[Linux](https://github.com/nodejs/node-gyp#on-unix),
[macOS](https://github.com/nodejs/node-gyp#on-macos), and
[Windows](https://github.com/nodejs/node-gyp#on-windows).

## Usage

Protagonist offers the ability to both validate, and parse an API Blueprint.
It offers the following APIs:

- [`validate(source, options)`](#validate)
- [`validateSync(source, options)`](#validate-sync)
- [`parse(source, options)`](#parse)
- [`parseSync(source, options)`](#parse-sync)

**NOTE:** *It is not recommended to use the synchronous APIs as they can block
the Node.JS event loop.*

<a name="validate"></a>
### Validating an API Blueprint

You can validate an API Blueprint to determine if the source is a valid
document. The parse result will contain any errors or warnings that the
document would emit during parsing.

```js
const protagonist = require('protagonist');

const parseResult = await protagonist.parse('# My API');
console.log(JSON.stringify(parseResult, null, 2));
```

or by using Promises:

```js
const protagonist = require('protagonist');

protagonist.validate('# My API')
  .then((parseResult) => {
    console.log(JSON.stringify(parseResult, null, 2));
  })
  .catch((error) => {
    console.error(error);
  });
```

See the [parse result](#parse-result) section below for more information about
the structure of the parse result.

<a name="validate-sync"></a>
#### Synchronous API

```js
const protagonist = require('protagonist');
const parseResult = protagonist.validateSync('# My API');
```

#### Validation Options

Options can be passed to the parser as an optional second argument to both the asynchronous and synchronous interfaces:

```js
const protagonist = require('protagonist');

const options = {
  requireBlueprintName: true,
};
const parseResult = await protagonist.validate('# My API', options);
```

The available options are:

Name                   | Description
---------------------- | ----------------------------------------------------------
`requireBlueprintName` | Require parsed blueprints have a title (default: false)

<a name="parse"></a>
### Parsing an API Blueprint

You can parse an API Blueprint with async/await:

```js
const protagonist = require('protagonist');

const parseResult = await protagonist.parse('# My API');
console.log(JSON.stringify(parseResult, null, 2));
```

or by using Promises:

```js
const protagonist = require('protagonist');

protagonist.parse('# My API')
  .then((parseResult) => {
    console.log(JSON.stringify(parseResult, null, 2));
  })
  .catch((error) => {
    console.error(error);
  });
```

See the [parse result](#parse-result) section below for more information about
the structure of the parse result.

<a name="parse-sync"></a>
#### Synchronous API

```js
const parseResult = protagonist.parseSync('# My API');
```

#### Parsing Options

Options can be passed to the parser as an optional second argument to both the asynchronous and synchronous interfaces:

```js
const options = {
  generateSourceMap: true,
  generateMessageBody: true,
  generateMessageBodySchema: true,
};
const parseResult = await protagonist.parse('# My API', options);
```

The available options are:

Name                        | Description
--------------------------- | ----------------------------------------------------------
`requireBlueprintName`      | Require parsed blueprints have a title (default: false)
`generateSourceMap`         | Enable sourcemap generation (default: false)
`generateMessageBody`       | Enable generation of messageBody from MSON (default: true)
`generateMessageBodySchema` | Enable generation of messageBodySchema from MSON (default: true)

<a name="parse-result"></a>
### Parse Result

The format of the parse result is an [API Elements](https://apielements.org/)
structure, there is also [API Elements:
JS](https://github.com/apiaryio/api-elements.js) which contains tooling to
handle this format in JavaScript. It is recommended to use the provided API
Elements tooling to [prevent any tight
coupling](http://apielements.org/en/latest/overview.html#consuming-documents)
between your tool and a parse result.

As an example, parsing the following API Blueprint:

```apib
# GET /
+ Response 204
```

Would result in the following [API Elements Parse
Result](http://apielements.org/en/latest/element-definitions.html#parse-result-element-types):

```json
{
  "element": "parseResult",
  "content": [
    {
      "element": "category",
      "meta": {
        "classes": {
          "element": "array",
          "content": [
            {
              "element": "string",
              "content": "api"
            }
          ]
        },
        "title": {
          "element": "string",
          "content": ""
        }
      },
      "content": [
        {
          "element": "resource",
          "meta": {
            "title": {
              "element": "string",
              "content": ""
            }
          },
          "attributes": {
            "href": {
              "element": "string",
              "content": "/"
            }
          },
          "content": [
            {
              "element": "transition",
              "meta": {
                "title": {
                  "element": "string",
                  "content": ""
                }
              },
              "content": [
                {
                  "element": "httpTransaction",
                  "content": [
                    {
                      "element": "httpRequest",
                      "attributes": {
                        "method": {
                          "element": "string",
                          "content": "GET"
                        }
                      },
                      "content": []
                    },
                    {
                      "element": "httpResponse",
                      "attributes": {
                        "statusCode": {
                          "element": "string",
                          "content": "204"
                        }
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
```

## Developing Protagonist

You can use the following steps to build and test Protagonist.

```sh
$ git clone --recursive https://github.com/apiaryio/protagonist.git
$ cd protagonist
$ npm install
```

While iterating on the package, you can use `npm run build` to compile
Protagonist:

```sh
$ npm run build
$ npm test
```

Protagonist is built using [node-gyp](https://github.com/nodejs/node-gyp), you
can consult their documentation for further information on the build system.

## License

MIT License. See the [LICENSE](https://github.com/apiaryio/protagonist/blob/master/LICENSE) file.
