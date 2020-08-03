#include "utils.h"
namespace dms::utils {
	bool typeassert(dms_args args, datatypes t1, datatypes t2, datatypes t3, datatypes t4, datatypes t5, datatypes t6, datatypes t7, datatypes t8, datatypes t9, datatypes t10, datatypes t11, datatypes t12) {
		size_t size = args.args.size();
		datatypes types[12] = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12 };
		if(size >= 4)
			for (int i = 0; i < 4;i++) {
				if (args.args[i].type != types[i])
					return false;
			}
		else
			for (int i = 0; i < size; i++) {
				if (args.args[i].type != types[i])
					return false;
			}
		return true;
	}
}