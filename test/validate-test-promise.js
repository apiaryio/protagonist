  var assert, error_fixture, error_refract, fs, path, protagonist, valid_fixture, warning_fixture, warning_refract;

  fs = require('fs');

  path = require('path');

  assert = require('chai').assert;

  protagonist = require('./protagonist');

  valid_fixture = path.join(__dirname, './fixtures/sample-api.apib');

  warning_fixture = path.join(__dirname, './fixtures/invalid-api-warning.apib');

  error_fixture = path.join(__dirname, './fixtures/invalid-api-error.apib');

  warning_refract = require('./fixtures/invalid-api-warning.json');

  error_refract = require('./fixtures/invalid-api-error.json');

describe("Validate Blueprint async with promise", () => {

  it("Validate with error", (done) => {
    fs.readFile(error_fixture, 'utf8', function(err, data) {
      if (err) {
        return done(err);
      }

      protagonist.validate(data)
        .then(res => {
          assert.fail()
        })
        .catch(err => {
          assert.deepEqual(err, error_refract);
          done()
        }) 
    });
  });

  it("Validate with warning", (done) => {
    fs.readFile(warning_fixture, 'utf8', function(err, data) {
      if (err) {
        return done(err);
      }

      protagonist.validate(data)
        .then(res => {
          assert.deepEqual(res, warning_refract);
          done()
        })
        .catch(err => {
          done(err)
        }) 
    });
  });

  it("Validate valid", (done) => {
    fs.readFile(valid_fixture, 'utf8', function(err, data) {
      if (err) {
        return done(err);
      }

      protagonist.validate(data)
        .then(res => {
          assert.isNull(res);
          done()
        })
        .catch(err => {
          done(err)
        }) 
    });
  });

})
