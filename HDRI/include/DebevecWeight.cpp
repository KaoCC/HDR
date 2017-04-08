#include "DebevecWeight.h"

#include <iostream>

namespace HDRI {


	const int DebevecWeight::kZmin = 0;
	const int DebevecWeight::kZmax = 255;
	const int DebevecWeight::N = 256;



	DebevecWeight::DebevecWeight() {

		mTable.resize(N);
		for (size_t i = 0; i < N; ++i) {

			if (0.5 * (kZmax + kZmin) > i) {
				mTable[i] = i - kZmin;
			} else {
				mTable[i] = kZmax - i;
			}

		}

		std::cerr << N << std::endl;

	}




	double DebevecWeight::getWeight(int index) {

		// cap ?

		if (index <= kZmin || index >= kZmax) {
			return 0;
		} else {
			return mTable[index];
		}
	}


}

