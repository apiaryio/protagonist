protagonist = require '../build/Release/protagonist'
{assert} = require 'chai'

describe 'Blueprint AST', ->
  ast = null

  before (done) ->
    source = '''
    FORMAT: 1A

    # My API
    Lorem Ipsum

    ## Group First
    Dolor Sit Amet

    ### Message [/message{id}]
    '''

    protagonist.parse source, type: 'ast', (err, result) ->
      return done err if err
      ast = result.ast

      # For debugging puposes:
      #console.log JSON.stringify result, undefined, 2

      done()

  describe 'blueprint', ->
    it 'is defined', ->
      assert.isDefined ast

    describe 'version', ->
      it 'is defined', ->
        assert.isDefined ast._version

      it 'is equal to 4.0', ->
        assert.equal ast._version, '4.0'

    describe 'metadata', ->
      it 'is defined', ->
        assert.isDefined ast.metadata

      it 'is array', ->
        assert.typeOf ast.metadata, 'array'

      it 'has one entry', ->
        assert.equal ast.metadata.length, 1

      it 'the entry name is "FORMAT"', ->
        assert.strictEqual ast.metadata[0].name, 'FORMAT'

      it 'the entry value is "1A"', ->
        assert.strictEqual ast.metadata[0].value, '1A'

    describe 'name', ->
      it 'is defined', ->
        assert.isDefined ast.name

      it 'is "My API"', ->
        assert.strictEqual ast.name, 'My API'

    describe 'description', ->
      it 'is defined', ->
        assert.isDefined ast.description

      it 'is "Lorem Ipsum"', ->
        assert.strictEqual ast.description, 'Lorem Ipsum\n\n'

    describe 'resource group', ->
      it 'is defined', ->
        assert.isDefined ast.resourceGroups

      it 'is array', ->
        assert.typeOf ast.resourceGroups, 'array'

      it 'has one resource', ->
        assert.equal ast.resourceGroups.length, 1

    describe 'element', ->
      it 'is defined', ->
        assert.isDefined ast.element

      it 'is "category"', ->
        assert.strictEqual ast.element, 'category'

    describe 'content', ->
      it 'is defined', ->
        assert.isDefined ast.content

      it 'is array', ->
        assert.typeOf ast.content, 'array'

      it 'has one entry', ->
        assert.equal ast.content.length, 1

      describe 'entry', ->

        entry = undefined

        before ->
          entry = ast.content[0]

        it 'is defined', ->
          assert.isDefined entry

        describe 'element', ->
          it 'is defined', ->
            assert.isDefined entry.element

          it 'is of "category" type', ->
            assert.strictEqual entry.element, 'category'
