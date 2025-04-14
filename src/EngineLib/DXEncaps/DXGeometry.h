#pragma once

class DXGeometry {
//##############################################################################
	//##------------------------------- ATTRIBUTES -------------------------------##
	//##############################################################################


private:

	/* FLAGS */
	bool m_Initialized;

	inline static std::unordered_map<GeometryType, MeshGeometry*> m_Geometries;


//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/
	DXGeometry();
	~DXGeometry();
/*------------------------------------*/

	bool Init();
	void UnInit();


/* GETTERS */

/* SETTERS */




/* OTHERS FUNCTIONS */

	static MeshGeometry* CreateGeometry(GeometryType geometryType);
	static MeshGeometry& GetGeometry(GeometryType geometryType);
	static const char* GetGeoDrawArgs(GeometryType geometryType);
	static void InitCubeGeo(MeshGeometry& geometry);

	inline static void (*geometryCreateFunctions[static_cast<int>(GeometryType::VALUE_COUNT)])(MeshGeometry& geometry) = {
		&DXGeometry::InitCubeGeo,
		nullptr,
		nullptr
	};
};