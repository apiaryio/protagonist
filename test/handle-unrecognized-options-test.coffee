fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe "Parser - remove type option", ->
  source = null

  before (done) ->
    source = '''
    # My API
    ## GET /message
    + Response 200 (text/plain)

            Hello World!
    '''
    done()

  it 'should throw if you pass `type` as option into parse', ->
    assert.throws () ->
      protagonist.parseSync source, type: 'refract'
    , "unrecognized option 'type', expected: 'requireBlueprintName', 'generateSourceMap'"

  it 'should throw if you pass `type` as option into validate', ->
    assert.throws () ->
      protagonist.validateSync source, type: 'refract'
    , "unrecognized option 'type', expected: 'requireBlueprintName'"
