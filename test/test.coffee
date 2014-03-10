SegfaultHandler = require 'segfault-handler'
SegfaultHandler.registerHandler()

protagonist = require '../build/Release/protagonist'

chai = require 'chai'
chai.Assertion.includeStack = true; 
assert = chai.assert

describe "API Blueprint parser", ->
  
  it 'parses API name', (done) ->
    protagonist.parse '# My API', (err, result) ->
      assert.isNull err

      assert.isDefined result
      assert.strictEqual result.ast.name, 'My API'

      done()

  it 'parses API description', (done) ->
    source = """
**description**

"""
    protagonist.parse source, (err, result) ->
      assert.isNull err

      assert.isDefined result
      assert.strictEqual result.ast.name, ''
      assert.isDefined result.ast.description
      assert.strictEqual result.ast.description, '**description**\n'

      done()

  it 'parses resource group', (done) ->
    source = """
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
      assert.strictEqual resourceGroup.name, 'Name'
      assert.isDefined resourceGroup.description
      assert.strictEqual resourceGroup.description, '_description_\n'

      done()

  it 'parses resource', (done) ->
    source = """
# My Resource [/resource]
Resource description

+ Model (text/plain)
  
        Hello World

## Retrieve Resource [GET]
Method description

+ Response 200 (text/plain)
  
  Response description

  + Headers

            X-Response-Header: Fighter

  + Body

            Y.T.

  + Schema

            Kourier

## Delete Resource [DELETE]

