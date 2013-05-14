{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe "API Blueprint parser", ->
  it 'parses API name', ->
    protagonist.parse '# My API', (err, result) ->

      assert.isNull err
      assert.isDefined result
      assert.strictEqual result.ast.name, 'My API'

  it 'parses API description', ->
    source = """
**description**

"""
    protagonist.parse source, (err, result) ->
      
      assert.isNull err

      assert.isDefined result
      assert.strictEqual result.ast.name, ''
      assert.isDefined result.ast.description
      assert.strictEqual result.ast.description, '**description**\n'

  it 'parses resource group', ->
    source = """
---

# Group Name
_description_

"""
    protagonist.parse source, (err, result) ->

      assert.isNull err

      assert.isDefined result.ast.resourceGroups
      assert.strictEqual result.ast.resourceGroups.length, 1
      assert.isDefined result.ast.resourceGroups[0] 

      resourceGroup = result.ast.resourceGroups[0]
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
    protagonist.parse source, (err, result) ->
      assert.isNull err

      assert.strictEqual result.ast.resourceGroups.length, 1

      resourceGroup = result.ast.resourceGroups[0]
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

  it 'fails to parse response without status code', ->
    source = """
# /resource
# GET
+ Response 
  C
"""
    protagonist.parse source, (err, result) ->

      assert.isDefined err
      assert.isNotNull err
      assert err.message.length != 0
      assert err.code != 0
      assert.isDefined err.location
      assert err.location.length, 1
      assert err.location[0].index, 18
      assert err.location[0].length, 16
      assert.isNull result.ast

  it 'parses blueprint with warnings', ->
    source = """
API description

---

Group description

"""
    protagonist.parse source, (err, result) ->

      assert.isNull err

      assert.isDefined result.warnings
      assert result.warnings.length, 2
      
      assert.isDefined result.warnings[0].message
      assert.isDefined result.warnings[0].code
      assert.isDefined result.warnings[0].location

      assert.isDefined result.warnings[1].message
      assert.isDefined result.warnings[1].code
      assert.isDefined result.warnings[1].location

      assert.isDefined result.ast
