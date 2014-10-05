{
	"targets": [
		{
			"target_name": "pgsync",
			"sources": [ 
				"src/pgsync.cc",
				"src/connection.cc"
			],
			'include_dirs': [
				'<!@(pg_config --includedir)',
				'<!(node -e "require(\'nan\')")'
			],
			'conditions': [
				['OS=="win"', {
					'libraries': ['<!@(pg_config --libdir)\\libpq']
				} ,{
					'libraries': ['-lpq -L<!@(pg_config --libdir)']
				}]
			]
		}
	]
}
