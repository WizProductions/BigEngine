#pragma once

struct IGeometryWrapper {

	int vertexCount;
	int indexCount;

	IGeometryWrapper(int _vertexCount, int _indexCount) :
		vertexCount(_vertexCount),
		indexCount(_indexCount) {}
	
};
//V = Vertices count | I = Indices count
template <int V, int I>
struct GeometryWrapper : IGeometryWrapper {
	std::array<Vertex, V> vertices;
	std::array<std::uint16_t, I> indices;

	GeometryWrapper() :
		IGeometryWrapper(V, I) {}
	
};

template<GeometryType T>
	struct GeometryTraits;

//Geometry sizes
template<>
struct GeometryTraits<GeometryType::CUBE> {
	static constexpr int V = 8;
	static constexpr int I = 36;
};

template<>
struct GeometryTraits<GeometryType::PYRAMID_SQUARE> {
	static constexpr int V = 5;
	static constexpr int I = 18;
};

class DXGeometry {

//##############################################################################
//##------------------------------- ATTRIBUTES -------------------------------##
//##############################################################################


private:

	/* FLAGS */
	bool m_Initialized;

	std::unordered_map<GeometryType, IGeometryWrapper*> m_Geometries;


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

	IGeometryWrapper& GetGeomtry(GeometryType geometryType);
};