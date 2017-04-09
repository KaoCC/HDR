#include "DebevecWeight.hpp"

#include <iostream>

namespace HDRI {


	const int DebevecWeight::kZmin = 0;
	const int DebevecWeight::kZmax = 255;
	const int DebevecWeight::N = 256;



	DebevecWeight::DebevecWeight() {

		mTable.resize(N);
		for (size_t i = 0; i < N; ++i) {

			if (i <= (0.5 * (kZmax + kZmin))) {
				mTable[i] = i - kZmin;
			} else {
				mTable[i] = kZmax - i;
			}

		}

		std::cerr << N << std::endl;

	}

	size_t DebevecWeight::getSize() const {
		return N;
	}




	double DebevecWeight::getWeight(int index) const {

		// cap ?

		if (index < kZmin || index > kZmax) {
			return 0;
		} else {
			return mTable[index];
		}
	}


}

