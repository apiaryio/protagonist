protagonist = require './protagonist'
{assert} = require 'chai'

describe 'MSON Refract', ->
  attributes = null
  dataStructures = null

  before (done) ->
    source = '''
    # Resource [/]
    ## Action [GET]
    + Response 200
        + Attributes
            + id

    # Data Structures
    ## Person
    + name
    '''

    protagonist.parse source, (err, parseResult) ->
      return done err if err

      api = parseResult.content[0]
      resource = api.content[0]
      transition = resource.content[0]
      transaction = transition.content[0]
      response = transaction.content[1]
      attributes = response.content[0]
      dataStructures = api.content[1].content

      done()

  describe 'Attributes', ->
    it 'are defined', ->
      assert.isDefined attributes

    it 'are a data structure', ->
      assert.equal attributes.element, 'dataStructure'

    it 'are an object', ->
      assert.equal attributes.content.element, 'object'

    it 'member is `id`', ->
      assert.equal attributes.content.content[0].content.key.content, 'id'

  describe 'Data structures', ->
    it 'are defined', ->
      assert.isDefined dataStructures

    it 'have one item', ->
      assert.equal dataStructures.length, 1

    it 'item is a `Person` structure', ->
      assert.equal dataStructures[0].content.meta.id.content, 'Person'

    it 'Person has a `name` member', ->
      assert.equal dataStructures[0].content.content[0].content.key.content, 'name'
