protagonist = require '../build/Release/protagonist'
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

    protagonist.parse source, type: 'ast', (err, result) ->
      return done err if err
      attributes = result.ast.content[0].content[0].actions[0].examples[0].responses[0].content[0]
      dataStructures = result.ast.content[1].content
      done()

  describe 'Attributes', ->
    it 'are defined', ->
      assert.isDefined attributes

    it 'are a data structure', ->
      assert.equal attributes.element, 'dataStructure'

    it 'are an object', ->
      assert.equal attributes.content[0].element, 'object'

    it 'object has a single member', ->
      assert.equal attributes.content[0].content.length, 1

    it 'member is `id`', ->
      assert.equal attributes.content[0].content[0].content.key.content, 'id'

  describe 'Data structures', ->
    it 'are defined', ->
      assert.isDefined dataStructures

    it 'have one item', ->
      assert.equal dataStructures.length, 1

    it 'item is a `Person` structure', ->
      assert.equal dataStructures[0].content[0].meta.id, 'Person'

    it 'Person has a `name` member', ->
      assert.equal dataStructures[0].content[0].content[0].content.key.content, 'name'
