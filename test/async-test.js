  var assert, fs, path, protagonist;

  fs = require('fs');
  path = require('path');
  assert = require('chai').assert;
  protagonist = require('./protagonist');

  data = fs.readFileSync(path.join(__dirname, './fixtures/sample-api.apib'), 'utf8');
  sync_parsed = protagonist.parseSync(data)

  describe('Async parse test', () => {


    it('will parse with callback', function(done) {
	  protagonist.parse(data, function (err, res) {
	     assert.isNull(err)
             assert.deepEqual(res, sync_parsed);
	     done()
      })
    })

    it.only('will parse with prom)ise', function(done) {
	  protagonist.parse(data)
	    .then(res => {
              assert.deepEqual(res, sync_parsed);
	    done()
		    console.log("aa")
	    })
	    .catch(err => {
		    assert.fail()
		    console.log("bb")
	    })
	 
      })

    it('will parse with await/async', async function(done) {
	  let res

	  try {
	    res = await protagonist.parse(data)
	  } catch (err) {
	    assert.fail()
	  }
              assert.deepEqual(res, sync_parsed);

      })
  })


