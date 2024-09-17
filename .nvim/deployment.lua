return {
	["srvwebstr01"] = {
		host = "srvwebstr01",
		mappings = {
			{
				["local"] = "",
				["remote"] = "/docker/other_data/shroomhub/",
			},
		},
		excludedPaths = {
			"hub/backend/tmp/",
			"hub/backend/.env",
			"hub/frontend/node_modules/",
			"sensor/",
		},
	},
}
