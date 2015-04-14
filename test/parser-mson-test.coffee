protagonist = require '../build/Release/protagonist'
{assert} = require 'chai'

describe 'MSON AST', ->
  mson_ast = null

  before (done) ->
    source = '''
    # Resource [/]
    ## Action [GET]
    + Response 200
        + attribute
            + message: Hello! (string) - The blog post article
            + things: a, b, c, 1, 2 (array[string, number], required, fixed)
            + choice
                + One of
                    + name
                    + address
            + mixin
                + Include Person

    # Data Structures
    ## Person
    '''

    protagonist.parse source, (err, result) ->
      return done err if err
      mson_ast = result.ast.content[0].content[0].actions[0].examples[0].responses[0].content[0]

      # For debugging puposes:
      #console.log JSON.stringify mson_ast, undefined, 2

      done()

  describe 'Attributes named type', ->
    it 'is defined', ->
      assert.isDefined mson_ast

    describe 'name', ->
      it 'is defined', ->
        assert.isDefined mson_ast.name

    describe 'base', ->
      it 'is undefined', ->
        assert.isUndefined mson_ast.base

    describe 'sections', ->
      it 'is defined', ->
        assert.isDefined mson_ast.sections

      it 'has one member section', ->
        assert.equal mson_ast.sections.length, 1

  describe 'Attributes object type section', ->
    member_ast = null

    before ->
      member_ast = mson_ast.sections[0]

    it 'is defined', ->
      assert.isDefined member_ast

    describe 'class', ->
      it 'is defined', ->
        assert.isDefined member_ast.class

      it 'is of "memberType" class', ->
        assert.equal member_ast.class, 'memberType'

    describe 'content', ->
      it 'is defined', ->
        assert.isDefined member_ast.content

      it 'has four member section', ->
        assert.equal member_ast.content.length, 4

    describe 'member type', ->
      member_type_ast = null

      before ->
        member_type_ast = member_ast.content[0]

      it 'is defined', ->
        assert.isDefined member_type_ast

      describe 'class', ->
        it 'is defined', ->
          assert.isDefined member_type_ast.class

        it 'is of "property" class', ->
          assert.equal member_type_ast.class, 'property'

      describe 'content', ->
        it 'is defined', ->
          assert.isDefined member_type_ast.content

      describe 'property member', ->
        property_member_ast = null

        before ->
          property_member_ast = member_type_ast.content

        describe 'name', ->
          it 'is defined', ->
            assert.isDefined property_member_ast.name

          it 'literal value is "messsage"', ->
            assert.equal property_member_ast.name.literal, 'message'
 
        describe 'description', ->
          it 'is defined', ->
            assert.isDefined property_member_ast.description
          
          it 'value is "The blog post article"', ->
            assert.equal property_member_ast.description, 'The blog post article'

        describe 'value definition', ->
          it 'is defined', ->
            assert.isDefined property_member_ast.valueDefinition

          describe 'values', ->
            it 'is defined', ->
              assert.isDefined property_member_ast.valueDefinition.values

            it 'has one value', ->
              assert.equal property_member_ast.valueDefinition.values.length, 1

            describe 'value', ->
              it 'is defined', ->
                assert.isDefined property_member_ast.valueDefinition.values[0]

              it 'literal value is "Hello!"', ->
                assert.equal property_member_ast.valueDefinition.values[0].literal, 'Hello!'

              it 'is not variable', ->
                assert.equal property_member_ast.valueDefinition.values[0].variable, false
          
          describe 'type definition', ->
            it 'is defined', ->
              assert.isDefined property_member_ast.valueDefinition.typeDefinition

            describe 'type specification', ->
              it 'is defined', ->
                assert.isDefined property_member_ast.valueDefinition.typeDefinition.typeSpecification

              describe 'type name', ->
                it 'is defined', ->
                  assert.isDefined property_member_ast.valueDefinition.typeDefinition.typeSpecification.name

                it 'is of "string" type', ->
                  assert.equal property_member_ast.valueDefinition.typeDefinition.typeSpecification.name, 'string'
