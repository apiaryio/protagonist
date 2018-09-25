fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require './protagonist'

describe "Parser Refract - Async", ->

  refract_fixture = require './fixtures/valid.parse.json'
  refract_parsed = null

  # Read & parse blueprint fixture
  before (done) ->

    fixture_path = path.join __dirname, './fixtures/valid.apib'

    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      protagonist.parse data, (err, result) ->
        return done err if err

        refract_parsed = result
        done()

  # Parser AST should conform to AST serialization JSON media type
  it 'conforms to the refract spec', ->
    assert.deepEqual refract_parsed, refract_fixture
