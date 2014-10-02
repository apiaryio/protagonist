SegfaultHandler = require 'segfault-handler'
SegfaultHandler.registerHandler()

protagonist = require '../build/Release/protagonist'
{assert} = require 'chai'

describe "API Blueprint parser", ->

  it 'parses API name', (done) ->
    protagonist.parse '# My API', (err, result) ->
      assert.isNull err

      assert.isDefined result
      assert.strictEqual result.ast.name, 'My API'

      assert.isNull result.sourcemap

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

      assert.isNull result.sourcemap

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

      assert.isDefined resource.model
      assert.strictEqual resource.model.name, 'My Resource'
      assert.isDefined resource.model.description
      assert.strictEqual resource.model.description.length, 0
      assert.isDefined resource.model.body
      assert.strictEqual resource.model.body, 'Hello World\n'
      assert.isDefined resource.model.headers
      assert.strictEqual resource.model.headers.length, 1
      assert.strictEqual resource.model.headers[0].name, 'Content-Type'
      assert.strictEqual resource.model.headers[0].value, 'text/plain'

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
      assert.isUndefined response.reference
      assert.isDefined response.description
      assert.strictEqual response.description, 'Response description\n'

      assert.isDefined response.headers
      assert.strictEqual response.headers.length, 2

      assert.strictEqual response.headers[0].name, 'Content-Type'
      assert.strictEqual response.headers[0].value, 'text/plain'

      assert.strictEqual response.headers[1].name, 'X-Response-Header'
      assert.strictEqual response.headers[1].value, 'Fighter'

      assert.strictEqual response.body, 'Y.T.\n'
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
      assert.isDefined response.reference
      assert.isDefined response.reference.id
      assert.strictEqual response.reference.id, 'My Resource'
      assert.isDefined response.description
      assert.strictEqual response.description.length, 0
      assert.isDefined response.body
      assert.strictEqual response.body, 'Hello World\n'
      assert.isDefined response.headers

      assert.strictEqual response.headers.length, 1
      assert.strictEqual response.headers[0].name, 'Content-Type'
      assert.strictEqual response.headers[0].value, 'text/plain'

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
      assert.isNull result.sourcemap

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
      assert.strictEqual result.ast.metadata.length, 3

      metadata = result.ast.metadata

      assert.strictEqual metadata[0].name, 'A'
      assert.strictEqual metadata[0].value, '1'

      assert.strictEqual metadata[1].name, 'B'
      assert.strictEqual metadata[1].value, '2'

      assert.strictEqual metadata[2].name, 'C'
      assert.strictEqual metadata[2].value, '3'

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
      assert.strictEqual resource.parameters.length, 1

      parameter = resource.parameters[0]
      assert.strictEqual parameter.name, 'limit'
      assert.strictEqual parameter.description, 'This is a limit'
      assert.strictEqual parameter.type, 'number'
      assert.strictEqual parameter.required, false
      assert.strictEqual parameter.default, '20'
      assert.strictEqual parameter.example, '42'
      assert.strictEqual parameter.values.length, 3

      values = parameter.values
      assert.strictEqual values[0].value, '20'
      assert.strictEqual values[1].value, '42'
      assert.strictEqual values[2].value, '53'

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
      assert.strictEqual action.parameters.length, 1

      parameter = action.parameters[0]
      assert.strictEqual parameter.name, 'id'
      assert.strictEqual parameter.description, 'Id of coupon'
      assert.strictEqual parameter.type, 'number'
      assert.strictEqual parameter.required, true
      assert.strictEqual parameter.default, ''
      assert.strictEqual parameter.example, '1001'
      assert.strictEqual parameter.values.length, 0

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
