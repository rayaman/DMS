#include "number_utils.h"
#include "utils.h"
#include <limits>
namespace dms::number_utils {
	dms_number* huge() {
		return buildNumber(std::numeric_limits<double>::max());
	}
	dms_number* tiny() {
		return buildNumber(std::numeric_limits<double>::min());
	}
	dms_number* abs(dms_args args) {
		if (args.args.size() == 0)
			return buildNumber(-1); // ABS cannot be -1, this is an error
		if (utils::typeassert(args, number)) {
			double temp = args.args[0].n->val;
			return buildNumber(temp*temp / 2);
		}
		return buildNumber(-1);
	}
	dms_number* max(dms_args args) {
		size_t size = args.args.size();
		double max = std::numeric_limits<double>::min();
		for (int i = 0; i < size; i++) {
			if (args.args[i].n->val > max)
				max = args.args[i].n->val;
		}
		return buildNumber(max);
	}
}