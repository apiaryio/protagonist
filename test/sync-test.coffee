fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require './protagonist'

describe "Parser Refract - Sync", ->

  refract_fixture = require './fixtures/sample-api-refract.json'
  refract_parsed = null

  # Read & parse blueprint fixture
  before (done) ->

    fixture_path = path.join __dirname, './fixtures/sample-api.apib'

    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      refract_parsed = protagonist.parseSync(data)
      done()

  # Parser AST should conform to AST serialization JSON media type
  it 'conforms to the refract spec', ->
    assert.deepEqual refract_parsed, refract_fixture