+ Response 200

    [My Resource][]

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
      assert.isDefined resource.uriTemplate
      assert.strictEqual resource.uriTemplate, '/resource'
      assert.isDefined resource.name
      assert.strictEqual resource.name, 'My Resource'
      assert.isDefined resource.description
      assert.strictEqual resource.description, 'Resource description\n\n'
      assert.isDefined resource.headers

      assert.strictEqual Object.keys(resource.headers).length, 0

      assert.isDefined resource.model
      assert.strictEqual resource.model.name, 'My Resource'
      assert.isDefined resource.model.description
      assert.strictEqual resource.model.description.length, 0
      assert.isDefined resource.model.body
      assert.strictEqual resource.model.body, 'Hello World\n'
      assert.isDefined resource.model.headers
      assert.strictEqual Object.keys(resource.model.headers).length, 1
      assert.isDefined resource.model.headers['Content-Type']
      assert.isDefined resource.model.headers['Content-Type'].value
      assert.strictEqual resource.model.headers['Content-Type'].value, 'text/plain'

      assert.isDefined resource.actions
      assert.strictEqual resource.actions.length, 2
      assert.isDefined resource.actions[0]

      action = resource.actions[0]
      assert.isDefined action.method
      assert.strictEqual action.method, 'GET'
      assert.isDefined action.name
      assert.strictEqual action.name, 'Retrieve Resource'
      assert.isDefined action.description
      assert.strictEqual action.description, 'Method description\n\n'
      assert.isDefined action.headers
      assert.strictEqual Object.keys(action.headers).length, 0

      assert.isDefined action.examples
      assert.strictEqual action.examples.length, 1
      assert.isDefined action.examples[0]

      example = action.examples[0]
      assert.isDefined example.requests
      assert.strictEqual example.requests.length, 0
      assert.isDefined example.responses
      assert.strictEqual example.responses.length, 1
      assert.isDefined example.responses[0]

      response = example.responses[0]
      assert.isDefined response.name
      assert.strictEqual response.name, '200'
      assert.isDefined response.description
      assert.strictEqual response.description, 'Response description\n'

      assert.isDefined response.headers
      assert.strictEqual Object.keys(response.headers).length, 2

      assert.isDefined response.headers['Content-Type']
      assert.isDefined response.headers['Content-Type'].value
      assert.strictEqual response.headers['Content-Type'].value, 'text/plain'
      assert.isDefined response.headers['X-Response-Header']
      assert.isDefined response.headers['X-Response-Header'].value
      assert.strictEqual response.headers['X-Response-Header'].value, 'Fighter'

      assert.isDefined response.body
      assert.strictEqual response.body, 'Y.T.\n'
      assert.isDefined response.schema
      assert.strictEqual response.schema, 'Kourier\n'

      action = resource.actions[1]
      assert.isDefined action.method
      assert.strictEqual action.method, 'DELETE'

      assert.isDefined action.examples
      assert.strictEqual action.examples.length, 1
      assert.isDefined action.examples[0]

      example = action.examples[0]
      assert.isDefined example.responses[0]
      response = example.responses[0]
      assert.isDefined response.name
      assert.strictEqual response.name, '200'
      assert.isDefined response.description
      assert.strictEqual response.description.length, 0
      assert.isDefined response.body
      assert.strictEqual response.body, 'Hello World\n'
      assert.isDefined response.headers

      assert.strictEqual Object.keys(response.headers).length, 1
      assert.isDefined response.headers['Content-Type']
      assert.isDefined response.headers['Content-Type'].value
      assert.strictEqual response.headers['Content-Type'].value, 'text/plain'

      done()

  it 'fails to parse blueprint with tabs', (done) ->
    source = """
# /resource
# GET
+ Response 
\tC
"""
    protagonist.parse source, (err, result) ->

      assert.isDefined err
      assert.isNotNull err
      assert err.message.length != 0
      assert err.code != 0
      assert.isDefined err.location

      done()

  it 'parses blueprint with warnings', (done) ->
    source = """
API description

---

Group description

"""
    protagonist.parse source, (err, result) ->
      assert.isNull err

      assert.isDefined result.warnings
      assert.strictEqual result.warnings.length, 1

      assert.isDefined result.warnings[0].message
      assert.isDefined result.warnings[0].code
      assert.isDefined result.warnings[0].location

      assert.isDefined result.ast

      done()

  it 'parses blueprint metadata', (done) ->
    source = """
A: 1
B: 2
C: 3

# API Name
"""
    protagonist.parse source, (err, result) ->
      assert.isNull err

      assert.isDefined result.warnings
      assert.strictEqual result.warnings.length, 0
      
      assert.isDefined result.ast.metadata
      assert.strictEqual Object.keys(result.ast.metadata).length, 3

      metadata = result.ast.metadata

      assert.isDefined metadata.A
      assert.isDefined metadata.A.value
      assert.strictEqual metadata.A.value, '1'

      assert.isDefined metadata.B
      assert.isDefined metadata.B.value
      assert.strictEqual metadata.B.value, '2'

      assert.isDefined metadata.C
      assert.isDefined metadata.C.value
      assert.strictEqual metadata.C.value, '3'

      done()

  it 'accepts options', (done) ->
    source = """
**description**

"""
    options =
      requireBlueprintName: true

    protagonist.parse source, options, (err, result) ->

      assert.isDefined err
      assert.isNotNull err
      assert err.message.length != 0
      assert err.code != 0
      assert.isDefined err.location

      done()

  it 'parses resource parameters', (done) ->
    source = """
# /machine{?limit}

+ Parameters
    + limit = `20` (optional, number, `42`) ... This is a limit
      + Values
          + `20`
          + `42`
          + `53`

## GET

+ Response 204
"""

    protagonist.parse source, (err, result) ->
      assert.isNull err

      assert.strictEqual result.ast.resourceGroups.length, 1
      assert.strictEqual result.ast.resourceGroups[0].resources.length, 1

      resource = result.ast.resourceGroups[0].resources[0]
      assert.strictEqual resource.uriTemplate, "/machine{?limit}"

      assert.isDefined resource.parameters
      assert.strictEqual Object.keys(resource.parameters).length, 1

      assert.isDefined resource.parameters.limit

      assert.isDefined resource.parameters.limit.description
      assert.strictEqual resource.parameters.limit.description, "This is a limit"

      assert.isDefined resource.parameters.limit.type
      assert.strictEqual resource.parameters.limit.type, "number"

      assert.isDefined resource.parameters.limit.required
      assert.strictEqual resource.parameters.limit.required, false

      assert.isDefined resource.parameters.limit.default
      assert.strictEqual resource.parameters.limit.default, "20"

      assert.isDefined resource.parameters.limit.example
      assert.strictEqual resource.parameters.limit.example, "42"

      assert.isDefined resource.parameters.limit.values
      assert.strictEqual resource.parameters.limit.values.length, 3

      values = resource.parameters.limit.values
      
      assert.isDefined values[0]
      assert.strictEqual values[0], "20"

      assert.isDefined values[1]
      assert.strictEqual values[1], "42"

      assert.isDefined values[2]
      assert.strictEqual values[2], "53"

      done()

  it 'parses action parameters', (done) ->
    source = """
# GET /coupons/{id}

+ Parameters
    + id (number, `1001`) ... Id of coupon

+ Response 204
"""

    protagonist.parse source, (err, result) ->
      assert.isNull err

      assert.strictEqual result.ast.resourceGroups.length, 1
      assert.strictEqual result.ast.resourceGroups[0].resources.length, 1
      assert.strictEqual result.ast.resourceGroups[0].resources[0].actions.length, 1 

      action = result.ast.resourceGroups[0].resources[0].actions[0]
      assert.strictEqual action.method, "GET"

      assert.isDefined action.parameters
      assert.strictEqual Object.keys(action.parameters).length, 1

      assert.isDefined action.parameters.id

      assert.isDefined action.parameters.id.description
      assert.strictEqual action.parameters.id.description, "Id of coupon"

      assert.isDefined action.parameters.id.type
      assert.strictEqual action.parameters.id.type, "number"

      assert.isDefined action.parameters.id.required
      assert.strictEqual action.parameters.id.required, true

      assert.isDefined action.parameters.id.default
      assert.strictEqual action.parameters.id.default, ""

      assert.isDefined action.parameters.id.example
      assert.strictEqual action.parameters.id.example, "1001"

      assert.isDefined action.parameters.id.values
      assert.strictEqual action.parameters.id.values.length, 0

      done()

  it 'parses multiple transactions', (done) ->
    source = """
# /resource
## GET

+ request A

        A

+ response 200

        200-A

+ request B

        B

+ response 200

        200-B

"""

    protagonist.parse source, (err, result) ->
      assert.isNull err

      assert.strictEqual result.ast.resourceGroups.length, 1
      assert.strictEqual result.ast.resourceGroups[0].resources.length, 1
      assert.strictEqual result.ast.resourceGroups[0].resources[0].actions.length, 1
      assert.strictEqual result.ast.resourceGroups[0].resources[0].actions[0].examples.length, 2

      example = result.ast.resourceGroups[0].resources[0].actions[0].examples[0]
      assert.strictEqual example.requests.length, 1
      assert.strictEqual example.responses.length, 1

      request = example.requests[0]
      assert.strictEqual request.name, 'A'
      assert.strictEqual request.body, 'A\n'      

      response = example.responses[0]
      assert.strictEqual response.name, '200'
      assert.strictEqual response.body, '200-A\n'      

      example = result.ast.resourceGroups[0].resources[0].actions[0].examples[1]   
      assert.strictEqual example.requests.length, 1
      assert.strictEqual example.responses.length, 1

      request = example.requests[0]
      assert.strictEqual request.name, 'B'
      assert.strictEqual request.body, 'B\n'

      response = example.responses[0]
      assert.strictEqual response.name, '200'
      assert.strictEqual response.body, '200-B\n'

      done()
