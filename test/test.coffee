{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe "API Blueprint parser", ->
  it 'parses API name', ->
    protagonist.parse '# My API', (err, blueprint) ->
      assert.isNull err

      assert.isDefined blueprint
      assert.strictEqual blueprint.name, 'My API'

  it 'parses API description', ->
    source = """
**description**

"""
    protagonist.parse source, (err, blueprint) ->
      assert.isNull err

      assert.isDefined blueprint
      assert.strictEqual blueprint.name, ''
      assert.isDefined blueprint.description
      assert.strictEqual blueprint.description, '**description**\n'

  it 'parses resource group', ->
    source = """
---

# Group Name
_description_

"""
    protagonist.parse source, (err, blueprint) ->
      assert.isNull err

      assert.isDefined blueprint.resourceGroups
      assert.strictEqual blueprint.resourceGroups.length, 1
      assert.isDefined blueprint.resourceGroups[0] 

      resourceGroup = blueprint.resourceGroups[0]
      assert.isDefined resourceGroup.name
      assert.strictEqual resourceGroup.name, 'Group Name'
      assert.isDefined resourceGroup.description
      assert.strictEqual resourceGroup.description, '_description_\n'

  it 'parses resource', ->
      source = """
# /resource
A

# GET
B

+ Response 200 (text/plain)
  C

  + Body

            Hello World

  """
      protagonist.parse source, (err, blueprint) ->
        assert.isNull err

        assert.strictEqual blueprint.resourceGroups.length, 1

        resourceGroup = blueprint.resourceGroups[0]
        assert.strictEqual resourceGroup.name, ''
        assert.strictEqual resourceGroup.description, ''
        
        assert.isDefined resourceGroup.resources
        assert.strictEqual resourceGroup.resources.length, 1
        assert.isDefined resourceGroup.resources[0]

        resource = resourceGroup.resources[0]
        assert.isDefined resource.uri
        assert.strictEqual resource.uri, '/resource'
        assert.isDefined resource.description
        assert.strictEqual resource.description, 'A\n\n'
        assert.isDefined resource.methods
        assert.strictEqual resource.methods.length, 1
        assert.isDefined resource.methods[0]

        method = resource.methods[0]
        assert.isDefined method.method
        assert.strictEqual method.method, 'GET'
        assert.isDefined method.description
        assert.strictEqual method.description, 'B\n\n'
        assert.isDefined method.requests
        assert.strictEqual method.requests.length, 0
        assert.isDefined method.responses
        assert.strictEqual method.responses.length, 1
        assert.isDefined method.responses[0]

        response = method.responses[0]
        assert.isDefined response.name
        assert.strictEqual response.name, '200'
        assert.isDefined response.description
        assert.strictEqual response.description, 'C'
        assert.isDefined response.body
        assert.strictEqual response.body, 'Hello World\n'
