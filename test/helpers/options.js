const assert = require('chai').assert;
const path = require('path');
const fs = require('fs');

const valid_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'valid.apib'), 'utf8');
const warning_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'warning.apib'), 'utf8');
const error_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'error.apib'), 'utf8');

const valid_refract = require('../fixtures/valid.parse.json');
const valid_sourcemap_refract = require('../fixtures/valid.parse.sourcemap.json');
const require_name = require('../fixtures/require-name.json');

module.exports = (parser) => {
  describe('Options containing', () => {
    // it('unsupported option when parsing should throw error', () => {
    //   parser.parse(valid_fixture, { type: 'refract' })
    //     .then(assert.fail)
    //     .catch((err) => {
    //       assert.deepEqual(err.message, `unrecognized option 'type', expected: 'requireBlueprintName', 'generateSourceMap'`);
    //     });
    // });

    // it('unsupported option when validating should throw error', () => {
    //   parser.validate(valid_fixture, { type: 'refract' })
    //     .then(assert.fail)
    //     .catch((err) => {
    //       assert.deepEqual(err.message, `unrecognized option 'type', expected: 'requireBlueprintName', 'generateSourceMap'`);
    //     });
    // });

    it('old sourcemap option should work', () => {
      parser.parse(valid_fixture, { exportSourcemap: true })
        .then((refract) => {
          assert.deepEqual(refract, valid_sourcemap_refract);
        }).catch(assert.isNull);
    });

    describe('requireBlueprintName', () => {
      describe('when parsing', () => {
        it('should work', () => {
          parser.parse(valid_fixture, { requireBlueprintName: true })
            .then((refract) => {
              assert.deepEqual(refract, require_name);
            }).catch(assert.isNull);
        });

        it('along with sourcemap should work', () => {
          parser.parse(valid_fixture, { requireBlueprintName: true, generateSourceMap: true })
            .then((refract) => {
              assert.deepEqual(refract, require_name);
            }).catch(assert.isNull);
        })
      });

      it('when validating should work', () => {
        parser.validate(valid_fixture, { requireBlueprintName: true })
          .then((refract) => {
            assert.deepEqual(refract, require_name);
          }).catch(assert.isNull);
      });
    });
  });
};
