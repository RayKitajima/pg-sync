{
	"targets": [
		{
			"target_name": "pgsync",
			"sources": [ 
				"src/pgsync.cc",
				"src/connection.cc"
			],
			'include_dirs': ['<!@(pg_config --includedir)'],
			'libraries' : ['-lpq -L<!@(pg_config --libdir)']
		}
	]
}
