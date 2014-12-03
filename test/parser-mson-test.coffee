SegfaultHandler = require 'segfault-handler'
SegfaultHandler.registerHandler()

protagonist = require '../build/Release/protagonist'
{assert} = require 'chai'

describe 'MSON AST', ->
  ast = null

  before (done) ->
    source = '''
    # Resource [/]
    ## Action [GET]
    + Response 200
      + attribute
        + message (string) - The blog post article
    '''

    protagonist.parse source, (err, result) ->
      done err if err
      ast = result.ast.resourceGroups[0].resources[0].actions[0].examples[0].responses[0].attributes.source
      # TODO: remove
      # console.log JSON.stringify result.ast, undefined, 2
      done()

  describe 'Attributes Named Type', ->
    it 'is defined', (done) ->
      assert.isDefined ast.source
      done()

    describe 'name', ->
      it 'is not set', (done) ->
        assert.isNull ast.source.name
        done()

    # TODO: Wrapper implementation utilizing C++ AST .empty() calls
    #
    # describe 'parent type', ->
    #     it 'is not set', (done) ->
    #       assert.isNull ast.source.base
    #       done()

    # describe 'sections', ->
    #   it 'is set', (done) ->
    #     assert.isDefined ast.source.sections
    #     done()

    # TODO: AST for the "name: John" property
    #
    # describe 'parent type', ->
    #   it 'is defined', (done) ->
    #     assert.isDefined ast.source.base
    #     done()
    #
    #   describe 'Type Specification', ->
    #     it 'is defined', (done) ->
    #       assert.isDefined ast.source.base.typeSpecification
    #       done()
    #
    #     it 'is "string"', (done) ->
    #       assert.isStrictEqual ast.source.base.typeSpecification.name.name.literal, 'string'
    #       done()
    #
    #   describe 'Attributes', ->
    #     it 'are not defined', (done) ->
    #       assert.isUndefined ast.source.base.attributes
    #       done()
