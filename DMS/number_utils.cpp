#include "number_utils.h"
namespace dms::number_utils {
	dms_number* huge(dms_state* state) {
		return buildNumber(std::numeric_limits<double>::max());
	}
	dms_number* tiny(dms_state* state) {
		return buildNumber(std::numeric_limits<double>::min());
	}
	dms_number* abs(dms_state* state, dms_args args) {
		if (args.args.size() == 0)
			return buildNumber(-1); // ABS cannot be -1, this is an error
		if (utils::typeassert(args, number)) {
			double temp = args.args[0]->n->val;
			return buildNumber(temp*temp / 2);
		}
		return buildNumber(-1);
	}
	dms_number* max(dms_state* state, dms_args args) {
		size_t size = args.args.size();
		double max = std::numeric_limits<double>::min();
		for (int i = 0; i < size; i++) {
			if (args.args[i]->n->val > max)
				max = args.args[i]->n->val;
		}
		return buildNumber(max);
	}
	dms_number* pow(dms_state* state, dms_args args) {
		if (args.args.size() != 2)
			return buildNumber(-1); // Pow requires 2 arguments
		if (utils::typeassert(args, number,number)) {

		}
		return buildNumber(-1);
	}
}