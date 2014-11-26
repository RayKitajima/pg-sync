
# pg-sync

Synchronous, blocking libpq binding (PostgreSQL client) for node.js, based on Brian Carlson's node-postgres.

## Preparation

This module requires libpq in your system.

	wget http://ftp.postgresql.org/pub/source/<version>/postgresql-<version9>.tar.gz
	tar zxvf postgresql-<version9>.tar.gz
	cd postgresql-<version9>
	./configure --prefix=/path/to/pgsql
	make
	make install
	
## Installation

    npm install pg-sync
    
## Examples

```javascript
var pgsync = require('pg-sync');
var client = new pgsync.Client();

client.connect("host=127.0.0.1 port=5432 dbname=mydb");

client.begin();
client.setIsolationLevelSerializable();

var result = client.query(queryText);
var result = client.query(queryText,[params]);

var result = client.exec(command); // returns result code only, even if the command was normal query

var statement = client.prepare(queryText);
var result = statement.execute([params]);

var queryName = "a_repeated_query";
var queryText = "delete from table where id=$1";
var statement = client.prepare(queryName,queryText);
[1,2,3,4].map( function(i){ statement.execute([i]); } );

client.commit();
client.rollback();

client.disconnect();
```

## Note

Node 0.8, 0.10, 0.11+ supported.

This module does not support copy command.

## See also

http://github.com/brianc/node-postgres

## License

Copyright (c) 2013 Rei Kitajima (rei.kitajima@gmail.com)

MIT License

This product is almost based on Brian Carlson's [node-postgres](https://github.com/brianc/node-postgres).

Copyright (c) 2010 Brian Carlson (brian.m.carlson@gmail.com)

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
