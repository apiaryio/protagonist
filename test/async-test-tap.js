  var assert, fs, path, protagonist;

  fs = require('fs');
  path = require('path');
  protagonist = require('./protagonist');
  const test = require('tap').test

  data = fs.readFileSync(path.join(__dirname, './fixtures/sample-api.apib'), 'utf8');
  sync_parsed = protagonist.parseSync(data)

test('callback test', function (t) {
  protagonist.parse(data, function (err, result) {
    t.ok(!err)
    t.same(sync_parsed, result)
    t.end()
  })
})



test('promise test', function (t) {
  protagonist.parse(data)
    .then(result => {
      t.same(sync_parsed, result)

      t.end()
    })
    .catch(err => {
      t.fail(err)
    })
})

test('async/await test', async function (t) {
  let result

  try {
    result = await protagonist.parse(data)
  } catch (err) {
    t.fail(err)
  }

  t.same(sync_parsed, result)
  t.end()
})

