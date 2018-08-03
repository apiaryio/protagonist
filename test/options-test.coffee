fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require './protagonist'

fixture_path = path.join __dirname, './fixtures/sample-api.apib'
expected_err = {
  "content": [
    {
      "attributes": {
        "code": {
          "content": 2,
          "element": "number"
        },
        "sourceMap": {
          "content": [
            {
              "content": [
                {
                  "content": [
                    {
                      "content": 0,
                      "element": "number"
                    },
                    {
                      "content": 12,
                      "element": "number"
                    }
                  ],
                  "element": "array"
                }
              ],
              "element": "sourceMap"
            }
          ],
          "element": "array"
        }
      },
      "content": "expected API name, e.g. '# <API Name>'",
      "element": "annotation",
      "meta": {
        "classes": {
          "content": [
            {
              "content": "error",
              "element": "string"
            }
          ],
          "element": "array"
        }
      }
    }
  ],
  "element": "parseResult"
}

describe "Requiring Blueprint name with sourcemaps", ->
  refract_err = null

  before (done) ->
    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      protagonist.parse data, { requireBlueprintName: true, generateSourceMap: true }, (err, result) ->
        if err
          refract_err = err

        done()

  it 'conforms to the refract spec', ->
    assert.deepEqual refract_err, expected_err

describe "Requiring Blueprint name without sourcemaps", ->
  refract_err = null

  before (done) ->
    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      protagonist.parse data, { requireBlueprintName: true, generateSourceMap: false }, (err, result) ->
        if err
          refract_err = err

        done()

  it 'conforms to the refract spec', ->
    assert.deepEqual refract_err, expected_err
