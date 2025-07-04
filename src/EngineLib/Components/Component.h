#pragma once

struct Component {

	//##############################################################################
	//##------------------------------- ATTRIBUTES -------------------------------##
	//##############################################################################

protected:

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################


public:

	/*----------< CONSTRUCTORS >----------*/

	/** Constructor */
	Component() = default;	
	virtual ~Component() = default;

	/*------------------------------------*/

	GETTER virtual SystemType GetComponentType() = 0;

};