{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe "API Blueprint parser", ->
  it 'parses API name', ->
    protagonist.parse '# My API', (err, blueprint) ->
      assert.isNull err
      assert.isDefined blueprint
      assert.equal blueprint.name, "My API"
