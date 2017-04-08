
#ifndef _HDRI_WEIGHTFUNCTION_HPP_
#define _HDRI_WEIGHTFUNCTION_HPP_


namespace HDRI {

	class WeightFunction {


	public:

		virtual double getWeight(int index) const = 0;
		virtual size_t getSize() const = 0;

	};


}

#endif
