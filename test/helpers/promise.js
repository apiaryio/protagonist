const assert = require('chai').assert;
const path = require('path');
const fs = require('fs');

const valid_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'valid.apib'), 'utf8');
const warning_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'warning.apib'), 'utf8');
const error_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'error.apib'), 'utf8');

const valid_refract = require('../fixtures/valid.parse.json');
const valid_sourcemap_refract = require('../fixtures/valid.parse.sourcemap.json');
const warning_parse_refract = require('../fixtures/warning.parse.json');
const warning_validate_refract = require('../fixtures/warning.validate.json');
const error_refract = require('../fixtures/error.json');

module.exports = (parser) => {
  describe('Parse with promise', () => {
    it('valid fixture without options should return parse result', () => {
      parser.parse(valid_fixture)
        .then(refract => {
          assert.deepEqual(refract, valid_refract);
        }).catch(assert.isNull);
    });

    it('valid fixture with options should return parse result', () => {
      parser.parse(valid_fixture, { generateSourceMap: true })
        .then(refract => {
          assert.deepEqual(refract, valid_sourcemap_refract);
        }).catch(assert.isNull);
    });

    it('error fixture without options should return parse result', () => {
      parser.parse(error_fixture)
        .then(refract => {
          assert.deepEqual(refract, error_refract);
        }).catch(assert.isNull);
    });

    it('warning fixture without options should return parse result', () => {
      parser.parse(warning_fixture)
        .then(refract => {
          assert.deepEqual(refract, warning_parse_refract);
        }).catch(assert.isNull);
    });
  });

  describe('Validate with promise', () => {
    it('valid fixture without options should return null', () => {
      parser.validate(valid_fixture)
        .then(refract => {
          assert.isNull(refract);
        }).catch(assert.isNull);
    });

    it('valid fixture with options should return null', () => {
      parser.validate(valid_fixture, {})
        .then(refract => {
          assert.isNull(refract);
        }).catch(assert.isNull);
    });

    it('error fixture without options should return annotations', () => {
      parser.validate(error_fixture)
        .then(refract => {
          assert.deepEqual(refract, error_refract);
        }).catch(assert.isNull);
    });

    it('warning fixture without options should return annotations', () => {
      parser.validate(warning_fixture)
        .then(refract => {
          assert.deepEqual(refract, warning_validate_refract);
        }).catch(assert.isNull);
    });
  });
};
