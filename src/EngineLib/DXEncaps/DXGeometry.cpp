#include "pch.h"
#include "DXGeometry.h"

#include <ranges>
DXGeometry::DXGeometry() { this->UnInit(); }
DXGeometry::~DXGeometry() { DESTRUCTOR_UNINIT(m_Initialized); }
bool DXGeometry::Init() {

	if (m_Initialized)
		return false;

	ret m_Initialized = true;
}

void DXGeometry::UnInit() {

	for (auto geometry : m_Geometries | std::views::values) {
		delete geometry;
	}
	m_Geometries.clear();
	
	m_Initialized = false;
}

IGeometryWrapper& DXGeometry::GetGeomtry(GeometryType geometryType) {

	if (m_Geometries.contains(geometryType)) {
		return *m_Geometries[geometryType];
	}
	
	switch (geometryType) {
	case GeometryType::CUBE:
		m_Geometries[geometryType] = new GeometryWrapper<
			GeometryTraits<GeometryType::CUBE>::V,
			GeometryTraits<GeometryType::CUBE>::I
		>();
		break;
	case GeometryType::PYRAMID_SQUARE:
		m_Geometries[geometryType] = new GeometryWrapper<
			GeometryTraits<GeometryType::PYRAMID_SQUARE>::V,
			GeometryTraits<GeometryType::PYRAMID_SQUARE>::I
		>();
		break;
	
	case GeometryType::VALUE_COUNT:
	default:
		std::cerr << "Error: DXGeometry::GetGeomtry() - GeometryType not found!" << std::endl;
	}

	return *m_Geometries[geometryType];
}