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
  describe('Parse with callback', () => {
    it('valid fixture without options should return parse result', (done) => {
      parser.parse(valid_fixture, (err, refract) => {
        assert.isNull(err);
        assert.deepEqual(refract, valid_refract);
        done();
      });
    });

    it('valid fixture with options should return parse result', (done) => {
      parser.parse(valid_fixture, { generateSourceMap: true }, (err, refract) => {
        assert.isNull(err);
        assert.deepEqual(refract, valid_sourcemap_refract);
        done();
      });
    });

    it('error fixture without options should return parse result', (done) => {
      parser.parse(error_fixture, (err, refract) => {
        assert.isNull(err);
        assert.deepEqual(refract, error_refract);
        done();
      });
    });

    it('warning fixture without options should return parse result', (done) => {
      parser.parse(warning_fixture, (err, refract) => {
        assert.isNull(err);
        assert.deepEqual(refract, warning_parse_refract);
        done();
      });
    });
  });

  describe('Validate with callback', () => {
    it('valid fixture without options should return null', (done) => {
      parser.validate(valid_fixture, (err, refract) => {
        assert.isNull(err);
        assert.isNull(refract);
        done();
      });
    });

    it('valid fixture with options should return null', (done) => {
      parser.validate(valid_fixture, {}, (err, refract) => {
        assert.isNull(err);
        assert.isNull(refract);
        done();
      });
    });

    it('error fixture without options should return annotations', (done) => {
      parser.validate(error_fixture, (err, refract) => {
        assert.isNull(err);
        assert.deepEqual(refract, error_refract);
        done();
      });
    });

    it('warning fixture without options should return annotations', (done) => {
      parser.validate(warning_fixture, (err, refract) => {
        assert.isNull(err);
        assert.deepEqual(refract, warning_validate_refract);
        done();
      });
    });
  });
};
